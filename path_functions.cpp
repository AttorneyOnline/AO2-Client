#include "aoapplication.h"

#include "text_file_functions.h"

#include <QDir>
#include <QDebug>

QString AOApplication::get_base_path(){
  return (QDir::currentPath() + "/base/");
}

QString AOApplication::get_theme_path()
{
  return get_base_path() + "themes/" + user_theme.toLower() + "/";
}

QString AOApplication::get_default_theme_path()
{
  return get_base_path() + "themes/default/";
}

QString AOApplication::get_character_path(QString p_character)
{
  return get_base_path() + "characters/" + p_character.toLower() + "/";
}

QString AOApplication::get_demothings_path()
{
  return get_base_path() + "misc/demothings/";
}
