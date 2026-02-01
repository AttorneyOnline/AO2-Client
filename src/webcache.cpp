#include "webcache.h"

#include "aoapplication.h"
#include "file_functions.h"
#include "options.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkRequest>
#include <QUrl>

namespace {
// Normalize path like webAO: lowercase and URL-encode each component
// Uses encodeURI-compatible encoding (preserves safe characters like ! ' ( ) *)
QString normalizePathForWeb(const QString &path)
{
  // Split path into components, lowercase and URL-encode each, then rejoin
  QStringList components = path.split('/');
  QStringList encoded;
  // Characters that encodeURI does NOT encode (excluding / which we handle via split)
  // These are: A-Za-z0-9 ; , ? : @ & = + $ - _ . ! ~ * ' ( ) #
  // We only pass the non-alphanumeric ones since alphanumerics are never encoded
  const QByteArray safeChars = ";,?:@&=+$-_.!~*'()#";
  for (const QString &component : components)
  {
    if (component.isEmpty())
    {
      continue;
    }
    // Lowercase and URL-encode (percent-encode) the component
    QString lowered = component.toLower();
    QString percentEncoded = QUrl::toPercentEncoding(lowered, safeChars);
    encoded.append(percentEncoded);
  }
  return encoded.join('/');
}
} // namespace

WebCache::WebCache(AOApplication *parent)
    : QObject(parent)
    , ao_app(parent)
    , m_network_manager(new QNetworkAccessManager(this))
{
  connect(m_network_manager, &QNetworkAccessManager::finished, this, &WebCache::onDownloadFinished);
}

WebCache::~WebCache()
{
}

QString WebCache::cacheDir() const
{
  return get_base_path() + "webcache/";
}

QString WebCache::cacheSubdir() const
{
  QString assetUrl = ao_app->m_serverdata.get_asset_url();
  if (assetUrl.isEmpty())
  {
    return QString();
  }

  // Extract host and path from asset URL (e.g., "https://direct.grave.wine/base/" -> "direct.grave.wine/base/")
  QUrl url(assetUrl);
  QString subdir = url.host() + url.path();

  // Ensure it ends with /
  if (!subdir.endsWith('/'))
  {
    subdir += '/';
  }

  return subdir;
}

QString WebCache::getCachedPath(const QString &relativePath) const
{
  if (relativePath.isEmpty())
  {
    return QString();
  }

  QString subdir = cacheSubdir();
  if (subdir.isEmpty())
  {
    return QString();
  }

  // Use normalized (lowercase) path for cache lookup
  QString normalizedPath = normalizePathForWeb(relativePath);
  QString localPath = cacheDir() + subdir + normalizedPath;

  if (!file_exists(localPath))
  {
    return QString();
  }

  if (isExpired(localPath))
  {
    return QString();
  }

  return localPath;
}

bool WebCache::isExpired(const QString &localPath) const
{
  QFileInfo fileInfo(localPath);
  if (!fileInfo.exists())
  {
    return true;
  }

  int expiryHours = Options::getInstance().webcacheExpiryHours();
  QDateTime expiryTime = fileInfo.lastModified().addSecs(expiryHours * 3600);

  return QDateTime::currentDateTime() > expiryTime;
}

void WebCache::resolveOrDownload(const QString &relativePath, const QStringList &suffixes)
{
  if (relativePath.isEmpty())
  {
    return;
  }

  // Reject paths containing absolute paths (they shouldn't be passed to webcache)
  // Check for Unix absolute paths embedded anywhere, or Windows drive letters
  if (relativePath.startsWith('/') || relativePath.contains("//") || relativePath.contains(":/"))
  {
    return;
  }

  // Check if webcache is enabled
  if (!Options::getInstance().webcacheEnabled())
  {
    return;
  }

  // Check if server has an asset URL
  QString assetUrl = ao_app->m_serverdata.get_asset_url();
  if (assetUrl.isEmpty())
  {
    return;
  }

  // Ensure asset URL ends with /
  if (!assetUrl.endsWith('/'))
  {
    assetUrl += '/';
  }

  // Get cache subdirectory for this server's asset URL
  QString subdir = cacheSubdir();
  if (subdir.isEmpty())
  {
    return;
  }

  // Try each suffix (like webAO tries multiple extensions)
  QStringList effectiveSuffixes = suffixes.isEmpty() ? QStringList{""} : suffixes;
  for (const QString &suffix : effectiveSuffixes)
  {
    QString fullPath = relativePath + suffix;

    // Normalize path like webAO: lowercase and URL-encode
    QString normalizedPath = normalizePathForWeb(fullPath);

    // Check if already downloading this path
    if (m_pending_downloads.contains(normalizedPath))
    {
      return;
    }

    // Check if this path previously failed (don't retry within this session)
    if (m_failed_downloads.contains(normalizedPath))
    {
      continue; // Try next suffix
    }

    QString localPath = cacheDir() + subdir + normalizedPath;

    // Skip if already cached and not expired
    if (file_exists(localPath) && !isExpired(localPath))
    {
      return; // Already have a valid cached file
    }

    // Construct remote URL with normalized path
    QString remoteUrl = assetUrl + normalizedPath;

    // Mark as pending and start download
    m_pending_downloads.insert(normalizedPath, true);
    startDownload(remoteUrl, localPath, normalizedPath);
    return; // Only try one suffix at a time
  }
}

void WebCache::startDownload(const QString &remoteUrl, const QString &localPath, const QString &relativePath)
{
  QNetworkRequest request{QUrl(remoteUrl)};
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

  // Store the local path and relative path in the request for later retrieval
  request.setAttribute(QNetworkRequest::User, localPath);
  request.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), relativePath);

  qDebug() << "WebCache: Downloading" << remoteUrl;
  m_network_manager->get(request);
}

void WebCache::onDownloadFinished(QNetworkReply *reply)
{
  QString localPath = reply->request().attribute(QNetworkRequest::User).toString();
  QString relativePath = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1)).toString();

  // Remove from pending
  m_pending_downloads.remove(relativePath);

  if (reply->error() != QNetworkReply::NoError)
  {
    qDebug() << "WebCache: Download failed for" << reply->url().toString() << "-" << reply->errorString();
    m_failed_downloads.insert(relativePath);
    reply->deleteLater();
    return;
  }

  // Check HTTP status code
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  if (statusCode != 200)
  {
    qDebug() << "WebCache: Download returned status" << statusCode << "for" << reply->url().toString();
    m_failed_downloads.insert(relativePath);
    reply->deleteLater();
    return;
  }

  // Create directory structure
  QFileInfo fileInfo(localPath);
  QDir dir = fileInfo.absoluteDir();
  if (!dir.exists())
  {
    if (!dir.mkpath("."))
    {
      qWarning() << "WebCache: Failed to create directory" << dir.absolutePath();
      reply->deleteLater();
      return;
    }
  }

  // Write the file
  QFile file(localPath);
  if (!file.open(QIODevice::WriteOnly))
  {
    qWarning() << "WebCache: Failed to open file for writing:" << localPath;
    reply->deleteLater();
    return;
  }

  file.write(reply->readAll());
  file.close();

  qDebug() << "WebCache: Successfully cached" << localPath;
  reply->deleteLater();
}

void WebCache::clearCache()
{
  QDir dir(cacheDir());
  if (dir.exists())
  {
    if (!dir.removeRecursively())
    {
      qWarning() << "WebCache: Failed to clear cache directory";
    }
    else
    {
      qDebug() << "WebCache: Cache cleared";
    }
  }
}

qint64 WebCache::getCacheSize() const
{
  QDir dir(cacheDir());
  if (!dir.exists())
  {
    return 0;
  }
  return calculateDirSize(dir);
}

qint64 WebCache::calculateDirSize(const QDir &dir) const
{
  qint64 size = 0;

  QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
  for (const QFileInfo &fileInfo : fileList)
  {
    size += fileInfo.size();
  }

  QFileInfoList dirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QFileInfo &dirInfo : dirList)
  {
    size += calculateDirSize(QDir(dirInfo.absoluteFilePath()));
  }

  return size;
}
