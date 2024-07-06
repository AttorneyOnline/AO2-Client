#include "file_functions.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

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
  return QDir(get_app_path()).absoluteFilePath("base") + "/";
}
