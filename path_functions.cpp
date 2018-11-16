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

QString AOApplication::get_base_path()
{
  QString base_path = "";
#ifdef ANDROID
  QString sdcard_storage = getenv("SECONDARY_STORAGE");
  if (dir_exists(sdcard_storage + "/AO2/")){
    base_path = sdcard_storage + "/AO2/";
  }
  else {
    QString external_storage = getenv("EXTERNAL_STORAGE");
    base_path = external_storage + "/AO2/";
  }
#else
  base_path = QDir::currentPath() + "/base/";
#endif

  return base_path;
}

QString AOApplication::get_data_path()
{
  return get_base_path() + "data/";
}

QString AOApplication::get_default_theme_path(QString p_file)
{
  QString path = get_base_path() + "themes/default/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

//assume that the capitalization of the theme in config is correct
QString AOApplication::get_theme_path(QString p_file)
{
  QString path = get_base_path() + "themes/" + current_theme + "/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_character_path(QString p_character, QString p_file)
{
  QString char_path = get_base_path() + "characters/" + p_character;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return char_path + "/" + p_file;
#else
  //need two calls to get_case_sensitive_path because character folder name may be wrong as well as the filename
  return get_case_sensitive_path(
         get_case_sensitive_path(char_path) + "/" + p_file);
#endif
}

QString AOApplication::get_character_emotions_path(QString p_character, QString p_file)
{
  QString char_path = get_base_path() + "characters/" + p_character;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return char_path + "/emotions/" + p_file;
#else
  return get_case_sensitive_path(
         get_case_sensitive_path(char_path) + "/emotions/" + p_file);
#endif
}

QString AOApplication::get_sounds_path()
{
  QString path = get_base_path() + "sounds/general/";
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_music_path(QString p_song)
{
  QString path = get_base_path() + "sounds/music/" + p_song;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_background_path(QString p_file)
{
  QString bg_path = get_base_path() + "background/" + w_courtroom->get_current_background();
  if (courtroom_constructed) {
#ifndef CASE_SENSITIVE_FILESYSTEM
    return bg_path + "/" + p_file;
#else
    return get_case_sensitive_path(
           get_case_sensitive_path(bg_path) + "/" + p_file);
#endif
  }
  //this function being called when the courtroom isn't constructed makes no sense
  return "";
}

QString AOApplication::get_default_background_path(QString p_file)
{
  QString path = get_base_path() + "background/default/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_evidence_path(QString p_file)
{
  QString path = get_base_path() + "evidence/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString AOApplication::get_case_sensitive_path(QString p_file) {
  QFileInfo file(p_file);

  //quick check to see if it's actually there first
  if (file.exists()) return p_file;

  QString file_name = file.fileName();
  QString file_path = file.absolutePath();

  QRegExp file_rx = QRegExp(file_name, Qt::CaseInsensitive);
  QStringList files = QDir(file_path).entryList();
  int result = files.indexOf(file_rx);

  if (result != -1)
    return file_path + "/" + files.at(result);

  //if nothing is found, let the caller handle the missing file
  return p_file;
}
