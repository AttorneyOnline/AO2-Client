#ifndef AOAPPLICATION_H
#define AOAPPLICATION_H

#include "aopacket.h"
#include "datatypes.h"
#include "discord_rich_presence.h"

#include "bass.h"
#include "bassopus.h"

#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QVector>

#include <QDebug>
#include <QDesktopWidget>
#include <QRect>

#include <QCryptographicHash>

#include <QDir>
#include <QStandardPaths>

#include <QColor>
#include <QScreen>
#include <QStringList>
#include <QTextStream>

class NetworkManager;
class Lobby;
class Courtroom;

class AOApplication : public QApplication {
  Q_OBJECT

public:
  AOApplication(int &argc, char **argv);
  ~AOApplication();

  NetworkManager *net_manager;
  Lobby *w_lobby;
  Courtroom *w_courtroom;
  AttorneyOnline::Discord *discord;

  bool lobby_constructed = false;
  bool courtroom_constructed = false;

  void construct_lobby();
  void destruct_lobby();

  void construct_courtroom();
  void destruct_courtroom();

  void ms_packet_received(AOPacket *p_packet);
  void server_packet_received(AOPacket *p_packet);

  void send_ms_packet(AOPacket *p_packet);
  void send_server_packet(AOPacket *p_packet, bool encoded = true);

  void call_settings_menu();
  void call_announce_menu(Courtroom *court);

  /////////////////server metadata//////////////////

  bool yellow_text_enabled = false;
  bool prezoom_enabled = false;
  bool flipping_enabled = false;
  bool custom_objection_enabled = false;
  bool desk_mod_enabled = false;
  bool evidence_enabled = false;
  bool cccc_ic_support_enabled = false;
  bool arup_enabled = false;
  bool casing_alerts_enabled = false;
  bool modcall_reason_enabled = false;
  bool looping_sfx_support_enabled = false;
  bool additive_enabled = false;
  bool effects_enabled = false;

  ///////////////loading info///////////////////

  // player number, it's hardly used but might be needed for some old servers
  int s_pv = 0;

  QString server_software = "";

  int char_list_size = 0;
  int loaded_chars = 0;
  int generated_chars = 0;
  int evidence_list_size = 0;
  int loaded_evidence = 0;
  int music_list_size = 0;
  int loaded_music = 0;

  bool courtroom_loaded = false;

  //////////////////versioning///////////////

  int get_release() const { return RELEASE; }
  int get_major_version() const { return MAJOR_VERSION; }
  int get_minor_version() const { return MINOR_VERSION; }
  QString get_version_string();

  ///////////////////////////////////////////

  void set_favorite_list();
  QVector<server_type> &get_favorite_list() { return favorite_list; }
  void add_favorite_server(int p_server);

  void set_server_list();
  QVector<server_type> &get_server_list() { return server_list; }

  // reads the theme from config.ini and sets it accordingly
  void reload_theme();

  // Returns the character the player has currently selected
  QString get_current_char();

  // implementation in path_functions.cpp
  QString get_base_path();
  QString get_data_path();
  QString get_theme_path(QString p_file);
  QString get_default_theme_path(QString p_file);
  QString get_custom_theme_path(QString p_theme, QString p_file);
  QString get_character_path(QString p_char, QString p_file);
  QString get_misc_path(QString p_misc, QString p_file);
  QString get_sounds_path(QString p_file);
  QString get_music_path(QString p_song);
  QString get_background_path(QString p_file);
  QString get_default_background_path(QString p_file);
  QString get_evidence_path(QString p_file);
  QString get_case_sensitive_path(QString p_file);

  ////// Functions for reading and writing files //////
  // Implementations file_functions.cpp

  // Instead of reinventing the wheel, we'll use a QSettings class.
  QSettings *configini;

  // Reads the theme from config.ini and loads it into the current_theme
  // variable
  QString read_theme();

  // Returns the value of ooc_name in config.ini
  QString get_ooc_name();

  // Returns the blip rate from config.ini (once per X symbols)
  int read_blip_rate();

  // Returns true if blank blips is enabled in config.ini and false otherwise
  bool get_blank_blip();

  // Returns true if looping sound effects are enabled in the config.ini
  bool get_looping_sfx();

  // Returns true if stop music on objection is enabled in the config.ini
  bool objection_stop_music();

  // Returns the value of default_music in config.ini
  int get_default_music();

  // Returns the value of default_sfx in config.ini
  int get_default_sfx();

  // Returns the value of default_blip in config.ini
  int get_default_blip();

  // Returns the value of whether Discord should be enabled on startup
  // from the config.ini.
  bool is_discord_enabled();

  // Returns the value of whether shaking should be enabled.
  // from the config.ini.
  bool is_shake_enabled();

  // Returns the value of whether effects should be enabled.
  // from the config.ini.
  bool is_effects_enabled();

  // Returns the value of whether frame-specific effects defined in char.ini
  // should be sent/received over the network. from the config.ini.
  bool is_frame_network_enabled();

  // Returns the value of whether colored ic log should be a thing.
  // from the config.ini.
  bool is_colorlog_enabled();

  // Returns the value of whether sticky sounds should be a thing.
  // from the config.ini.
  bool is_stickysounds_enabled();

  // Returns the value of whether sticky effects should be a thing.
  // from the config.ini.
  bool is_stickyeffects_enabled();

  // Returns the value of whether sticky preanims should be a thing.
  // from the config.ini.
  bool is_stickypres_enabled();

  // Returns the value of whether custom chatboxes should be a thing.
  // from the config.ini.
  // I am increasingly maddened by the lack of dynamic auto-generation system
  // for settings.
  bool is_customchat_enabled();

  // Returns the value of the maximum amount of lines the IC chatlog
  // may contain, from config.ini.
  int get_max_log_size();

  // Returns whether the log should go upwards (new behaviour)
  // or downwards (vanilla behaviour).
  bool get_log_goes_downwards();

  // Returns whether the log should separate name from text via newline or :
  bool get_log_newline();

  // Get spacing between IC log entries.
  int get_log_margin();

  // Returns whether the log should have a timestamp.
  bool get_log_timestamp();

  // Returns the username the user may have set in config.ini.
  QString get_default_username();

  // Returns the audio device used for the client.
  QString get_audio_output_device();

  // Returns whether the user would like to have custom shownames on by default.
  bool get_showname_enabled_by_default();

  // Returns the list of words in callwords.ini
  QStringList get_call_words();

  // returns all of the file's lines in a QStringList
  QStringList get_list_file(QString p_file);

  // Process a file and return its text as a QString
  QString read_file(QString filename);

  // Write text to file. make_dir would auto-create the directory if it doesn't
  // exist.
  bool write_to_file(QString p_text, QString p_file, bool make_dir = false);

  // Append text to the end of the file. make_dir would auto-create the
  // directory if it doesn't exist.
  bool append_to_file(QString p_text, QString p_file, bool make_dir = false);

  // Appends the argument string to serverlist.txt
  void write_to_serverlist_txt(QString p_line);

  // Returns the contents of serverlist.txt
  QVector<server_type> read_serverlist_txt();

  // Returns the value of p_identifier in the design.ini file in p_design_path
  QString read_design_ini(QString p_identifier, QString p_design_path);

  // Returns the coordinates of widget with p_identifier from p_file
  QPoint get_button_spacing(QString p_identifier, QString p_file);

  // Returns the dimensions of widget with specified identifier from p_file
  pos_size_type get_element_dimensions(QString p_identifier, QString p_file,
                                       QString p_char = "");

  // Returns the value to you
  QString get_design_element(QString p_identifier, QString p_file,
                             QString p_char = "");

  // Returns the name of the font with p_identifier from p_file
  QString get_font_name(QString p_identifier, QString p_file);

  // Returns the value of font_size with p_identifier from p_file
  int get_font_size(QString p_identifier, QString p_file);

  // Returns the color with p_identifier from p_file
  QColor get_color(QString p_identifier, QString p_file);

  // Returns the markup symbol used for specified p_identifier such as colors
  QString get_chat_markup(QString p_identifier, QString p_file);

  // Returns the color from the misc folder.
  QColor get_chat_color(QString p_identifier, QString p_chat);

  // Returns the sfx with p_identifier from sounds.ini in the current theme path
  QString get_sfx(QString p_identifier);

  // Figure out if we can opus this or if we should fall back to wav
  QString get_sfx_suffix(QString sound_to_check);

  // Can we use APNG for this? If not, WEBP? If not, GIF? If not, fall back to
  // PNG.
  QString get_image_suffix(QString path_to_check);

  // If this image is static and non-animated, return the supported static image
  // formats. Currently only PNG.
  QString get_static_image_suffix(QString path_to_check);

  // Returns the value of p_search_line within target_tag and terminator_tag
  QString read_char_ini(QString p_char, QString p_search_line,
                        QString target_tag);

  // Returns a QStringList of all key=value definitions on a given tag.
  QStringList read_ini_tags(QString p_file, QString target_tag = "");

  // Sets the char.ini p_search_line key under tag target_tag to value.
  void set_char_ini(QString p_char, QString value, QString p_search_line,
                    QString target_tag);

  // Returns the text between target_tag and terminator_tag in p_file
  QString get_stylesheet(QString p_file);

  // Returns the text between target_tag and terminator_tag in p_file
  QString get_tagged_stylesheet(QString target_tag, QString p_file);

  // Returns the side of the p_char character from that characters ini file
  QString get_char_side(QString p_char);

  // Returns the showname from the ini of p_char
  QString get_showname(QString p_char);

  // Returns the value of chat image from the specific p_char's ini file
  QString get_chat(QString p_char);

  // Returns the value of chat font from the specific p_char's ini file
  QString get_chat_font(QString p_char);

  // Returns the value of chat font size from the specific p_char's ini file
  int get_chat_size(QString p_char);

  // Returns the value of shouts from the specified p_char's ini file
  QString get_char_shouts(QString p_char);

  // Returns the preanim duration of p_char's p_emote
  int get_preanim_duration(QString p_char, QString p_emote);

  // Same as above, but only returns if it has a % in front(refer to Preanims
  // section in the manual)
  int get_ao2_preanim_duration(QString p_char, QString p_emote);

  // Not in use
  int get_text_delay(QString p_char, QString p_emote);

  // Get the effects folder referenced by the char.ini, read it and return the
  // list of filenames in a string
  QStringList get_theme_effects();

  // Get the theme's effects folder, read it and return the list of filenames in
  // a string
  QStringList get_effects(QString p_char);

  // t
  QString get_effect(QString effect, QString p_char, QString p_folder);

  // Return p_property of fx_name. If p_property is "sound", return
  // the value associated with fx_name, otherwise use fx_name + '_' + p_property.
  QString get_effect_property(QString fx_name, QString p_char, QString p_property);

  // Returns the custom realisation used by the character.
  QString get_custom_realization(QString p_char);

  // Returns the name of p_char
  QString get_char_name(QString p_char);

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

  // Returns the blipsound of p_char's p_emote
  QString get_emote_blip(QString p_char, int p_emote);

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

  // Returns p_char's gender
  QString get_gender(QString p_char);

  // Get a property of a given emote, or get it from "options" if emote doesn't have it
  QString get_emote_property(QString p_char, QString p_emote, QString p_property);

  // Return a transformation mode from a string ("smooth" for smooth, anything else for fast)
  Qt::TransformationMode get_scaling(QString p_scaling);

  // Returns the scaling type for p_miscname
  Qt::TransformationMode get_misc_scaling(QString p_miscname);

  // ======
  // These are all casing-related settings.
  // ======

  // Returns if the user has casing alerts enabled.
  bool get_casing_enabled();

  // Returns if the user wants to get alerts for the defence role.
  bool get_casing_defence_enabled();

  // Same for prosecution.
  bool get_casing_prosecution_enabled();

  // Same for judge.
  bool get_casing_judge_enabled();

  // Same for juror.
  bool get_casing_juror_enabled();

  // Same for steno.
  bool get_casing_steno_enabled();

  // Same for CM.
  bool get_casing_cm_enabled();

  // Get the message for the CM for casing alerts.
  QString get_casing_can_host_cases();

  // Get if automatic logging is enabled
  bool get_auto_logging_enabled();

  // The file name of the log file in base/logs.
  QString log_filename;

  void initBASS();
  static void load_bass_opus_plugin();
  static void CALLBACK BASSreset(HSTREAM handle, DWORD channel, DWORD data,
                                 void *user);
  static void doBASSreset();

private:
  const int RELEASE = 2;
  const int MAJOR_VERSION = 8;
  const int MINOR_VERSION = 5;

  QString current_theme = "default";

  QVector<server_type> server_list;
  QVector<server_type> favorite_list;

private slots:
  void ms_connect_finished(bool connected, bool will_retry);

public slots:
  void server_disconnected();
  void loading_cancelled();
};

#endif // AOAPPLICATION_H
