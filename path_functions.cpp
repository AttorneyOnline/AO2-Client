#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QRegExp>

#ifdef BASE_OVERRIDE
#include "base_override.h"
#endif

//this is a quite broad generalization
//the most common OSes(mac and windows) are _usually_ case insensitive
//however, there do exist mac installations with case sensitive filesystems
//in that case, define CASE_SENSITIVE_FILESYSTEM and compile on a mac
#if (defined (LINUX) || defined (__linux__))
#define CASE_SENSITIVE_FILESYSTEM
#endif

QString base_path = "";

QString AOApplication::get_base_path()
{
    if (base_path == "")
    {
#ifdef BASE_OVERRIDE
  base_path = base_override;
#elif defined(ANDROID)
        QString sdcard_storage = getenv("SECONDARY_STORAGE");
        if (dir_exists(sdcard_storage + "/AO2/")){
            base_path = sdcard_storage + "/AO2/";
        }else{
            QString external_storage = getenv("EXTERNAL_STORAGE");
            base_path = external_storage + "/AO2/";
        }
#else
  base_path = QDir::currentPath() + "/base/";
#endif
}
    return base_path;
}

QString AOApplication::get_data_path()
{
  return get_base_path() + "data/";
}

QString AOApplication::get_theme_path()
{
  return get_base_path() + "themes/" + current_theme.toLower() + "/";
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
  QString default_path = "misc/demothings/";
  QString alt_path = "misc/RosterImages";
  if (dir_exists(default_path))
    return get_base_path() + default_path;
  else if (dir_exists(alt_path))
    return get_base_path() + alt_path;
  else
    return get_base_path() + default_path;
}
QString AOApplication::get_sounds_path()
{
  return get_base_path() + "sounds/general/";
}
QString AOApplication::get_music_path(QString p_song)
{
#ifndef CASE_SENSITIVE_FILESYSTEM
  return get_base_path() + "sounds/music/" + p_song;
#else
  return get_case_sensitive_path(get_base_path() + "sounds/music/", p_song);
#endif
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
  return get_base_path() + "background/default/";
}

QString AOApplication::get_evidence_path()
{
    QString default_path = "evidence/";
    QString alt_path = "items/";
    if (dir_exists(default_path))
      return get_base_path() + default_path;
    else if (dir_exists(alt_path))
      return get_base_path() + alt_path;
    else
      return get_base_path() + default_path;
}

QString AOApplication::get_case_sensitive_path(QString p_dir, QString p_file) {
  qDebug() << "calling get_case_sensitive_path";
  QRegExp file_rx = QRegExp(p_file, Qt::CaseInsensitive);
  QStringList files = QDir(p_dir).entryList();
  int result = files.indexOf(file_rx);

  if (result != -1) {
    QString path = p_dir + files.at(result);
    qDebug()  << "returning " << path;
    return path;
  }
  //if nothing is found, let the caller handle the missing file
  return p_dir + p_file;
}

QString Courtroom::get_background_path()
{
  return ao_app->get_base_path() + "background/" + current_background.toLower() + "/";
}

QString Courtroom::get_default_background_path()
{
  return ao_app->get_base_path() + "background/default/";
}
