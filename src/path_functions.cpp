#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"
#include "options.h"

#include <QDir>
#include <QRegularExpression>
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

static bool is_power_2(unsigned int n) {
  unsigned int r = 0;
  while (n) {
    r += n & 1;
    n >>= 1;
  }
  return r == 1;
}

VPath AOApplication::get_theme_path(QString p_file, QString p_theme)
{
  if (p_theme == "")
      p_theme = Options::getInstance().theme();
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

QString AOApplication::get_pos_path(const QString& pos, const bool desk)
{
  // witness is default if pos is invalid
  QString f_background;
  QString f_desk_image;
  if (file_exists(get_image_suffix(get_background_path("witnessempty")))) {
    f_background = "witnessempty";
    f_desk_image = "stand";
  }
  else {
    f_background = "wit";
    f_desk_image = "wit_overlay";
  }

  if (pos == "def" && file_exists(get_image_suffix(
                             get_background_path("defenseempty")))) {
    f_background = "defenseempty";
    f_desk_image = "defensedesk";
  }
  else if (pos == "pro" &&
           file_exists(get_image_suffix(
               get_background_path("prosecutorempty")))) {
    f_background = "prosecutorempty";
    f_desk_image = "prosecutiondesk";
  }
  else if (pos == "jud" && file_exists(get_image_suffix(
                                  get_background_path("judgestand")))) {
    f_background = "judgestand";
    f_desk_image = "judgedesk";
  }
  else if (pos == "hld" &&
           file_exists(get_image_suffix(
               get_background_path("helperstand")))) {
    f_background = "helperstand";
    f_desk_image = "helperdesk";
  }
  else if (pos == "hlp" &&
           file_exists(get_image_suffix(
               get_background_path("prohelperstand")))) {
    f_background = "prohelperstand";
    f_desk_image = "prohelperdesk";
  }
  else if (pos == "jur" && file_exists(get_image_suffix(
                                  get_background_path("jurystand")))) {
    f_background = "jurystand";
    f_desk_image = "jurydesk";
  }
  else if (pos == "sea" &&
           file_exists(get_image_suffix(
               get_background_path("seancestand")))) {
    f_background = "seancestand";
    f_desk_image = "seancedesk";
  }

  if (file_exists(get_image_suffix(
          get_background_path(pos)))) // Unique pos path
  {
    f_background = pos;
    f_desk_image = pos + "_overlay";
  }

  QString desk_override = read_design_ini("overlays/" + f_background, get_background_path("design.ini"));
  if (desk_override != "") {
    f_desk_image = desk_override;
}
  if (desk) {
    return f_desk_image;
  }
  return f_background;
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            settings.setIniCodec("UTF-8");
#endif
            QVariant value = settings.value(p_identifier);
            if (value.type() == QVariant::StringList) {
              return value.toStringList().join(",");
            }
            else if (!value.isNull()){
              return value.toString();
            }
        }
    }
    return "";
}

QString AOApplication::get_asset(QString p_element, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc, QString p_character, QString p_placeholder)
{
  QString ret = get_asset_path(get_asset_paths(p_element, p_theme, p_subtheme, p_default_theme, p_misc, p_character, p_placeholder));
  if (ret.isEmpty()) {
    qWarning().nospace() << "could not find asset " << p_element
                         << " (theme = " << p_theme
                         << ", misc = " << p_misc
                         << ", char = " << p_character << ")";
  }
  return ret;
}

QString AOApplication::get_image(QString p_element, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc, QString p_character, QString p_placeholder,
                                 bool static_image)
{
  QString ret = get_image_path(get_asset_paths(p_element, p_theme, p_subtheme, p_default_theme, p_misc, p_character, p_placeholder), static_image);
  if (ret.isEmpty()) {
    qWarning().nospace() << "could not find image " << p_element
                         << " (theme = " << p_theme
                         << ", misc = " << p_misc
                         << ", char = " << p_character
                         << ", static = " << static_image << ")";
  }
  return ret;
}

QString AOApplication::get_sfx(QString p_sfx, QString p_misc, QString p_character)
{
  QVector<VPath> pathlist;
  // Sounds subfolder is prioritized for organization sake
  pathlist += get_asset_paths("sounds/" + p_sfx, Options::getInstance().theme(), Options::getInstance().subTheme(), default_theme, p_misc, p_character);
  // If sound subfolder not found, search just for SFX
  pathlist += get_asset_paths(p_sfx, Options::getInstance().theme(), Options::getInstance().subTheme(), default_theme, p_misc, p_character);
  // If SFX not found, search base/sounds/general/ folder
  pathlist += get_sounds_path(p_sfx);
  QString ret = get_sfx_path(pathlist);
  if (ret.isEmpty()) {
    qWarning().nospace() << "could not find sfx " << p_sfx
                         << " (char = " << p_character
                         << ", misc = " << p_misc << ")";
  }
  return ret;
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

QString AOApplication::get_real_path(const VPath &vpath,
                                     const QStringList &suffixes) {
  // Try cache first
  QString phys_path = asset_lookup_cache.value(qHash(vpath));
  if (!phys_path.isEmpty() && exists(phys_path)) {
    for (const QString &suffix : suffixes) { // make sure cached asset is the right type
      if (phys_path.endsWith(suffix, Qt::CaseInsensitive))
        return phys_path;
    }
  }

  // Cache miss; try all known mount paths
  QStringList bases = Options::getInstance().mountPaths();
  bases.prepend(get_base_path());
  // base
  // content 1
  // content 2

  // We search last to first
  std::reverse(bases.begin(), bases.end());

  // content 2
  // content 1
  // base
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
        unsigned int cache_size = asset_lookup_cache.size();
        if (is_power_2(cache_size))
          qDebug() << "lookup cache has reached" << cache_size << "entries";
        return path;
      }
    }
  }

  // Not found in mount paths; check if the file is remote
  QString remotePath = vpath.toQString();
  if (remotePath.startsWith("http:") || remotePath.startsWith("https:")) {
      return remotePath;
  }

  // File or directory not found
  return QString();
}

void AOApplication::invalidate_lookup_cache() {
  asset_lookup_cache.clear();
  dir_listing_cache.clear();
  dir_listing_exist_cache.clear();
}
