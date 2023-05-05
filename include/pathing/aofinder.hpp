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

  QString get_base_path() const;

  VPath get_theme_path(QString p_file, QString p_theme="") const;
  VPath get_character_path(QString p_char, QString p_file) const;
  VPath get_misc_path(QString p_misc, QString p_file) const;
  VPath get_sounds_path(QString p_file) const;
  VPath get_music_path(QString p_song) const;
  VPath get_background_path(QString p_file, QString f_background) const;
  VPath get_default_background_path(QString p_file) const;
  VPath get_evidence_path(QString p_file) const;
  QVector<VPath> get_asset_paths(QString p_element, QString p_theme="", QString p_subtheme="", QString p_default_theme="", QString p_misc="", QString p_character="", QString p_placeholder="") const;
  QString get_asset_path(QVector<VPath> pathlist) const;
  QString get_image_path(QVector<VPath> pathlist, bool static_image=false) const;
  QString get_sfx_path(QVector<VPath> pathlist) const;
  QString get_config_value(QString p_identifier, QString p_config, QString p_theme="", QString p_subtheme="", QString p_default_theme="", QString p_misc="") const;
  QString get_asset(QString p_element, QString p_theme="", QString p_subtheme="", QString p_default_theme="", QString p_misc="", QString p_character="", QString p_placeholder="") const;
  QString get_image(QString p_element, QString p_theme="", QString p_subtheme="", QString p_default_theme="", QString p_misc="", QString p_character="", QString p_placeholder="", bool static_image=false) const;
  QString get_sfx(QString p_sfx, QString p_misc="", QString p_character="") const;
  QString get_pos_path(const QString& f_background, const QString& pos, bool desk = false) const;
  QString get_case_sensitive_path(QString p_file) const;
  QString get_real_path(const VPath &vpath, const QStringList &suffixes={""}) const;

  // Figure out if we can opus this or if we should fall back to wav
  QString get_sfx_suffix(VPath sound_to_check) const;

         // Can we use APNG for this? If not, WEBP? If not, GIF? If not, fall back to
         // PNG.
  QString get_image_suffix(VPath path_to_check, bool static_image=false) const;
  // Returns the value of p_identifier in the design.ini file in p_design_path
  QString read_design_ini(QString p_identifier, VPath p_design_path) const;
  QString read_design_ini(QString p_identifier, QString p_design_path) const;

  void invalidate_lookup_cache(QString f_reason);

private:
  const std::unique_ptr<AttorneyOnline::Core::AOCache> m_cache;
  const QString m_base_path;
  const QStringList m_mount_paths;

public slots:
  void updateMountPaths(QStringList f_base_mounts, QString f_reason);
};

#endif // AOFINDER_H
