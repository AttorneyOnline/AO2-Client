#include <QFileInfo>

#include "file_functions.h"

bool file_exists(QString file_path)
{
  QFileInfo check_file(file_path);

  return check_file.exists() && check_file.isFile();
}
