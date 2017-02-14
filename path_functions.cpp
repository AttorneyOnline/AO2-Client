#include "aoapplication.h"
#include "courtroom.h"

#ifndef OMNI_WIN_DEBUG
#define OMNI_WIN_DEBUG

#include <QDir>
#include <QDebug>
#endif

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
QString AOApplication::get_sounds_path()
{
  return get_base_path() + "sounds/general/";
}
QString AOApplication::get_music_path(QString p_song)
{
  return get_base_path() + "sounds/music/" + p_song.toLower();
}

QString AOApplication::get_background_path()
{
  if (courtroom_constructed)
    return w_courtroom->get_background_path();
  //this function being called when the courtroom isn't constructed makes no sense
  return "";
}

QString AOApplication::get_default_background_path()
{
  return get_base_path() + "background/gs4/";
}

QString Courtroom::get_background_path()
{
  return ao_app->get_base_path() + "background/" + current_background.toLower() + "/";
}

QString Courtroom::get_default_background_path()
{
  return ao_app->get_base_path() + "background/gs4/";
}
