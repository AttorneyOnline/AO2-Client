#include "path_functions.h"

#include "global_variables.h"
#include "text_file_functions.h"

#include <QDir>
#include <QDebug>

QString get_base_path(){
  return (QDir::currentPath() + "/base/");
}

QString get_theme_path()
{
  return get_base_path() + "themes/" + g_user_theme.toLower() + "/";
}

QString get_default_theme_path()
{
  return get_base_path() + "themes/default/";
}
