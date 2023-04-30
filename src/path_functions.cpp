#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"
#include "options.h"
#include "pathing/aofinder.hpp"

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
  return m_finder.get()->get_theme_path(p_file, p_theme);
}

VPath AOApplication::get_character_path(QString p_char, QString p_file)
{
  return m_finder.get()->get_character_path(p_char, p_file);
}

VPath AOApplication::get_misc_path(QString p_misc, QString p_file)
{
  return m_finder.get()->get_misc_path(p_misc, p_file);
}

VPath AOApplication::get_sounds_path(QString p_file)
{
  return m_finder.get()->get_sounds_path(p_file);
}

VPath AOApplication::get_music_path(QString p_song)
{
  return m_finder.get()->get_music_path(p_song);
}

VPath AOApplication::get_background_path(QString p_file)
{
  return m_finder.get()->get_background_path(p_file, w_courtroom->get_current_background());
}

VPath AOApplication::get_default_background_path(QString p_file)
{
  return m_finder.get()->get_default_background_path(p_file);
}

QString AOApplication::get_pos_path(const QString& pos, const bool desk)
{
  return m_finder.get()->get_pos_path(w_courtroom->get_current_background(), pos, desk);
}

VPath AOApplication::get_evidence_path(QString p_file)
{
  return m_finder.get()->get_evidence_path(p_file);
}

QVector<VPath> AOApplication::get_asset_paths(QString p_element, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc, QString p_character, QString p_placeholder)
{
  return m_finder.get()->get_asset_paths(p_element, p_theme, p_subtheme, p_default_theme, p_misc, p_character, p_placeholder);
}

QString AOApplication::get_asset_path(QVector<VPath> pathlist)
{
  return m_finder.get()->get_asset_path(pathlist);
}

QString AOApplication::get_image_path(QVector<VPath> pathlist, bool static_image)
{
  return m_finder.get()->get_image_path(pathlist, static_image);
}

QString AOApplication::get_sfx_path(QVector<VPath> pathlist)
{
  return m_finder.get()->get_sfx_path(pathlist);
}

QString AOApplication::get_config_value(QString p_identifier, QString p_config, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc)
{
  return m_finder.get()->get_config_value(p_identifier, p_config, p_theme, p_subtheme, p_default_theme, p_misc);
}

QString AOApplication::get_asset(QString p_element, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc, QString p_character, QString p_placeholder)
{
  return m_finder.get()->get_asset(p_element, p_theme, p_subtheme, p_default_theme, p_misc, p_character, p_placeholder);
}

QString AOApplication::get_image(QString p_element, QString p_theme, QString p_subtheme, QString p_default_theme, QString p_misc, QString p_character, QString p_placeholder,
                                 bool static_image)
{
  return m_finder.get()->get_image(p_element, p_theme, p_subtheme, p_default_theme, p_misc, p_character, p_placeholder, static_image);
}

QString AOApplication::get_sfx(QString p_sfx, QString p_misc, QString p_character)
{
  return m_finder.get()->get_sfx(p_sfx, p_misc, p_character);
}

QString AOApplication::get_case_sensitive_path(QString p_file)
{
  return m_finder.get()->get_case_sensitive_path(p_file);
}

QString AOApplication::get_real_path(const VPath &vpath,
                                     const QStringList &suffixes)
{
  return m_finder.get()->get_real_path(vpath, suffixes);
}

void AOApplication::invalidate_lookup_cache() {
  return m_finder.get()->invalidate_lookup_cache("Testing");
}
