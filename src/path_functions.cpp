#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"

#include <QDir>
#include <QRegExp>
#include <QStandardPaths>
#include <QStringBuilder>

#ifdef BASE_OVERRIDE
#include "base_override.h"
#endif

// this is a quite broad generalization
// the most common OSes(mac and windows) are _usually_ case insensitive
// however, there do exist mac installations with case sensitive filesystems
// in that case, define CASE_SENSITIVE_FILESYSTEM and compile on a mac
#if (defined(LINUX) || defined(__linux__))
#define CASE_SENSITIVE_FILESYSTEM
#endif

QString AOApplication::get_base_path()
{
  QString base_path = "";
#ifdef ANDROID
  QString sdcard_storage = getenv("SECONDARY_STORAGE");
  if (dir_exists(sdcard_storage + "/AO2/")) {
    base_path = sdcard_storage + "/AO2/";
  }
  else {
    QString external_storage = getenv("EXTERNAL_STORAGE");
    base_path = external_storage + "/AO2/";
  }
#elif defined(__APPLE__)
  base_path = applicationDirPath() + "/../../../base/";
#else
  base_path = applicationDirPath() + "/base/";
#endif

  return base_path;
}

VPath AOApplication::get_theme_path(QString p_file, QString p_theme)
{
  if (p_theme == "")
      p_theme = current_theme;
  return VPath("themes/" + p_theme + "/" + p_file);
}

VPath AOApplication::get_character_path(QString p_char, QString p_file)
{
  return VPath("characters/" + p_char + "/" + p_file);
}

VPath AOApplication::get_misc_path(QString p_misc, QString p_file)
{
  return VPath("misc/" + p_misc + "/" + p_file);
}

VPath AOApplication::get_sounds_path(QString p_file)
{
  return VPath("sounds/general/" + p_file);
}

VPath AOApplication::get_music_path(QString p_song)
{
  if (p_song.startsWith("http")) {
    return VPath(p_song); // url
  }
  return VPath("sounds/music/" + p_song);
}

VPath AOApplication::get_background_path(QString p_file)
{
  if (courtroom_constructed) {
    return VPath("background/" + w_courtroom->get_current_background() + "/" + p_file);
  }
  return get_default_background_path(p_file);
}

VPath AOApplication::get_default_background_path(QString p_file)
{
  return VPath("background/default/" + p_file);
}

VPath AOApplication::get_evidence_path(QString p_file)
{
  return VPath("evidence/" + p_file);
}

QVector<VPath> AOApplication::get_asset_paths(QString p_element, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc, QString p_character, QString p_placeholder)
{
    QVector<VPath> pathlist;
    if (p_character != "")
      pathlist += get_character_path(p_character, p_element); // Character folder
    if (p_misc != "" && p_theme != "" && p_subtheme != "")
      pathlist += get_theme_path("misc/" + p_misc + "/" + p_element, p_theme + "/" + p_subtheme); // Subtheme misc path
    if (p_misc != "" && p_theme != "")
      pathlist += get_theme_path("misc/" + p_misc + "/" + p_element, p_theme); // Theme misc path
    if (p_theme != "" && p_subtheme != "")
      pathlist += get_theme_path(p_element, p_theme + "/" + p_subtheme); // Subtheme path
    if (p_misc != "")
      pathlist += get_misc_path(p_misc, p_element); // Base misc path
    if (p_theme != "")
      pathlist += get_theme_path(p_element, p_theme); // Theme path
    if (p_default_theme != "")
      pathlist += get_theme_path(p_element, p_default_theme); // Default theme path
    pathlist += VPath(p_element); // The path by itself
    if (p_placeholder != "" && p_theme != "")
      pathlist += get_theme_path(p_placeholder, p_theme); // Placeholder path
    if (p_placeholder != "" && p_default_theme != "")
      pathlist += get_theme_path(p_placeholder, p_default_theme); // Default placeholder path
    return pathlist;
}

QString AOApplication::get_asset_path(QVector<VPath> pathlist)
{
  for (const VPath &p : pathlist) {
      QString path = get_real_path(p);
      if (!path.isEmpty()) {
          return path;
      }
  }
  return QString();
}

QString AOApplication::get_image_path(QVector<VPath> pathlist, bool static_image)
{
  for (const VPath &p : pathlist) {
      QString path = get_image_suffix(p, static_image);
      if (!path.isEmpty()) {
          return path;
      }
  }
  return QString();
}

QString AOApplication::get_sfx_path(QVector<VPath> pathlist)
{
  for (const VPath &p : pathlist) {
      QString path = get_sfx_suffix(p);
      if (!path.isEmpty()) {
          return path;
      }
  }
  return QString();
}

QString AOApplication::get_config_value(QString p_identifier, QString p_config, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc)
{
    QString path;
//    qDebug() << "got request for" << p_identifier << "in" << p_config;
    const auto paths = get_asset_paths(p_config, p_theme, p_subtheme, p_default_theme, p_misc);
    for (const VPath &p : paths) {
        path = get_real_path(p);
        if (!path.isEmpty()) {
            QSettings settings(path, QSettings::IniFormat);
            settings.setIniCodec("UTF-8");
            QVariant value = settings.value(p_identifier);
            if (value.type() == QVariant::StringList) {
//              qDebug() << "got" << p << "is a string list, returning" << value.toStringList().join(",");
              return value.toStringList().join(",");
            }
            else if (!value.isNull()){
//              qDebug() << "got" << p << "is a string, returning" << value.toString();
              return value.toString();
            }
        }
    }
    return "";
}

QString AOApplication::get_asset(QString p_element, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc, QString p_character, QString p_placeholder)
{
  return get_asset_path(get_asset_paths(p_element, p_theme, p_subtheme, p_default_theme, p_misc, p_character, p_placeholder));
}

QString AOApplication::get_image(QString p_element, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc, QString p_character, QString p_placeholder,
                                 bool static_image)
{
  return get_image_path(get_asset_paths(p_element, p_theme, p_subtheme, p_default_theme, p_misc, p_character, p_placeholder), static_image);
}

QString AOApplication::get_sfx(QString p_sfx, QString p_misc, QString p_character)
{
  QVector<VPath> pathlist = get_asset_paths(p_sfx, current_theme, get_subtheme(), default_theme, p_misc, p_character);
  pathlist += get_sounds_path(p_sfx); // Sounds folder path
  return get_sfx_path(pathlist);
}

QString AOApplication::get_case_sensitive_path(QString p_file)
{
#ifdef CASE_SENSITIVE_FILESYSTEM
  // first, check to see if it's actually there (also serves as base case for
  // recursion)
  QFileInfo file(p_file);
  QString file_basename = file.fileName();
  if (exists(p_file))
    return p_file;


  QString file_parent_dir = get_case_sensitive_path(file.absolutePath());

  // second, does it exist in the new parent dir?
  if (exists(file_parent_dir + "/" + file_basename))
    return file_parent_dir + "/" + file_basename;

  // last resort, dirlist parent dir and find case insensitive match

  if (!dir_listing_exist_cache.contains(qHash(file_parent_dir))) {
    QStringList files = QDir(file_parent_dir).entryList();
    for (const QString &file : files) {
      dir_listing_cache.insert(qHash(file_parent_dir % QChar('/') % file.toLower()), file);
    }
    dir_listing_exist_cache.insert(qHash(file_parent_dir));
  }
  QString found_file = dir_listing_cache.value(
        qHash(file_parent_dir % QChar('/') % file_basename.toLower()));

  if (!found_file.isEmpty()) {
    return file_parent_dir + "/" + found_file;
  }

  // if nothing is found, let the caller handle the missing file
  return file_parent_dir + "/" + file_basename;
#else
  return p_file;
#endif
}

QString AOApplication::get_real_path(const VPath &vpath) {
  // Try cache first
  QString phys_path = asset_lookup_cache.value(qHash(vpath));
  if (!phys_path.isEmpty() && exists(phys_path)) {
    return phys_path;
  }

  // Cache miss; try all known mount paths
  QStringList bases = get_mount_paths();
  bases.push_front(get_base_path());

  for (const QString &base : bases) {
    QDir baseDir(base);
    QString path = baseDir.absoluteFilePath(vpath.toQString());
    if (!path.startsWith(baseDir.absolutePath())) {
      qWarning() << "invalid path" << path << "(path is outside vfs)";
      break;
    }
    path = get_case_sensitive_path(path);
    if (exists(path)) {
      asset_lookup_cache.insert(qHash(vpath), path);
      return path;
    }
  }

  // File or directory not found
  return QString();
}

// Special case of get_real_path where multiple suffixes need to be tried
// on each mount path.
QString AOApplication::get_real_suffixed_path(const VPath &vpath,
                                              const QStringList &suffixes) {
  // Try cache first
  QString phys_path = asset_lookup_cache.value(qHash(vpath));
  if (!phys_path.isEmpty() && exists(phys_path)) {
    return phys_path;
  }

  // Cache miss; try each suffix on all known mount paths
  QStringList bases = get_mount_paths();
  bases.push_front(get_base_path());

  for (const QString &base : bases) {
    for (const QString &suffix : suffixes) {
      QDir baseDir(base);
      QString path = baseDir.absoluteFilePath(vpath.toQString() + suffix);
      if (!path.startsWith(baseDir.absolutePath())) {
        qWarning() << "invalid path" << path << "(path is outside vfs)";
        break;
      }
      path = get_case_sensitive_path(path);
      if (exists(path)) {
        asset_lookup_cache.insert(qHash(vpath), path);
        return path;
      }
    }
  }

  // File or directory not found
  return QString();
}

void AOApplication::invalidate_lookup_cache() {
  asset_lookup_cache.clear();
  dir_listing_cache.clear();
  dir_listing_exist_cache.clear();
}
