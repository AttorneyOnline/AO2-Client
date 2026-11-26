#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"
#include "mount_manager.h"
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

namespace {
// Separates the VPath from its requested suffix list (and each suffix from the
// next) inside the lookup cache key. Chosen because it cannot appear in a
// normal path.
const QChar kPathCacheSeparator(0x1f);

QString make_lookup_cache_key(const VPath &vpath, const QStringList &suffixes)
{
  return vpath.toQString() + kPathCacheSeparator +
         suffixes.join(kPathCacheSeparator);
}
} // namespace

VPath AOApplication::get_theme_path(QString p_file, QString p_theme)
{
  if (p_theme == "")
      p_theme = Options::getInstance().theme();
  return VPath("themes/" + p_theme + "/" + p_file);
}

VPath AOApplication::get_character_path(QString p_char, QString p_file)
{
  // if (!asset_url.isEmpty()) {
  //  VPath path = VPath(asset_url + "characters/" + p_char + "/" + p_file);
  //  qDebug() << path.toQString();
  // }
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
    if (p_file.startsWith("overlays/")){
      return VPath(p_file);
    }else{
      return VPath("background/" + w_courtroom->get_current_background() + "/" + p_file);
    }
  }
  return get_default_background_path(p_file);
}

VPath AOApplication::get_default_background_path(QString p_file)
{
  return VPath("background/default/" + p_file);
}

QString AOApplication::get_pos_path(const QString& pos, const bool desk)
{
  // Witness is default BG
  QString f_background = "wit";
  QString f_desk_image = "wit_overlay";

  if (desk) {
    f_desk_image = get_old_pos_path(pos, desk);
  }
  else {
    f_background = get_old_pos_path(pos, desk);
  }

  // Special desk handling
  QString desk_override = read_design_ini("overlays/" + f_background, get_background_path("design.ini"));
  if (desk_override != "") {
    f_desk_image = get_image_suffix(get_background_path(desk_override));
  }

  // DRO-style override handling
  QString suffix = "back";
  if (desk) {
    suffix = "front";
  }

  // Read the positions.ini file
  QString override = read_design_ini(pos + "/" + suffix, get_background_path("positions.ini"));
  if (override != "") {
    // Set both to the override (we only send back 1 of the 2 anyway)
    f_desk_image = get_image_suffix(get_background_path(override));
    f_background = get_image_suffix(get_background_path(override));
  }

  // Server dictates to override our overlay
  if (w_courtroom->server_overlay != "") {
    // Overlay is located inside of our BG folder
    QString path_test = get_image_suffix(get_background_path(w_courtroom->server_overlay));
    if (file_exists(path_test)) {
      f_desk_image = path_test;
    }

    // Specific overlay *folder* to access inside of the overlay folder
    path_test = get_image_suffix(VPath("overlays/" + w_courtroom->server_overlay + "/" + pos));
    if (file_exists(path_test)) {
      f_desk_image = path_test;
    }

    // Specific file path to access inside of the overlay folder
    path_test = get_image_suffix(VPath("overlays/" + w_courtroom->server_overlay));
    if (file_exists(path_test)) {
      f_desk_image = path_test;
    }
  }
  if (desk) {
    return f_desk_image;
  }
  return f_background;
}

QString AOApplication::get_old_pos_path(const QString& pos, const bool desk)
{
  QString f_background = "wit";
  QString f_desk_image = "wit_overlay";
  // if we have pre-2.8-style positions, e.g. witnessempty.png, use that as the default.
  if (file_exists(get_image_suffix(get_background_path("witnessempty")))) {
    f_background = "witnessempty";
    f_desk_image = "stand";
  }

  // Pre-2.8-style positions are detected
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

  if (desk) {
    return get_image_suffix(get_background_path(f_desk_image));
  }
  return get_image_suffix(get_background_path(f_background));
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
            QVariant value;
            if (!ini_cache.contains(path)) {
              QSettings settings = QSettings(path, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
              settings.setIniCodec("UTF-8");
#endif
              QMap<QString, QVariant> map;
              foreach (QString key, settings.allKeys()) {
                   map[key] = settings.value(key);
              }
              ini_cache[path] = map;
            }
            value = ini_cache[path][p_identifier];
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
  // make sure to replace windows \ with /
  p_file.replace("\\", "/");
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

  if (!dir_listing_exist_cache.contains(file_parent_dir)) {
    const QStringList files = QDir(file_parent_dir).entryList();
    for (const QString &file : files) {
      dir_listing_cache.insert(file_parent_dir % QChar('/') % file.toLower(), file);
    }
    dir_listing_exist_cache.insert(file_parent_dir);
  }
  const QString found_file = dir_listing_cache.value(
        file_parent_dir % QChar('/') % file_basename.toLower());

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
  const QString cache_key = make_lookup_cache_key(vpath, suffixes);

  // Positive and negative results are both cached: once an asset has been
  // looked up, resolving it again is pure memory work.
  const auto cached = asset_lookup_cache.constFind(cache_key);
  if (cached != asset_lookup_cache.constEnd())
    return cached.value();

  if (asset_missing_cache.contains(cache_key))
    return QString();

  // Cache miss; try all known mount paths
  QStringList bases = m_mounts->resolvedMountPaths();
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
    QDir baseDir(base);
    const QString basePath = baseDir.absolutePath();
    for (const QString &suffix : suffixes) {
      QString path = baseDir.absoluteFilePath(vpath.toQString() + suffix);
      if (!path.startsWith(basePath)) {
        qWarning() << "invalid path" << path << "(path is outside vfs)";
        break;
      }
      path = get_case_sensitive_path(path);
      if (exists(path)) {
        asset_lookup_cache.insert(cache_key, path);
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
    asset_lookup_cache.insert(cache_key, remotePath);
    return remotePath;
  // } else if (!asset_url.isEmpty()) { // Reminder to change this later
  //    remotePath = asset_url + remotePath;
  //    qDebug() << "Remote path: " << remotePath;
  //    return QString();
  }

  // File or directory not found. Remember that, so that repeated page visits
  // do not rescan every mount for the same missing asset.
  asset_missing_cache.insert(cache_key);
  return QString();
}

void AOApplication::invalidate_lookup_cache() {
  asset_lookup_cache.clear();
  asset_missing_cache.clear();
  dir_listing_cache.clear();
  dir_listing_exist_cache.clear();
  ini_cache.clear();
}

void AOApplication::invalidate_cached_path(const VPath &vpath) {
  const QString path = vpath.toQString();
  // Drop everything cached for the containing folder, not just the exact
  // VPath: a downloaded file can change the result for its siblings and for
  // the folder itself. For a plain top-level name, only drop that VPath.
  const int slash = path.lastIndexOf(QLatin1Char('/'));
  const QString prefix = slash >= 0 ? path.left(slash + 1)
                                    : path + kPathCacheSeparator;

  for (auto it = asset_lookup_cache.begin(); it != asset_lookup_cache.end();) {
    if (it.key().startsWith(prefix))
      it = asset_lookup_cache.erase(it);
    else
      ++it;
  }

  for (auto it = asset_missing_cache.begin(); it != asset_missing_cache.end();) {
    if (it->startsWith(prefix))
      it = asset_missing_cache.erase(it);
    else
      ++it;
  }

  // A file may have just appeared in an existing directory, so the cached
  // case-sensitive directory listings can no longer be trusted.
  dir_listing_cache.clear();
  dir_listing_exist_cache.clear();
}
