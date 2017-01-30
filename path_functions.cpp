#include "aoapplication.h"
#include "courtroom.h"

#include <QDir>
#include <QDebug>

QString AOApplication::get_base_path(){

#ifdef OMNI_DEBUG
  return "/media/omnitroid/Data/winshare/AO/client/base/";
#else
  return (QDir::currentPath() + "/base/");
#endif

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

QString Courtroom::get_background_path()
{
  return ao_app->get_base_path() + "background/" + current_background.toLower() + "/";
}

QString Courtroom::get_default_background_path()
{
  return ao_app->get_base_path() + "background/gs4/";
}
