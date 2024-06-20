#pragma once

#include "aopacket.h"
#include "datatypes.h"
#include "demoserver.h"
#include "discord_rich_presence.h"
#include "serverdata.h"
#include "widgets/aooptionsdialog.h"

#include <bass.h>

#include <QApplication>
#include <QColor>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QRect>
#include <QScreen>
#include <QSettings>
#include <QStandardPaths>
#include <QStringList>
#include <QTextStream>
#include <QTime>
#include <QVector>

class NetworkManager;
class Lobby;
class Courtroom;
class Options;

class VPath : QString
{
  using QString::QString;

public:
  explicit VPath(const QString &str)
      : QString(str)
  {}
  inline const QString &toQString() const { return *this; }
  inline bool operator==(const VPath &str) const { return this->toQString() == str.toQString(); }
  inline VPath operator+(const VPath &str) const { return VPath(this->toQString() + str.toQString()); }
};

inline uint qHash(const VPath &key, uint seed = qGlobalQHashSeed())
{
  return qHash(key.toQString(), seed);
}

class AOApplication : public QApplication
{
  Q_OBJECT

public:
  AOApplication(int &argc, char **argv);
  ~AOApplication();

  NetworkManager *net_manager;
  Lobby *w_lobby = nullptr;
  Courtroom *w_courtroom = nullptr;
  AttorneyOnline::Discord *discord;

  QFont default_font;

  bool is_lobby_constructed();
  void construct_lobby();
  void destruct_lobby();

  bool is_courtroom_constructed();
  void construct_courtroom();
  void destruct_courtroom();

  void server_packet_received(AOPacket p_packet);

  void send_server_packet(AOPacket p_packet);

  void call_settings_menu();

  qint64 latency = 0;
  QString window_title;

  /// Stores everything related to the server the client is connected to, if
  /// any.
  server::ServerData m_serverdata;

  ///////////////loading info///////////////////

  // client ID. Not useful, to be removed eventually
  int client_id = 0;

  /// Used for a fancy loading bar upon joining a server.
  int generated_chars = 0;

  bool courtroom_loaded = false;

  /**
   * @brief Returns the version string of the software.
   *
   * @return The string "X.Y.Z", where X is the release of the software (usually
   * '2'), Y is the major version, and Z is the minor version.
   */
  static QString get_version_string();

  static const int RELEASE = 2;
  static const int MAJOR_VERSION = 10;
  static const int MINOR_VERSION = 1;

  void set_server_list(QVector<ServerInfo> &servers) { server_list = servers; }
  QVector<ServerInfo> &get_server_list() { return server_list; }

  // implementation in path_functions.cpp
  VPath get_theme_path(QString p_file, QString p_theme = QString());
  VPath get_character_path(QString p_char, QString p_file);
  VPath get_misc_path(QString p_misc, QString p_file);
  VPath get_sounds_path(QString p_file);
  VPath get_music_path(QString p_song);
  VPath get_background_path(QString p_file);
  VPath get_default_background_path(QString p_file);
  VPath get_evidence_path(QString p_file);
  QVector<VPath> get_asset_paths(QString p_element, QString p_theme = QString(), QString p_subtheme = QString(), QString p_default_theme = QString(), QString p_misc = QString(), QString p_character = QString(), QString p_placeholder = QString());
  QString get_asset_path(QVector<VPath> pathlist);
  QString get_image_path(QVector<VPath> pathlist, int &index, bool static_image = false);
  QString get_image_path(QVector<VPath> pathlist, bool static_image = false);
  QString get_sfx_path(QVector<VPath> pathlist);
  QString get_config_value(QString p_identifier, QString p_config, QString p_theme = QString(), QString p_subtheme = QString(), QString p_default_theme = QString(), QString p_misc = QString());
  QString get_asset(QString p_element, QString p_theme = QString(), QString p_subtheme = QString(), QString p_default_theme = QString(), QString p_misc = QString(), QString p_character = QString(), QString p_placeholder = QString());
  QString get_image(QString p_element, QString p_theme = QString(), QString p_subtheme = QString(), QString p_default_theme = QString(), QString p_misc = QString(), QString p_character = QString(), QString p_placeholder = QString(), bool static_image = false);
  QString get_sfx(QString p_sfx, QString p_misc = QString(), QString p_character = QString());

  BackgroundPosition get_pos_path(const QString &pos);

  QString get_case_sensitive_path(QString p_file);
  QString get_real_path(const VPath &vpath, const QStringList &suffixes = {""});

  QString find_image(QStringList p_list);

  ////// Functions for reading and writing files //////
  // Implementations file_functions.cpp

  // returns all of the file's lines in a QStringList
  QStringList get_list_file(VPath path);
  QStringList get_list_file(QString p_file);

  // Process a file and return its text as a QString
  QString read_file(QString filename);

  // Write text to file. make_dir would auto-create the directory if it doesn't
  // exist.
  bool write_to_file(QString p_text, QString p_file, bool make_dir = false);

  // Append text to the end of the file. make_dir would auto-create the
  // directory if it doesn't exist.
  bool append_to_file(QString p_text, QString p_file, bool make_dir = false);

  // Append to the currently open demo file if there is one
  void append_to_demofile(QString packet_string);

  /**
   * @brief Reads the clients log folder and locates potential demo files to populate the demoserver list.
   *
   * @return A seperated list of servernames and demo logfile filenames.
   *
   * @details This is to remove the need of delimiters or deal with potential
   * harmfully encoding or plattform differences. We always get a combo of servername and filename.
   *
   * Do note this function assumes all demo files have the .demo extension.
   *
   */
  QMultiMap<QString, QString> load_demo_logs_list() const;

  // Returns the value of p_identifier in the design.ini file in p_design_path
  QString read_design_ini(QString p_identifier, VPath p_design_path);
  QString read_design_ini(QString p_identifier, QString p_design_path);

  // Returns the coordinates of widget with p_identifier from p_file
  QPoint get_button_spacing(QString p_identifier, QString p_file);

  // Returns the dimensions of widget with specified identifier from p_file
  pos_size_type get_element_dimensions(QString p_identifier, QString p_file, QString p_misc = QString());

  // Returns the value to you
  QString get_design_element(QString p_identifier, QString p_file, QString p_misc = QString());

  // Returns the color with p_identifier from p_file
  QColor get_color(QString p_identifier, QString p_file);

  // Returns the markup symbol used for specified p_identifier such as colors
  QString get_chat_markup(QString p_identifier, QString p_file);

  // Returns the color from the misc folder.
  QColor get_chat_color(QString p_identifier, QString p_chat);

  // Returns the value with p_identifier from penalty/penalty.ini in the current
  // theme path
  QString get_penalty_value(QString p_identifier);

  // Returns the sfx with p_identifier from courtroom_sounds.ini in the current theme path
  QString get_court_sfx(QString p_identifier, QString p_misc = QString());

  // Figure out if we can opus this or if we should fall back to wav
  QString get_sfx_suffix(VPath sound_to_check);

  // Can we use APNG for this? If not, WEBP? If not, GIF? If not, fall back to
  // PNG.
  QString get_image_suffix(VPath path_to_check, bool static_image = false);

  // Returns the value of p_search_line within target_tag and terminator_tag
  QString read_char_ini(QString p_char, QString p_search_line, QString target_tag);

  // Returns a QStringList of all key=value definitions on a given tag.
  QStringList read_ini_tags(VPath p_file, QString target_tag = QString());

  // Returns the text between target_tag and terminator_tag in p_file
  QString get_stylesheet(QString p_file);

  // Returns the side of the p_char character from that characters ini file
  QString get_char_side(QString p_char);

  // Returns the showname from the ini of p_char
  QString get_showname(QString p_char, int p_emote = -1);

  // Returns the category of this character
  QString get_category(QString p_char);

  // Returns the value of chat image from the specific p_char's ini file
  QString get_chat(QString p_char);

  // Returns the value of chat font from the specific p_char's ini file
  QString get_chat_font(QString p_char);

  // Returns the value of chat font size from the specific p_char's ini file
  int get_chat_size(QString p_char);

  // Returns the preanim duration of p_char's p_emote
  int get_preanim_duration(QString p_char, QString p_emote);

  // Not in use
  int get_text_delay(QString p_char, QString p_emote);

  // Get the theme's effects folder, read it and return the list of filenames in
  // a string
  QStringList get_effects(QString p_char);

  // Get the correct effect image
  QString get_effect(QString effect, QString p_char, QString p_folder);

  // Return p_property of fx_name. If p_property is "sound", return
  // the value associated with fx_name, otherwise use fx_name + '_' + p_property.
  QString get_effect_property(QString fx_name, QString p_char, QString p_folder, QString p_property);

  // Returns the custom realisation used by the character.
  QString get_custom_realization(QString p_char);

  // Returns whether the given pos is a judge position
  bool get_pos_is_judge(const QString &p_pos);

  /**
   * @brief Returns the duration of the transition animation between the two
   * given positions, if it exists
   */
  int get_pos_transition_duration(const QString &old_pos, const QString &new_pos);

  // Returns the total amount of emotes of p_char
  int get_emote_number(QString p_char);

  // Returns the emote comment of p_char's p_emote
  QString get_emote_comment(QString p_char, int p_emote);

  // Returns the base name of p_char's p_emote
  QString get_emote(QString p_char, int p_emote);

  // Returns the preanimation name of p_char's p_emote
  QString get_pre_emote(QString p_char, int p_emote);

  // Returns the sfx of p_char's p_emote
  QString get_sfx_name(QString p_char, int p_emote);

  // Returns if the sfx is defined as looping in char.ini
  QString get_sfx_looping(QString p_char, int p_emote);

  // Returns if an emote has a frame specific SFX for it
  QString get_sfx_frame(QString p_char, QString p_emote, int n_frame);

  // Returns if an emote has a frame specific SFX for it
  QString get_flash_frame(QString p_char, QString p_emote, int n_frame);

  // Returns if an emote has a frame specific SFX for it
  QString get_screenshake_frame(QString p_char, QString p_emote, int n_frame);

  // Not in use
  int get_sfx_delay(QString p_char, int p_emote);

  // Returns the modifier for p_char's p_emote
  int get_emote_mod(QString p_char, int p_emote);

  // Returns the desk modifier for p_char's p_emote
  int get_desk_mod(QString p_char, int p_emote);

  // Returns p_char's blipname by reading char.ini for blips (previously called "gender")
  QString get_blipname(QString p_char, int p_emote = -1);

  // Returns p_blipname's sound(path) to play in the client
  QString get_blips(QString p_blipname);

  // Get a property of a given emote, or get it from "options" if emote doesn't have it
  QString get_emote_property(QString p_char, QString p_emote, QString p_property);

  // Return a transformation mode from a string ("smooth" for smooth, anything else for fast)
  Qt::TransformationMode get_scaling(QString p_scaling);

  // Returns the scaling type for p_miscname
  Qt::TransformationMode get_misc_scaling(QString p_miscname);

  // ======
  // These are all casing-related settings.
  // ======

  // Currently defined subtheme
  QString subtheme;

  // Default is always default.
  const QString default_theme = "default";

  // The file name of the log file in base/logs.
  QString log_filename;

  void initBASS();
  static void load_bass_plugins();
  static void CALLBACK BASSreset(HSTREAM handle, DWORD channel, DWORD data, void *user);
  static void doBASSreset();

  QElapsedTimer demo_timer;
  DemoServer *demo_server = nullptr;

private:
  QVector<ServerInfo> server_list;
  QHash<uint, QString> asset_lookup_cache;
  QHash<uint, QString> dir_listing_cache;
  QSet<uint> dir_listing_exist_cache;

public Q_SLOTS:
  void server_disconnected();
  void loading_cancelled();

Q_SIGNALS:
  void qt_log_message(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};
