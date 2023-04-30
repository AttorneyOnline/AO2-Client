#ifndef AOFINDER_H
#define AOFINDER_H

#include <QObject>
#include <memory>

#include "pathing/aovpath.hpp"

namespace AttorneyOnline::Core {
  class AOCache;
}

class AOFinder : public QObject {
public:
  AOFinder(QObject *parent, QString f_base_path, QStringList f_mount_paths);
  ~AOFinder();

  // implementation in path_functions.cpp
  VPath get_theme_path(QString p_file, QString p_theme="");
  VPath get_character_path(QString p_char, QString p_file);
  VPath get_misc_path(QString p_misc, QString p_file);
  VPath get_sounds_path(QString p_file);
  VPath get_music_path(QString p_song);
  VPath get_background_path(QString p_file, QString f_background);
  VPath get_default_background_path(QString p_file);
  VPath get_evidence_path(QString p_file);
  QVector<VPath> get_asset_paths(QString p_element, QString p_theme="", QString p_subtheme="", QString p_default_theme="", QString p_misc="", QString p_character="", QString p_placeholder="");
  QString get_asset_path(QVector<VPath> pathlist);
  QString get_image_path(QVector<VPath> pathlist, bool static_image=false);
  QString get_sfx_path(QVector<VPath> pathlist);
  QString get_config_value(QString p_identifier, QString p_config, QString p_theme="", QString p_subtheme="", QString p_default_theme="", QString p_misc="");
  QString get_asset(QString p_element, QString p_theme="", QString p_subtheme="", QString p_default_theme="", QString p_misc="", QString p_character="", QString p_placeholder="");
  QString get_image(QString p_element, QString p_theme="", QString p_subtheme="", QString p_default_theme="", QString p_misc="", QString p_character="", QString p_placeholder="", bool static_image=false);
  QString get_sfx(QString p_sfx, QString p_misc="", QString p_character="");
  QString get_pos_path(const QString& f_background, const QString& pos, bool desk = false);
  QString get_case_sensitive_path(QString p_file);
  QString get_real_path(const VPath &vpath, const QStringList &suffixes={""});

  // Figure out if we can opus this or if we should fall back to wav
  QString get_sfx_suffix(VPath sound_to_check);

         // Can we use APNG for this? If not, WEBP? If not, GIF? If not, fall back to
         // PNG.
  QString get_image_suffix(VPath path_to_check, bool static_image=false);
  // Returns the value of p_identifier in the design.ini file in p_design_path
  QString read_design_ini(QString p_identifier, VPath p_design_path);
  QString read_design_ini(QString p_identifier, QString p_design_path);

  void invalidate_lookup_cache(QString f_reason);

private:
  std::unique_ptr<AttorneyOnline::Core::AOCache> m_cache;
  QString m_base_path;
  QStringList m_mount_paths;

public slots:
  void updateMountPaths(QStringList f_base_mounts, QString f_reason);
};

#endif // AOFINDER_H
