#include "mount_manager.h"

#include "file_functions.h"
#include "options.h"

#include <QApplication>
#include <QCollator>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSaveFile>
#include <QSet>
#include <QTextStream>
#include <QTimer>

#include <algorithm>

namespace {
constexpr int kScanDebounceMs = 200;
}

MountManager::MountManager(QObject *parent) : QObject(parent)
{
  m_watcher = new QFileSystemWatcher(this);
  connect(m_watcher, &QFileSystemWatcher::directoryChanged, this,
          &MountManager::onDirectoryChanged);
  connect(qApp, QOverload<Qt::ApplicationState>::of(
                    &QApplication::applicationStateChanged),
          this, &MountManager::onApplicationStateChanged);
}

void MountManager::initialize()
{
  m_packagesPath = QDir::cleanPath(get_packages_path());
  QDir().mkpath(m_packagesPath);
  m_mountsFilePath = QDir(m_packagesPath).filePath("mounts.txt");

  const bool fileExists = QFileInfo::exists(m_mountsFilePath);
  QStringList loaded;
  const bool readOk = fileExists && readMountsFile(loaded);

  if (!readOk) {
    // Since we don't have a mounts file yet, create one from the
    // legacy config.ini mounts.
    loaded = Options::getInstance().legacyMountPaths();
  }

  m_entries = sanitizedEntries(loaded);
  m_resolvedPathsValid = false;

  // Force the packages/ folder to write a mounts.txt file if
  // initialization had to port the legacy config.
  scanPackages(!readOk);
  refreshWatchPath();
}

QStringList MountManager::entries() const { return m_entries; }

QStringList MountManager::resolvedMountPaths() const
{
  if (!m_resolvedPathsValid) {
    m_resolvedPathsCache.clear();
    m_resolvedPathsCache.reserve(m_entries.size());
    for (const QString &entry : m_entries) {
      m_resolvedPathsCache.append(resolveEntry(entry));
    }
    m_resolvedPathsValid = true;
  }
  return m_resolvedPathsCache;
}

bool MountManager::hasPackagesPrefix(const QString &entry)
{
#ifdef Q_OS_WIN
  return entry.startsWith("packages/", Qt::CaseInsensitive);
#else
  return entry.startsWith("packages/");
#endif
}

QString MountManager::packageNameFromEntry(const QString &entry) const
{
  if (!hasPackagesPrefix(entry)) {
    return {};
  }

  const QString name = entry.mid(QStringLiteral("packages/").size());
  if (name.isEmpty() || name == "." || name == ".." ||
      name.contains(QLatin1Char('/'))) {
    return {};
  }
  return name;
}

bool MountManager::isPackageEntry(const QString &entry) const
{
  return !packageNameFromEntry(normalizedEntry(entry)).isEmpty();
}

bool MountManager::containsEntry(const QString &entry) const
{
  const QString normalized = normalizedEntry(entry);
  if (normalized.isEmpty()) {
    return false;
  }
  const QString key = dedupeKey(normalized);
  for (const QString &existing : m_entries) {
    if (dedupeKey(existing) == key) {
      return true;
    }
  }
  return false;
}

QString MountManager::normalizedEntry(const QString &entry)
{
  QString clean = entry.trimmed();
  if (clean.isEmpty() || clean.startsWith(QLatin1Char('#'))) {
    return {};
  }
  clean = QDir::fromNativeSeparators(clean);
  clean = QDir::cleanPath(clean);
  if (clean == ".") {
    return {};
  }
  return clean;
}

QString MountManager::resolveEntry(const QString &entry)
{
  const QString clean = QDir::cleanPath(entry);
  if (QDir::isAbsolutePath(clean)) {
    return clean;
  }
  return QDir::cleanPath(get_app_dir_path() + QLatin1Char('/') + clean);
}

QString MountManager::dedupeKey(const QString &entry)
{
  QString key = QDir::cleanPath(resolveEntry(entry));
#ifdef Q_OS_WIN
  key = key.toLower();
#endif
  return key;
}

QStringList MountManager::sanitizedEntries(const QStringList &entries) const
{
  QStringList result;
  QSet<QString> seen;
  for (const QString &raw : entries) {
    const QString entry = normalizedEntry(raw);
    if (entry.isEmpty()) {
      continue;
    }
    const QString key = dedupeKey(entry);
    if (seen.contains(key)) {
      continue;
    }
    seen.insert(key);
    result.append(entry);
  }
  return result;
}

QString MountManager::entryForPath(const QString &path) const
{
  QFileInfo info(path);
  if (!info.exists() || !info.isDir()) {
    return {};
  }

  const QString absolute = QDir::cleanPath(info.absoluteFilePath());
  const QString packagesRoot = QDir::cleanPath(m_packagesPath);

  // Anything inside the packages/ folder is considered a proper package.
  // Anything outside the packages/ folder written in mounts.txt is considered external!
  const QString packageRelative =
      QDir(packagesRoot).relativeFilePath(absolute);
  if (!packageRelative.isEmpty() && packageRelative != "." &&
      packageRelative != ".." && !packageRelative.startsWith(QStringLiteral("..")) &&
      !packageRelative.contains(QLatin1Char('/'))) {
    return QStringLiteral("packages/") + packageRelative;
  }

  // Prefer to display relative paths when we can help it.
  // Anything going outside the app dir just gets the full (absolute) path instead.
  QDir appDir(get_app_dir_path());
  const QString appRelative = appDir.relativeFilePath(absolute);
  if (!appRelative.isEmpty() && appRelative != "." &&
      !appRelative.startsWith(QStringLiteral(".."))) {
    return QDir::cleanPath(appRelative);
  }

  return absolute;
}

bool MountManager::setEntries(const QStringList &entries)
{
  const QStringList sanitized = sanitizedEntries(entries);
  if (sanitized == m_entries) {
    return false;
  }

  m_entries = sanitized;
  m_resolvedPathsValid = false;
  if (!writeMountsFile(m_entries)) {
    qWarning() << "Unable to write mount order to" << m_mountsFilePath;
  }
  emit changed();
  return true;
}

QString MountManager::packagesPath() const { return m_packagesPath; }

void MountManager::rescanNow()
{
  m_scanPending = false;
  if (!QDir(m_packagesPath).exists()) {
    QDir().mkpath(m_packagesPath);
  }
  refreshWatchPath();
  scanPackages(false);
}

bool MountManager::readMountsFile(QStringList &entries) const
{
  QFile file(m_mountsFilePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  in.setCodec("UTF-8");
#endif

  entries.clear();
  while (!in.atEnd()) {
    const QString line = in.readLine();
    const QString entry = normalizedEntry(line);
    if (!entry.isEmpty()) {
      entries.append(entry);
    }
  }
  return true;
}

bool MountManager::writeMountsFile(const QStringList &entries) const
{
  QSaveFile file(m_mountsFilePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return false;
  }

  QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  out.setCodec("UTF-8");
#endif
  for (const QString &entry : entries) {
    out << entry << '\n';
  }
  return file.commit();
}

bool MountManager::scanPackages(bool forceWrite)
{
  if (!QDir(m_packagesPath).exists()) {
    QDir().mkpath(m_packagesPath);
  }

  const QStringList previous = m_entries;
  QStringList updated;
  QSet<QString> presentPackages;

  for (const QString &entry : m_entries) {
    const QString packageName = packageNameFromEntry(entry);
    if (packageName.isEmpty()) {
      updated.append(entry);
      continue;
    }

    if (packageName.startsWith(QLatin1Char('.'))) {
      continue;
    }

    if (QFileInfo(QDir(m_packagesPath).filePath(packageName)).isDir()) {
      updated.append(entry);
#ifdef Q_OS_WIN
      presentPackages.insert(packageName.toLower());
#else
      presentPackages.insert(packageName);
#endif
    }
  }

  QFileInfoList folders = QDir(m_packagesPath).entryInfoList(
      QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);

  QCollator collator;
  collator.setNumericMode(true);
  collator.setCaseSensitivity(Qt::CaseInsensitive);
  std::sort(folders.begin(), folders.end(),
            [&collator](const QFileInfo &a, const QFileInfo &b) {
              return collator.compare(a.fileName(), b.fileName()) < 0;
            });

  for (const QFileInfo &folder : std::as_const(folders)) {
    const QString name = folder.fileName();

    if (name.startsWith(QLatin1Char('.'))) {
      continue;
    }
#ifdef Q_OS_WIN
    const QString key = name.toLower();
#else
    const QString key = name;
#endif
    if (presentPackages.contains(key)) {
      continue;
    }
    updated.append(QStringLiteral("packages/") + name);
    presentPackages.insert(key);
  }

  m_entries = updated;
  m_resolvedPathsValid = false;
  const bool listChanged = previous != m_entries;

  if (listChanged || forceWrite) {
    if (!writeMountsFile(m_entries)) {
      qWarning() << "Unable to write mount order to" << m_mountsFilePath;
    }
  }

  if (listChanged) {
    emit changed();
  }
  return listChanged;
}

void MountManager::scheduleScan()
{
  m_scanPending = true;

  if (QApplication::applicationState() != Qt::ApplicationActive) {
    return;
  }
  if (m_scanScheduled) {
    return;
  }

  m_scanScheduled = true;
  QTimer::singleShot(kScanDebounceMs, this, [this] {
    m_scanScheduled = false;
    if (!m_scanPending) {
      return;
    }
    if (QApplication::applicationState() != Qt::ApplicationActive) {
      return;
    }
    m_scanPending = false;
    refreshWatchPath();
    scanPackages(false);
  });
}

void MountManager::refreshWatchPath()
{
  if (m_watcher == nullptr) {
    return;
  }
  if (!QDir(m_packagesPath).exists()) {
    return;
  }
  if (!m_watcher->directories().contains(m_packagesPath)) {
    m_watcher->addPath(m_packagesPath);
  }
}

void MountManager::onDirectoryChanged(const QString &path)
{
  Q_UNUSED(path);
  scheduleScan();
}

void MountManager::onApplicationStateChanged(Qt::ApplicationState state)
{
  if (state != Qt::ApplicationActive) {
    return;
  }

  m_scanPending = false;
  refreshWatchPath();
  scanPackages(false);
}
