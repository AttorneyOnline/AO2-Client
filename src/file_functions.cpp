#include "file_functions.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

bool file_exists(QString file_path)
{
  if (file_path.isEmpty())
  {
    return false;
  }

  QFileInfo check_file(file_path);

  return check_file.exists() && check_file.isFile();
}

bool dir_exists(QString dir_path)
{
  if (dir_path == "")
  {
    return false;
  }

  QDir check_dir(dir_path);

  return check_dir.exists();
}

bool exists(QString p_path)
{
  QFile file(p_path);

  return file.exists();
}

QString get_app_path()
{
  QString path = QCoreApplication::applicationDirPath();

#ifdef Q_OS_ANDROID
  QString storage_path = qgetenv("SECONDARY_STORAGE");
  if (dir_exists(storage_path))
  {
    path = storage_path;
  }
  else
  {
    QString external_path = qgetenv("EXTERNAL_STORAGE");
    if (dir_exists(external_path))
    {
      path = external_path;
    }
  }
#endif

#ifdef Q_OS_LINUX
  QString app_path = qgetenv("APPIMAGE");
  if (!app_path.isEmpty())
  {
    path = QFileInfo(app_path).absoluteDir().path();
  }
#endif

#ifdef Q_OS_MAC
  path += "/../../..";
#endif

  if (path.endsWith(QDir::separator()))
  {
    path.chop(1);
  }

  return path;
}

QString get_base_path()
{
#ifdef Q_OS_MAC
  // The .app is relocatable and can run read-only under Gatekeeper App
  // Translocation, so the writable base lives in Application Support. Defaults
  // are seeded there from the bundle on first run (see seed_base_if_missing).
  return QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absoluteFilePath("base") + "/";
#else
  return QDir(get_app_path()).absoluteFilePath("base") + "/";
#endif
}

void seed_base_if_missing()
{
#ifdef Q_OS_MAC
  const QString user_base = get_base_path();
  if (dir_exists(user_base))
  {
    return;
  }

  // Read-only defaults shipped inside the bundle at Contents/Resources/base.
  const QString bundled_base = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../Resources/base");
  if (!dir_exists(bundled_base))
  {
    return; // e.g. a non-bundle run; nothing to seed
  }

  // ditto creates the destination tree and preserves symlinks/permissions.
  QProcess::execute("ditto", {bundled_base, user_base});
#endif
}
