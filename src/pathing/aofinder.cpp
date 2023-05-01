#include "pathing/aofinder.hpp"
#include "pathing/aocache.hpp"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSettings>

AOFinder::AOFinder(QObject *parent, QString f_base_path,
                   QStringList f_mount_paths)
    : QObject(parent), m_cache(std::make_unique<AttorneyOnline::Core::AOCache>(
                           this, f_base_path)),
      m_base_path(f_base_path), m_mount_paths(f_mount_paths)
{
  qInfo() << "[AOFinder]::CTOR: Starting";
}

AOFinder::~AOFinder() {}

// this is a quite broad generalization
// the most common OSes(mac and windows) are _usually_ case insensitive
// however, there do exist mac installations with case sensitive filesystems
// in that case, define CASE_SENSITIVE_FILESYSTEM and compile on a mac
#if (defined(LINUX) || defined(__linux__))
#define CASE_SENSITIVE_FILESYSTEM
#endif

static bool is_power_2(unsigned int n)
{
  unsigned int r = 0;
  while (n) {
    r += n & 1;
    n >>= 1;
  }
  return r == 1;
}

VPath AOFinder::get_theme_path(QString f_file, QString f_theme)
{
  return VPath("themes/" + f_theme + "/" + f_file);
}

VPath AOFinder::get_character_path(QString f_char, QString f_file)
{
  return VPath("characters/" + f_char + "/" + f_file);
}

VPath AOFinder::get_misc_path(QString f_misc, QString f_file)
{
  return VPath("misc/" + f_misc + "/" + f_file);
}

VPath AOFinder::get_sounds_path(QString f_file)
{
  return VPath("sounds/general/" + f_file);
}

VPath AOFinder::get_music_path(QString f_song)
{
  if (f_song.startsWith("http")) {
    return VPath(f_song); // url
  }
  return VPath("sounds/music/" + f_song);
}

VPath AOFinder::get_background_path(QString f_file, QString f_background)
{
  if (!f_background.isEmpty()) {
    return VPath("background/" + f_background + "/" + f_file);
  }
  return get_default_background_path(f_file);
}

VPath AOFinder::get_default_background_path(QString f_file)
{
  return VPath("background/default/" + f_file);
}

QString AOFinder::get_sfx_suffix(VPath sound_to_check)
{
  QStringList suffixes = {".opus", ".ogg", ".mp3", ".wav", ".mid", ".midi",
                          ".xm",   ".it",  ".s3m", ".mod", ".mtm", ".umx"};
  // Check if we were provided a direct filepath with a suffix already
  QString path = sound_to_check.toQString();
  // Loop through our suffixes
  for (const QString &suffix : suffixes) {
    // If our VPath ends with a valid suffix
    if (path.endsWith(suffix, Qt::CaseInsensitive))
      // Return that as the path
      return get_real_path(sound_to_check);
  }
  // Otherwise, ignore the provided suffix and check our own
  return get_real_path(sound_to_check, suffixes);
}

QString AOFinder::get_image_suffix(VPath path_to_check, bool static_image)
{
  QStringList suffixes{};
  if (!static_image) {
    suffixes.append({".webp", ".apng", ".gif"});
  }
  suffixes.append(".png");

  // Check if we were provided a direct filepath with a suffix already
  QString path = path_to_check.toQString();
  // Loop through our suffixes
  for (const QString &suffix : suffixes) {
    // If our VPath ends with a valid suffix
    if (path.endsWith(suffix, Qt::CaseInsensitive))
      // Return that as the path
      return get_real_path(path_to_check);
  }
  // Otherwise, ignore the provided suffix and check our own
  return get_real_path(path_to_check, suffixes);
}

QString AOFinder::get_pos_path(const QString &f_background, const QString &pos,
                               const bool desk)
{
  // witness is default if pos is invalid
  QString l_background;
  QString l_desk_image;

  if (QFile::exists(get_image_suffix(get_background_path("witnessempty", f_background)))) {
    l_background = "witnessempty";
    l_desk_image = "stand";
  }
  else {
    l_background = "wit";
    l_desk_image = "wit_overlay";
  }

  if (pos == "def" && QFile::exists(get_image_suffix(
                          get_background_path("defenseempty", f_background)))) {
    l_background = "defenseempty";
    l_desk_image = "defensedesk";
  }
  else if (pos == "pro" &&
           QFile::exists(get_image_suffix(
               get_background_path("prosecutorempty", f_background)))) {
    l_background = "prosecutorempty";
    l_desk_image = "prosecutiondesk";
  }
  else if (pos == "jud" && QFile::exists(get_image_suffix(
                               get_background_path("judgestand", f_background)))) {
    l_background = "judgestand";
    l_desk_image = "judgedesk";
  }
  else if (pos == "hld" &&
           QFile::exists(get_image_suffix(
               get_background_path("helperstand", f_background)))) {
    l_background = "helperstand";
    l_desk_image = "helperdesk";
  }
  else if (pos == "hlp" &&
           QFile::exists(get_image_suffix(
               get_background_path("prohelperstand", f_background)))) {
    l_background = "prohelperstand";
    l_desk_image = "prohelperdesk";
  }
  else if (pos == "jur" && QFile::exists(get_image_suffix(
                               get_background_path("jurystand", f_background)))) {
    l_background = "jurystand";
    l_desk_image = "jurydesk";
  }
  else if (pos == "sea" &&
           QFile::exists(get_image_suffix(
               get_background_path("seancestand", f_background)))) {
    l_background = "seancestand";
    l_desk_image = "seancedesk";
  }

  if (QFile::exists(get_image_suffix(
          get_background_path(pos, f_background)))) // Unique pos path
  {
    l_background = pos;
    l_desk_image = pos + "_overlay";
  }

  QString desk_override = read_design_ini("overlays/" + f_background, get_background_path("design.ini", f_background));
  if (desk_override != "") {
    l_desk_image = desk_override;
  }
  if (desk) {
    return l_desk_image;
  }
  return l_background;
}

QString AOFinder::read_design_ini(QString p_identifier, VPath p_design_path)
{
  return read_design_ini(p_identifier, get_real_path(p_design_path));
}

QString AOFinder::read_design_ini(QString p_identifier, QString p_design_path)
{
  QSettings settings(p_design_path, QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  QVariant value = settings.value(p_identifier);
  if (value.type() == QVariant::StringList) {
    return value.toStringList().join(",");
  }
  else if (!value.isNull()) {
    return value.toString();
  }
  return "";
}

void AOFinder::invalidate_lookup_cache(QString f_reason)
{
  m_cache.get()->invalidateCache(f_reason);
}

VPath AOFinder::get_evidence_path(QString f_file)
{
  return VPath("evidence/" + f_file);
}

QVector<VPath> AOFinder::get_asset_paths(QString f_element, QString f_theme,
                                         QString f_subtheme,
                                         QString f_default_theme,
                                         QString f_misc, QString f_character,
                                         QString f_placeholder)
{
  QVector<VPath> pathlist;
  if (f_character != "")
    pathlist += get_character_path(f_character, f_element); // Character folder
  if (f_misc != "" && f_theme != "" && f_subtheme != "")
    pathlist +=
        get_theme_path("misc/" + f_misc + "/" + f_element,
                       f_theme + "/" + f_subtheme); // Subtheme misc path
  if (f_misc != "" && f_theme != "")
    pathlist += get_theme_path("misc/" + f_misc + "/" + f_element,
                               f_theme); // Theme misc path
  if (f_theme != "" && f_subtheme != "")
    pathlist +=
        get_theme_path(f_element, f_theme + "/" + f_subtheme); // Subtheme path
  if (f_misc != "")
    pathlist += get_misc_path(f_misc, f_element); // Base misc path
  if (f_theme != "")
    pathlist += get_theme_path(f_element, f_theme); // Theme path
  if (f_default_theme != "")
    pathlist +=
        get_theme_path(f_element, f_default_theme); // Default theme path
  pathlist += VPath(f_element);                     // The path by itself
  if (f_placeholder != "" && f_theme != "")
    pathlist += get_theme_path(f_placeholder, f_theme); // Placeholder path
  if (f_placeholder != "" && f_default_theme != "")
    pathlist += get_theme_path(f_placeholder,
                               f_default_theme); // Default placeholder path
  return pathlist;
}

QString AOFinder::get_asset_path(QVector<VPath> pathlist)
{
  for (const VPath &p : pathlist) {
    QString path = get_real_path(p);
    if (!path.isEmpty()) {
      return path;
    }
  }
  return QString();
}

QString AOFinder::get_image_path(QVector<VPath> pathlist, bool static_image)
{
  for (const VPath &p : pathlist) {
    QString path = get_image_suffix(p, static_image);
    if (!path.isEmpty()) {
      return path;
    }
  }
  return QString();
}

QString AOFinder::get_sfx_path(QVector<VPath> pathlist)
{
  for (const VPath &p : pathlist) {
    QString path = get_sfx_suffix(p);
    if (!path.isEmpty()) {
      return path;
    }
  }
  return QString();
}

QString AOFinder::get_config_value(QString f_identifier, QString f_config,
                                   QString f_theme, QString f_subtheme,
                                   QString f_default_theme, QString f_misc)
{
  QString path;
  //    qDebug() << "got request for" << f_identifier << "in" << f_config;
  const auto paths =
      get_asset_paths(f_config, f_theme, f_subtheme, f_default_theme, f_misc);
  for (const VPath &p : paths) {
    path = get_real_path(p);
    if (!path.isEmpty()) {
      QSettings settings(path, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
      settings.setIniCodec("UTF-8");
#endif
      QVariant value = settings.value(f_identifier);
      if (value.type() == QVariant::StringList) {
        return value.toStringList().join(",");
      }
      else if (!value.isNull()) {
        return value.toString();
      }
    }
  }
  return "";
}

QString AOFinder::get_asset(QString f_element, QString f_theme,
                            QString f_subtheme, QString f_default_theme,
                            QString f_misc, QString f_character,
                            QString f_placeholder)
{
  QString ret = get_asset_path(get_asset_paths(f_element, f_theme, f_subtheme,
                                               f_default_theme, f_misc,
                                               f_character, f_placeholder));
  if (ret.isEmpty()) {
    qWarning().nospace() << "could not find asset " << f_element
                         << " (theme = " << f_theme << ", misc = " << f_misc
                         << ", char = " << f_character << ")";
  }
  return ret;
}

QString AOFinder::get_image(QString f_element, QString f_theme,
                            QString f_subtheme, QString f_default_theme,
                            QString f_misc, QString f_character,
                            QString f_placeholder, bool static_image)
{
  QString ret = get_image_path(get_asset_paths(f_element, f_theme, f_subtheme,
                                               f_default_theme, f_misc,
                                               f_character, f_placeholder),
                               static_image);
  if (ret.isEmpty()) {
    qWarning().nospace() << "could not find image " << f_element
                         << " (theme = " << f_theme << ", misc = " << f_misc
                         << ", char = " << f_character
                         << ", static = " << static_image << ")";
  }
  return ret;
}

QString AOFinder::get_sfx(QString f_sfx, QString f_misc, QString f_character)
{
  QVector<VPath> pathlist;
  // If sound subfolder not found, search just for SFX
  pathlist += get_asset_paths(f_sfx, "", "", "", f_misc, f_character);
  // If SFX not found, search base/sounds/general/ folder
  pathlist += get_sounds_path(f_sfx);
  QString ret = get_sfx_path(pathlist);
  if (ret.isEmpty()) {
    qWarning().nospace() << "could not find sfx " << f_sfx
                         << " (char = " << f_character << ", misc = " << f_misc
                         << ")";
  }
  return ret;
}

QString AOFinder::get_case_sensitive_path(QString f_file)
{
#ifdef CASE_SENSITIVE_FILESYSTEM
  // first, check to see if it's actually there (also serves as base case for
  // recursion)
  QFileInfo file(f_file);
  QString file_basename = file.fileName();
  if (QFile::exists((f_file))
    return f_file;


  QString file_parent_dir = get_case_sensitive_path(file.absolutePath());

         // second, does it exist in the new parent dir?
  if (QFile::exists((file_parent_dir + "/" + file_basename))
    return file_parent_dir + "/" + file_basename;

         // last resort, dirlist parent dir and find case insensitive match

  if (!dir_listing_exist_cache.contains(qHash(file_parent_dir))) {
    QStringList files = QDir(file_parent_dir).entryList();
    for (const QString &file : files) {
      dir_listing_cache.insert(
          qHash(file_parent_dir % QChar('/') % file.toLower()), file);
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
  return f_file;
#endif
}

QString AOFinder::get_real_path(const VPath &vpath, const QStringList &suffixes)
{
  // Try cache first
  QString phys_path = m_cache->checkAssetCache(vpath);
  if (!phys_path.isEmpty() && QFile::exists(phys_path)) {
    for (const QString &suffix :
         suffixes) { // make sure cached asset is the right type
      if (phys_path.endsWith(suffix, Qt::CaseInsensitive)) {
        return phys_path;
      }
    }
  }

  // Cache miss; try all known mount paths
  QStringList bases = m_mount_paths;
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
      if (QFile::exists(path)) {
        m_cache->insertIntoAssetCache(vpath, path);
        unsigned int cache_size = is_power_2(m_cache->assetCacheSize());
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

void AOFinder::updateMountPaths(QStringList f_base_mounts, QString f_reason)
{
  m_cache.get()->invalidateCache(f_reason);
  m_cache.get()->populateDirectoryCache(f_base_mounts);
}
