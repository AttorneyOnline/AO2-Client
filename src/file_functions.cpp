#include "file_functions.h"

#include <QFile>
#include <QProcess>
#include <QStandardPaths>

bool file_exists(QString file_path)
{
  if (file_path.isEmpty())
    return false;

  QFileInfo check_file(file_path);

  return check_file.exists() && check_file.isFile();
}

bool dir_exists(QString dir_path)
{
  if (dir_path == "")
      return false;

  QDir check_dir(dir_path);

  return check_dir.exists();
}

bool exists(QString p_path)
{
  QFile file(p_path);

  return file.exists();
}

QString get_app_dir_path()
{
#ifdef Q_OS_MACOS
  return QCoreApplication::applicationDirPath() + "/../../../base/";
#elif defined(Q_OS_LINUX)
  // When running from an AppImage, $APPIMAGE points to the .AppImage file.
  // Put "base/" next to the .AppImage so users can drop assets beside it.
  QByteArray appimage = qgetenv("APPIMAGE");
  if (!appimage.isEmpty()) {
    QFileInfo appimageInfo(QString::fromLocal8Bit(appimage));
    return appimageInfo.absoluteDir().absolutePath();
  }
#endif
  return QCoreApplication::applicationDirPath();
}

QString get_base_path()
{
  QString base_path = "";
#ifdef ANDROID
  QString sdcard_storage = getenv("SECONDARY_STORAGE");
  if (dir_exists(sdcard_storage + "/base/")) {
    base_path = sdcard_storage + "/base/";
  }
  else {
    QString external_storage = getenv("EXTERNAL_STORAGE");
    base_path = external_storage + "/base/";
  }
#else
  base_path = get_app_dir_path() + "/base/";
#endif
  return base_path;
}

QString get_packages_path()
{
  QDir base_dir(get_base_path());
  if (base_dir.cdUp()) {
    return base_dir.absolutePath() + "/packages/";
  }
  return get_app_dir_path() + "/packages/";
}

void seed_base_if_missing()
{
#ifdef Q_OS_MACOS
  const QString user_base = get_base_path();
  if (dir_exists(user_base))
    return;

  // The default base is shipped read-only inside the app bundle. Copy it to
  // the writable per-user location on first launch.
  const QString bundled_base = QDir::cleanPath(
      QCoreApplication::applicationDirPath() + "/../Resources/base");
  if (!dir_exists(bundled_base))
    return;

  QDir().mkpath(QFileInfo(user_base).absolutePath());
  QProcess::execute("ditto", {bundled_base, user_base});
#endif
}

bool copyDirectoryRecursive(const QString &sourcePath, const QString &destPath)
{
  if (!QDir().mkpath(destPath)) {
    return false;
  }

  const QFileInfoList entries = QDir(sourcePath).entryInfoList(
      QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);

  for (const QFileInfo &info : entries) {
    const QString target = QDir(destPath).filePath(info.fileName());
    if (info.isSymLink()) {
      if (!QFile::link(info.symLinkTarget(), target)) {
        return false;
      }
    }
    else if (info.isDir()) {
      if (!copyDirectoryRecursive(info.absoluteFilePath(), target)) {
        return false;
      }
    }
    else if (!QFile::copy(info.absoluteFilePath(), target)) {
      return false;
    }
  }
  return true;
}

bool moveDirectoryInto(const QString &sourcePath, const QString &destPath,
                       QString *error)
{
  if (QFileInfo::exists(destPath)) {
    if (error != nullptr) {
      *error = QObject::tr(
          "A folder with that name already exists in the packages folder.");
    }
    return false;
  }

  // Refuse to move a folder into itself (for example, selecting the app
  // folder when packages/ lives inside it).
  const QString sourceCanonical = QDir(sourcePath).canonicalPath();
  const QString destParentCanonical =
      QDir(QFileInfo(destPath).absolutePath()).canonicalPath();
  if (!sourceCanonical.isEmpty() && !destParentCanonical.isEmpty() &&
      (destParentCanonical == sourceCanonical ||
       destParentCanonical.startsWith(sourceCanonical + QLatin1Char('/')))) {
    if (error != nullptr) {
      *error = QObject::tr("A folder can't be moved into itself.");
    }
    return false;
  }

  if (QDir().rename(sourcePath, destPath)) {
    return true;
  }

  // rename() cannot move between different drives/filesystems. Falling back
  // to copy-then-delete keeps the Move option useful for those folders.
  if (!copyDirectoryRecursive(sourcePath, destPath)) {
    QDir(destPath).removeRecursively();
    if (error != nullptr) {
      *error = QObject::tr("Couldn't copy the folder into packages.");
    }
    return false;
  }

  if (!QDir(sourcePath).removeRecursively()) {
    // The copy is complete and usable. Leave the original in place rather
    // than deleting anything else, but let the user know.
    qWarning() << "Copied" << sourcePath << "to" << destPath
               << "but could not remove the original folder";
  }
  return true;
}

bool moveFileInto(const QString &sourcePath, const QString &destPath,
                  QString *error)
{
  if (QFile::rename(sourcePath, destPath)) {
    return true;
  }

  if (!QFile::copy(sourcePath, destPath)) {
    if (error != nullptr) {
      *error = QObject::tr("Couldn't move %1.")
                   .arg(QFileInfo(sourcePath).fileName());
    }
    return false;
  }

  if (!QFile::remove(sourcePath)) {
    qWarning() << "Copied" << sourcePath << "to" << destPath
               << "but could not remove the original file";
  }
  return true;
}

bool moveDirectoryContentsInto(const QString &sourcePath,
                               const QString &destPath, QString *error,
                               bool overwrite)
{
  if (!QDir().mkpath(destPath)) {
    if (error != nullptr) {
      *error = QObject::tr("Couldn't create the destination folder.");
    }
    return false;
  }

  const QFileInfoList entries = QDir(sourcePath).entryInfoList(
      QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System,
      QDir::Name);
  for (const QFileInfo &entry : entries) {
    const QString destination = QDir(destPath).filePath(entry.fileName());
    if (entry.isDir()) {
      if (!overwrite) {
        if (!moveDirectoryInto(entry.absoluteFilePath(), destination, error)) {
          return false;
        }
        continue;
      }
      if (QFileInfo::exists(destination) && !QFileInfo(destination).isDir()) {
        QFile::remove(destination);
      }
      const bool merged =
          QFileInfo(destination).isDir()
              ? moveDirectoryContentsInto(entry.absoluteFilePath(), destination,
                                          error, overwrite)
              : moveDirectoryInto(entry.absoluteFilePath(), destination, error);
      if (!merged) {
        return false;
      }
    }
    else {
      if (overwrite && QFileInfo::exists(destination)) {
        if (QFileInfo(destination).isDir())
          QDir(destination).removeRecursively();
        else
          QFile::remove(destination);
      }
      if (!moveFileInto(entry.absoluteFilePath(), destination, error)) {
        return false;
      }
    }
  }
  return true;
}