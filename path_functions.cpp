#include "text_file_functions.h"
#include "courtroom.h"

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

QString TextFileHandler::get_base_path()
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

QString TextFileHandler::get_data_path()
{
  return get_base_path() + "data/";
}

QString TextFileHandler::get_default_theme_path(QString p_file)
{
  QString path = get_base_path() + "themes/default/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString TextFileHandler::get_custom_theme_path(QString p_theme, QString p_file)
{
  QString path = get_base_path() + "themes/" + p_theme + "/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString TextFileHandler::get_theme_path(QString p_file)
{
  QString path = get_base_path() + "themes/" + current_theme + "/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString TextFileHandler::get_character_path(QString p_char, QString p_file)
{
  QString path = get_base_path() + "characters/" + p_char + "/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString TextFileHandler::get_sounds_path(QString p_file)
{
  QString path = get_base_path() + "sounds/general/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString TextFileHandler::get_music_path(QString p_song)
{
  QString path = get_base_path() + "sounds/music/" + p_song;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString TextFileHandler::get_background_path(QString p_file)
{
  QString path = get_base_path() + "background/" + ao_app->w_courtroom->get_current_background() + "/" + p_file;
  if (ao_app->courtroom_constructed) {
#ifndef CASE_SENSITIVE_FILESYSTEM
    return path;
#else
    return get_case_sensitive_path(path);
#endif
  }
  return get_default_background_path(p_file);
}

QString TextFileHandler::get_default_background_path(QString p_file)
{
  QString path = get_base_path() + "background/default/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString TextFileHandler::get_evidence_path(QString p_file)
{
  QString path = get_base_path() + "evidence/" + p_file;
#ifndef CASE_SENSITIVE_FILESYSTEM
  return path;
#else
  return get_case_sensitive_path(path);
#endif
}

QString TextFileHandler::get_case_sensitive_path(QString p_file) {
  //first, check to see if it's actually there (also serves as base case for recursion)
  if (exists(p_file)) return p_file;

  QFileInfo file(p_file);

  //qDebug() << file.fileName() << "//" << file.filePath();

  QString file_basename = file.fileName();
  QString file_parent_dir = get_case_sensitive_path(file.absolutePath());

  //second, does it exist in the new parent dir?
  if (exists(file_parent_dir + "/" + file_basename))
    return file_parent_dir + "/" + file_basename;

  //last resort, dirlist parent dir and find case insensitive match
  QRegExp file_rx = QRegExp(file_basename, Qt::CaseInsensitive);
  QStringList files = QDir(file_parent_dir).entryList();

  int result = files.indexOf(file_rx);

  if (result != -1)
    return file_parent_dir + "/" + files.at(result);

  //if nothing is found, let the caller handle the missing file
  return file_parent_dir + "/" + file_basename;
}
