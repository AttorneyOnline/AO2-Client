#include "file_functions.h"

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
#elif defined(__APPLE__)
  base_path = QCoreApplication::applicationDirPath() + "/../../../base/";
#else
  base_path = QCoreApplication::applicationDirPath() + "/base/";
#endif
  return base_path;
}
