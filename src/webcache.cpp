#include "webcache.h"

#include "aoapplication.h"
#include "file_functions.h"
#include "options.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkRequest>
#include <QUrl>

namespace
{
// Lowercase path components (for local storage)
QString lowercasePath(const QString &path)
{
  QStringList components = path.split('/');
  QStringList result;
  for (const QString &component : components)
  {
    if (component.isEmpty())
    {
      continue;
    }
    result.append(component.toLower());
  }
  return result.join('/');
}

// URL-encode path for web requests (like webAO's encodeURI)
// Uses encodeURI-compatible encoding (preserves safe characters like ! ' ( ) *)
QString urlEncodePath(const QString &path)
{
  QStringList components = path.split('/');
  QStringList encoded;
  // Characters that encodeURI does NOT encode (excluding / which we handle via split)
  // These are: A-Za-z0-9 ; , ? : @ & = + $ - _ . ! ~ * ' ( ) #
  const QByteArray safeChars = ";,?:@&=+$-_.!~*'()#";
  for (const QString &component : components)
  {
    if (component.isEmpty())
    {
      continue;
    }
    QString percentEncoded = QUrl::toPercentEncoding(component, safeChars);
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
{}

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

QString WebCache::getCachedPath(const QString &relativePath, const QStringList &suffixes) const
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

  // Try each suffix
  QStringList effectiveSuffixes = suffixes.isEmpty() ? QStringList{""} : suffixes;
  for (const QString &suffix : effectiveSuffixes)
  {
    // Use lowercase path for cache lookup (no percent-encoding in local paths)
    QString lowerPath = lowercasePath(relativePath + suffix);
    QString localPath = cacheDir() + subdir + lowerPath;

    if (!file_exists(localPath))
    {
      continue;
    }

    if (isExpired(localPath))
    {
      continue;
    }

    return localPath;
  }

  return QString();
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

    // Lowercase path for local storage and tracking (no percent-encoding)
    QString lowerPath = lowercasePath(fullPath);

    // Check if already downloading this path
    if (m_pending_downloads.contains(lowerPath))
    {
      return;
    }

    // Check if this path previously failed (don't retry within this session)
    if (m_failed_downloads.contains(lowerPath))
    {
      continue; // Try next suffix
    }

    // Local path uses lowercase without percent-encoding
    QString localPath = cacheDir() + subdir + lowerPath;

    // Skip if already cached and not expired
    if (file_exists(localPath) && !isExpired(localPath))
    {
      return; // Already have a valid cached file
    }

    // Remote URL uses percent-encoded lowercase path
    QString remoteUrl = assetUrl + urlEncodePath(lowerPath);

    // Mark as pending and start download
    m_pending_downloads.insert(lowerPath, true);
    startDownload(remoteUrl, localPath, lowerPath);
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
