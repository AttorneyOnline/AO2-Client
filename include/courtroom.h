#ifndef COURTROOM_H
#define COURTROOM_H

#include "aoapplication.h"
#include "aoblipplayer.h"
#include "aobutton.h"
#include "aocharbutton.h"
#include "aoclocklabel.h"
#include "aoemotebutton.h"
#include "aoevidencebutton.h"
#include "aoevidencedisplay.h"
#include "aoimage.h"
#include "aolayer.h"
#include "aomusicplayer.h"
#include "aooptionsdialog.h"
#include "aopacket.h"
#include "aosfxplayer.h"
#include "aotextarea.h"
#include "chatlogpiece.h"
#include "datatypes.h"
#include "debug_functions.h"
#include "file_functions.h"
#include "hardware_functions.h"
#include "lobby.h"
#include "scrolltext.h"
#include "eventfilters.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QPlainTextEdit>
#include <QSlider>
#include <QSpinBox>
#include <QTextBrowser>
#include <QTreeWidget>
#include <QVector>
#include <QQueue>

#include <QBrush>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFont>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QRegExp>
#include <QScrollBar>
#include <QTextBoundaryFinder>
#include <QTextCharFormat>
#include <QElapsedTimer>

#include <algorithm>
#include <stack>

class AOApplication;

class Courtroom : public QMainWindow {
  Q_OBJECT
public:
  explicit Courtroom(AOApplication *p_ao_app);

  void append_char(char_type p_char) { char_list.append(p_char); }
  void append_evidence(evi_type p_evi) { evidence_list.append(p_evi); }
  void append_music(QString f_music) { music_list.append(f_music); }
  void append_area(QString f_area) { area_list.append(f_area); }
  void clear_music() { music_list.clear(); }
  void clear_areas() { area_list.clear(); }

  void fix_last_area()
  {
    if (area_list.size() > 0) {
      QString malplaced = area_list.last();
      area_list.removeLast();
      append_music(malplaced);
    }
  }

  void arup_append(int players, QString status, QString cm, QString locked)
  {
    arup_players.append(players);
    arup_statuses.append(status);
    arup_cms.append(cm);
    arup_locks.append(locked);
  }

  void arup_clear() {
    arup_players.clear();
    arup_statuses.clear();
    arup_cms.clear();
    arup_locks.clear();
  }

  void arup_modify(int type, int place, QString value)
  {
    if (type == 0) {
      if (arup_players.size() > place)
        arup_players[place] = value.toInt();
    }
    else if (type == 1) {
      if (arup_statuses.size() > place)
        arup_statuses[place] = value;
    }
    else if (type == 2) {
      if (arup_cms.size() > place)
        arup_cms[place] = value;
    }
    else if (type == 3) {
      if (arup_locks.size() > place)
        arup_locks[place] = value;
    }
  }

  void character_loading_finished();

  //
  void set_courtroom_size();

  // sets position of widgets based on theme ini files
  void set_widgets();

  // sets font size based on theme ini files
  void set_font(QWidget *widget, QString class_name, QString p_identifier,
                QString p_char = "", QString font_name = "",
                int f_pointsize = 0);

  // Get the properly constructed font
  QFont get_qfont(QString font_name, int f_pointsize, bool antialias = true);

  // actual operation of setting the font on a widget
  void set_qfont(QWidget *widget, QString class_name, QFont font,
                 QColor f_color = Qt::black, bool bold = false);

  // helper function that calls above function on the relevant widgets
  void set_fonts(QString p_char = "");

  // sets dropdown menu stylesheet
  void set_stylesheet(QWidget *widget);

  // helper funciton that call above function on the relevant widgets
  void set_stylesheets();

  void set_window_title(QString p_title);

  // reads theme and sets size and pos based on the identifier (using p_misc if provided)
  void set_size_and_pos(QWidget *p_widget, QString p_identifier, QString p_misc="");

  // reads theme inis and returns the size and pos as defined by it
  QPoint get_theme_pos(QString p_identifier);

  // sets status as taken on character with cid n_char and places proper shading
  // on charselect
  void set_taken(int n_char, bool p_taken);

  // sets the current background to argument. also does some checks to see if
  // it's a legacy bg
  void set_background(QString p_background, bool display = false);

  // sets the local character pos/side to use.
  void set_side(QString p_side);

  // sets the pos dropdown
  void set_pos_dropdown(QStringList pos_dropdowns);

  // sets the evidence list member variable to argument
  void set_evidence_list(QVector<evi_type> &p_evi_list);

  // called when a DONE#% from the server was received
  void done_received();

  // sets the local mute list based on characters available on the server
  void set_mute_list();

  // Sets the local pair list based on the characters available on the server.
  void set_pair_list();

  // sets desk and bg based on pos in chatmessage
  void set_scene(QString f_desk_mod, QString f_side);

  // sets ui_vp_player_char according to SELF_OFFSET, only a function bc it's used with desk_mod 4 and 5
  void set_self_offset(QString p_list);

  // takes in serverD-formatted IP list as prints a converted version to server
  // OOC admittedly poorly named
  void set_ip_list(QString p_list);

  // disables chat if current cid matches second argument
  // enables if p_muted is false
  void set_mute(bool p_muted, int p_cid);

  // send a message that the player is banned and quits the server
  void set_ban(int p_cid);

  // cid = character id, returns the cid of the currently selected character
  int get_cid() { return m_cid; }
  QString get_current_char() { return current_char; }
  QString get_current_background() { return current_background; }

  // updates character to p_cid and updates necessary ui elements
  void update_character(int p_cid);

  // properly sets up some varibles: resets user state
  void enter_courtroom();

  // helper function that populates ui_music_list with the contents of
  // music_list
  void list_music();
  void list_areas();

  // these are for OOC chat
  void append_ms_chatmessage(QString f_name, QString f_message);
  void append_server_chatmessage(QString p_name, QString p_message,
                                 QString p_color);

  // Add the message packet to the stack
  void chatmessage_enqueue(QStringList p_contents);

  // Parse the chat message packet and unpack it into the m_chatmessage[ITEM] format
  void unpack_chatmessage(QStringList p_contents);

  // Skip the current queue, adding all the queue messages to the logs if desynchronized logs are disabled
  void skip_chatmessage_queue();

  enum LogMode {
    IO_ONLY,
    DISPLAY_ONLY,
    DISPLAY_AND_IO
  };
  // Log the message contents and information such as evidence presenting etc. into the log file, the IC log, or both.
  void log_chatmessage(QString f_message, int f_char_id, QString f_showname = "", QString f_char = "", QString f_objection_mod = "", int f_evi_id = 0, int f_color = 0, LogMode f_log_mode=IO_ONLY);

  // Log the message contents and information such as evidence presenting etc. into the IC logs
  void handle_callwords();

  // Handle the objection logic, if it's interrupting the currently parsing message.
  // Returns true if this message has an objection, otherwise returns false. The result decides when to call handle_ic_message()
  bool handle_objection();

  // Display the evidence image box when presenting evidence in IC
  void display_evidence_image();

  // Handle the stuff that comes when the character appears on screen and starts animating (preanims etc.)
  void handle_ic_message();
  
  // Display the character.
  void display_character();

  // Display the character's pair if present.
  void display_pair_character(QString other_charid, QString other_offset);

  // Handle the emote modifier value and proceed through the logic accordingly.
  void handle_emote_mod(int emote_mod, bool p_immediate);

  // Initialize the chatbox image, showname shenanigans, custom chatboxes, etc.
  void initialize_chatbox();

  // Finally start displaying the chatbox we initialized, display the evidence, and play the talking or idle emote for the character.
  // Callwords are also handled here.
  void handle_ic_speaking();

  // This function filters out the common CC inline text trickery, for appending
  // to the IC chatlog.
  QString filter_ic_text(QString p_text, bool colorize = false, int pos = -1,
                         int default_color = 0);

  void log_ic_text(QString p_name, QString p_showname, QString p_message,
                   QString p_action = "", int p_color = 0);

  // adds text to the IC chatlog. p_name first as bold then p_text then a newlin
  // this function keeps the chatlog scrolled to the top unless there's text
  // selected
  // or the user isn't already scrolled to the top
  void append_ic_text(QString p_text, QString p_name = "", QString action = "",
                      int color = 0, QDateTime timestamp = QDateTime::currentDateTime());

  // prints who played the song to IC chat and plays said song(if found on local
  // filesystem) takes in a list where the first element is the song name and
  // the second is the char id of who played it
  void handle_song(QStringList *p_contents);

  void play_preanim(bool immediate);

  // plays the witness testimony or cross examination animation based on
  // argument
  void handle_wtce(QString p_wtce, int variant);

  // sets the hp bar of defense(p_bar 1) or pro(p_bar 2)
  // state is an number between 0 and 10 inclusive
  void set_hp_bar(int p_bar, int p_state);

  // Toggles the judge buttons, whether they should appear or not.
  void toggle_judge_buttons(bool is_on);

  void announce_case(QString title, bool def, bool pro, bool jud, bool jur,
                     bool steno);

  void check_connection_received();

  void start_clock(int id);
  void start_clock(int id, qint64 msecs);
  void set_clock(int id, qint64 msecs);
  void pause_clock(int id);
  void stop_clock(int id);
  void set_clock_visibility(int id, bool visible);
  void skip_clocks(qint64 msecs);

  qint64 pong();
  // Truncates text so it fits within theme-specified boundaries and sets the tooltip to the full string
  void truncate_label_text(QWidget* p_widget, QString p_identifier);

  void on_authentication_state_received(int p_state);

  ~Courtroom();
private:
  AOApplication *ao_app;

  int m_courtroom_width = 714;
  int m_courtroom_height = 668;

  int m_viewport_x = 0;
  int m_viewport_y = 0;

  int m_viewport_width = 256;
  int m_viewport_height = 192;

  int maximumMessages = 0;

  QParallelAnimationGroup *screenshake_animation_group =
      new QParallelAnimationGroup;

  bool next_character_is_not_special = false; // If true, write the
                                              // next character as it is.

  bool message_is_centered = false;

  int current_display_speed = 3;
  int text_crawl = 40;
  double message_display_mult[7] = {0, 0.25, 0.65, 1, 1.25, 1.75, 2.25};

  // The character ID of the character this user wants to appear alongside with.
  int other_charid = -1;

  // The horizontal offset this user has given if they want to appear alongside someone.
  int char_offset = 0;

  // The vertical offset this user has given.
  int char_vert_offset = 0;

  // 0 = in front, 1 = behind
  int pair_order = 0;

  QVector<char_type> char_list;
  QVector<evi_type> evidence_list;
  QVector<QString> music_list;
  QVector<QString> area_list;

  QVector<int> arup_players;
  QVector<QString> arup_statuses;
  QVector<QString> arup_cms;
  QVector<QString> arup_locks;

  QVector<chatlogpiece> ic_chatlog_history;
  QString last_ic_message = "";

  QQueue<QStringList> chatmessage_queue;

  // triggers ping_server() every 45 seconds
  QTimer *keepalive_timer;

  // determines how fast messages tick onto screen
  QTimer *chat_tick_timer;

  // count up timer to check how long it took for us to get a response from ping_server()
  QElapsedTimer ping_timer;
  bool is_pinging = false;

  // int chat_tick_interval = 60;
  // which tick position(character in chat message) we are at
  int tick_pos = 0;
  // the actual document tick pos we gotta worry about for making the text
  // scroll better
  int real_tick_pos = 0;
  // used to determine how often blips sound
  int blip_ticker = 0;
  int blip_rate = 2;
  int rainbow_counter = 0;
  bool rainbow_appended = false;
  bool blank_blip = false;

  // Used for getting the current maximum blocks allowed in the IC chatlog.
  int log_maximum_blocks = 0;

  // True, if the log should go downwards.
  bool log_goes_downwards = true;

  // True, if log should display colors.
  bool log_colors = true;

  // True, if the log should display the message like name<br>text instead of
  // name: text
  bool log_newline = false;

  // True, if the log should include RP actions like interjections, showing evidence, etc.
  bool log_ic_actions = true;

  // Margin in pixels between log entries for the IC log.
  int log_margin = 0;

  // True, if the log should have a timestamp.
  bool log_timestamp = false;

  // format string for aforementioned log timestamp
  QString log_timestamp_format;

  // How long in miliseconds should the objection wait before appearing.
  int objection_threshold = 1500;

  // delay before chat messages starts ticking
  QTimer *text_delay_timer;
  
  // delay before the next queue entry is going to be processed
  QTimer *text_queue_timer;

  // delay before sfx plays
  QTimer *sfx_delay_timer;

  // every time point in char.inis times this equals the final time
  const int time_mod = 40;

  // the amount of time non-animated objection/hold it/takethat images stay
  // onscreen for in ms, and the maximum amount of time any interjections are
  // allowed to play
  const int shout_static_time = 724;
  const int shout_max_time = 1500;

  // the amount of time non-animated guilty/not guilty images stay onscreen for
  // in ms, and the maximum amount of time g/ng images are allowed to play
  const int verdict_static_time = 3000;
  const int verdict_max_time = 4000;

  // the amount of time non-animated witness testimony/cross-examination images
  // stay onscreen for in ms, and the maximum time any wt/ce image is allowed to
  // play
  const int wtce_static_time = 1500;
  const int wtce_max_time = 4000;

  // characters we consider punctuation
  const QString punctuation_chars = ".,?!:;";

  // amount by which we multiply the delay when we parse punctuation chars
  const int punctuation_modifier = 3;

  // Minumum and maximum number of parameters in the MS packet
  static const int MS_MINIMUM = 15;
  static const int MS_MAXIMUM = 30;
  QString m_chatmessage[MS_MAXIMUM];

  QString previous_ic_message = "";
  QString additive_previous = "";

  // char id, muted or not
  QMap<int, bool> mute_map;

  // QVector<int> muted_cids;

  bool is_muted = false;

  // state of animation, 0 = objecting, 1 = preanim, 2 = talking, 3 = idle, 4 =
  // noniterrupting preanim, 5 = (c) animation
  int anim_state = 3;

  // whether or not current color is a talking one
  bool color_is_talking = true;

  // state of text ticking, 0 = not yet ticking, 1 = ticking in progress, 2 =
  // ticking done
  int text_state = 2;

  // character id, which index of the char_list the player is
  int m_cid = -1;
  // cid and this may differ in cases of ini-editing
  QString current_char = "";

  int objection_state = 0;
  QString objection_custom = "";
  struct CustomObjection {
    QString name;
    QString filename;
  };
  QList<CustomObjection> custom_objections_list;
  int realization_state = 0;
  int screenshake_state = 0;
  int text_color = 0;

  // How many unique user colors are possible
  static const int max_colors = 12;

  // Text Color-related optimization:
  // Current color list indexes to real color references
  QVector<int> color_row_to_number;

  // List of associated RGB colors for this color index
  QVector<QColor> color_rgb_list;

  // Same as above but populated from misc/default's config
  QVector<QColor> default_color_rgb_list;

  // Get a color index from an arbitrary misc config
  void gen_char_rgb_list(QString p_misc);
  QVector<QColor> char_color_rgb_list;

  // Misc we used for the last message, and the one we're using now. Used to avoid loading assets when it's not needed
  QString current_misc;
  QString last_misc;

  // List of markdown start characters, their index is tied to the color index
  QStringList color_markdown_start_list;

  // List of markdown end characters, their index is tied to the color index
  QStringList color_markdown_end_list;

  // Whether or not we're supposed to remove this char during parsing
  QVector<bool> color_markdown_remove_list;

  // Whether or not this color allows us to play the talking animation
  QVector<bool> color_markdown_talking_list;
  // Text Color-related optimization END

  // List of all currently available pos
  QStringList pos_dropdown_list;

  // Current list file sorted line by line
  QStringList sound_list;

  // Current SFX the user put in for the sfx dropdown list
  QString custom_sfx = "";

  // is the message we're about to send supposed to present evidence?
  bool is_presenting_evidence = false;
  bool c_played = false; // whether we've played a (c)-style postanimation yet

  // have we already presented evidence for this message?
  bool evidence_presented = false;

  QString effect = "";

  // Music effect flags we want to send to server when we play music
  int music_flags = FADE_OUT;

  int defense_bar_state = 0;
  int prosecution_bar_state = 0;

  int current_char_page = 0;
  int char_columns = 10;
  int char_rows = 9;
  int max_chars_on_page = 90;

  const int button_width = 60;
  const int button_height = 60;

  int current_emote_page = 0;
  int current_emote = 0;
  int emote_columns = 5;
  int emote_rows = 2;
  int max_emotes_on_page = 10;

  QVector<evi_type> local_evidence_list;
  QVector<evi_type> private_evidence_list;
  QVector<evi_type> global_evidence_list;

  // false = use private_evidence_list
  bool current_evidence_global = true;

  int current_evidence_page = 0;
  int current_evidence = 0;
  int evidence_columns = 6;
  int evidence_rows = 3;
  int max_evidence_on_page = 18;

  // whether the ooc chat is server or master chat, true is server
  bool server_ooc = true;

  QString current_background = "default";
  QString current_side = "";

  QString last_music_search = "";
  QString last_area_search = "";

  QBrush free_brush;
  QBrush lfp_brush;
  QBrush casing_brush;
  QBrush recess_brush;
  QBrush rp_brush;
  QBrush gaming_brush;
  QBrush locked_brush;

  AOMusicPlayer *music_player;
  AOSfxPlayer *sfx_player;
  AOSfxPlayer *objection_player;
  AOBlipPlayer *blip_player;

  AOSfxPlayer *modcall_player;

  AOImage *ui_background;

  QWidget *ui_viewport;
  BackgroundLayer *ui_vp_background;
  SplashLayer *ui_vp_speedlines;
  CharLayer *ui_vp_player_char;
  CharLayer *ui_vp_sideplayer_char;
  BackgroundLayer *ui_vp_desk;
  AOEvidenceDisplay *ui_vp_evidence_display;
  AOImage *ui_vp_chatbox;
  QLabel *ui_vp_showname;
  InterfaceLayer *ui_vp_chat_arrow;
  QTextEdit *ui_vp_message;
  SplashLayer *ui_vp_testimony;
  SplashLayer *ui_vp_wtce;
  EffectLayer *ui_vp_effect;
  SplashLayer *ui_vp_objection;

  QTextEdit *ui_ic_chatlog;

  AOTextArea *ui_ms_chatlog;
  AOTextArea *ui_server_chatlog;

  QListWidget *ui_mute_list;
  QTreeWidget *ui_area_list;
  QTreeWidget *ui_music_list;

  ScrollText *ui_music_name;
  InterfaceLayer *ui_music_display;

  StickerLayer *ui_vp_sticker;

  static const int max_clocks = 5;
  AOClockLabel *ui_clock[max_clocks];

  AOButton *ui_pair_button;
  QListWidget *ui_pair_list;
  QSpinBox *ui_pair_offset_spinbox;
  QSpinBox *ui_pair_vert_offset_spinbox;

  QComboBox *ui_pair_order_dropdown;

  QLineEdit *ui_ic_chat_message;
  AOLineEditFilter *ui_ic_chat_message_filter;
  QLineEdit *ui_ic_chat_name;

  QLineEdit *ui_ooc_chat_message;
  QLineEdit *ui_ooc_chat_name;

  // QLineEdit *ui_area_password;
  QLineEdit *ui_music_search;

  QWidget *ui_emotes;
  QVector<AOEmoteButton *> ui_emote_list;
  AOButton *ui_emote_left;
  AOButton *ui_emote_right;

  QComboBox *ui_emote_dropdown;
  QComboBox *ui_pos_dropdown;
  AOButton *ui_pos_remove;

  QComboBox *ui_iniswap_dropdown;
  AOButton *ui_iniswap_remove;

  QComboBox *ui_sfx_dropdown;
  AOButton *ui_sfx_remove;

  QComboBox *ui_effects_dropdown;

  AOImage *ui_defense_bar;
  AOImage *ui_prosecution_bar;

  QLabel *ui_music_label;
  QLabel *ui_sfx_label;
  QLabel *ui_blip_label;

  AOButton *ui_hold_it;
  AOButton *ui_objection;
  AOButton *ui_take_that;

  AOButton *ui_ooc_toggle;

  AOButton *ui_witness_testimony;
  AOButton *ui_cross_examination;
  AOButton *ui_guilty;
  AOButton *ui_not_guilty;

  AOButton *ui_change_character;
  AOButton *ui_reload_theme;
  AOButton *ui_call_mod;
  AOButton *ui_settings;
  AOButton *ui_announce_casing;
  AOButton *ui_switch_area_music;

  QCheckBox *ui_pre;
  QCheckBox *ui_flip;
  QCheckBox *ui_additive;
  QCheckBox *ui_guard;
  QCheckBox *ui_casing;

  QCheckBox *ui_immediate;
  QCheckBox *ui_showname_enable;

  AOButton *ui_custom_objection;
  QMenu *custom_obj_menu;
  AOButton *ui_realization;
  AOButton *ui_screenshake;
  AOButton *ui_mute;

  AOButton *ui_defense_plus;
  AOButton *ui_defense_minus;

  AOButton *ui_prosecution_plus;
  AOButton *ui_prosecution_minus;

  QComboBox *ui_text_color;

  QSlider *ui_music_slider;
  QSlider *ui_sfx_slider;
  QSlider *ui_blip_slider;

  AOImage *ui_muted;

  AOButton *ui_evidence_button;
  AOImage *ui_evidence;
  QLineEdit *ui_evidence_name;
  AOLineEditFilter *ui_evidence_name_filter;
  QWidget *ui_evidence_buttons;
  QVector<AOEvidenceButton *> ui_evidence_list;
  AOButton *ui_evidence_left;
  AOButton *ui_evidence_right;
  AOButton *ui_evidence_present;
  AOImage *ui_evidence_overlay;
  AOButton *ui_evidence_delete;
  QLineEdit *ui_evidence_image_name;
  AOLineEditFilter *ui_evidence_image_name_filter;
  AOButton *ui_evidence_image_button;
  AOButton *ui_evidence_x;
  AOButton *ui_evidence_ok;
  AOButton *ui_evidence_switch;
  AOButton *ui_evidence_transfer;
  AOButton *ui_evidence_save;
  AOButton *ui_evidence_load;
  AOButton *ui_evidence_edit;
  QPlainTextEdit *ui_evidence_description;


  AOImage *ui_char_select_background;

  // pretty list of characters
  QTreeWidget *ui_char_list;

  // abstract widget to hold char buttons
  QWidget *ui_char_buttons;

  QVector<AOCharButton *> ui_char_button_list;
  QVector<AOCharButton *> ui_char_button_list_filtered;

  AOButton *ui_back_to_lobby;

  QLineEdit *ui_char_password;

  AOButton *ui_char_select_left;
  AOButton *ui_char_select_right;

  AOButton *ui_spectator;

  QLineEdit *ui_char_search;
  QCheckBox *ui_char_passworded;
  QCheckBox *ui_char_taken;

  void construct_char_select();
  void set_char_select();
  void set_char_select_page();
  void char_clicked(int n_char);
  void put_button_in_place(int starting, int chars_on_this_page);
  void filter_character_list();

  void initialize_emotes();
  void refresh_emotes();
  void set_emote_page();
  void set_emote_dropdown();

  void initialize_evidence();
  void refresh_evidence();
  void set_evidence_page();

  void reset_ui();

  void regenerate_ic_chatlog();
public slots:
  void objection_done();
  void effect_done();
  void preanim_done();
  void do_screenshake();
  void do_flash();
  void do_effect(QString fx_path, QString fx_sound, QString p_char,
                 QString p_folder);
  void play_char_sfx(QString sfx_name);

  void mod_called(QString p_ip);

  void case_called(QString msg, bool def, bool pro, bool jud, bool jur,
                   bool steno);
  void on_reload_theme_clicked();

private slots:
  void start_chat_ticking();
  void play_sfx();

  void chat_tick();

  void on_mute_list_clicked(QModelIndex p_index);
  void on_pair_list_clicked(QModelIndex p_index);

  void on_chat_return_pressed();

  void on_ooc_return_pressed();

  void on_music_search_return_pressed();
  void on_music_search_edited(QString p_text);
  void on_music_list_double_clicked(QTreeWidgetItem *p_item, int column);
  void on_music_list_context_menu_requested(const QPoint &pos);
  void music_fade_out(bool toggle);
  void music_fade_in(bool toggle);
  void music_synchronize(bool toggle);
  void music_random();
  void music_list_expand_all();
  void music_list_collapse_all();
  void music_stop(bool no_effects = false);
  void on_area_list_double_clicked(QTreeWidgetItem *p_item, int column);

  void select_emote(int p_id);

  void on_emote_clicked(int p_id);

  void on_emote_left_clicked();
  void on_emote_right_clicked();

  void on_emote_dropdown_changed(int p_index);
  void on_pos_dropdown_changed(int p_index);
  void on_pos_dropdown_changed(QString p_text);
  void on_pos_remove_clicked();

  void on_iniswap_dropdown_changed(int p_index);
  void set_iniswap_dropdown();
  void on_iniswap_context_menu_requested(const QPoint &pos);
  void on_iniswap_edit_requested();
  void on_iniswap_remove_clicked();

  void on_sfx_dropdown_changed(int p_index);
  void on_sfx_dropdown_custom(QString p_sfx);
  void set_sfx_dropdown();
  void on_sfx_context_menu_requested(const QPoint &pos);
  void on_sfx_edit_requested();
  void on_sfx_remove_clicked();

  void set_effects_dropdown();
  void on_effects_context_menu_requested(const QPoint &pos);
  void on_effects_edit_requested();
  void on_character_effects_edit_requested();
  void on_effects_dropdown_changed(int p_index);
  bool effects_dropdown_find_and_set(QString effect);

  QString get_char_sfx();
  int get_char_sfx_delay();

  void on_evidence_name_edited();
  void on_evidence_image_name_edited();
  void on_evidence_image_button_clicked();
  void on_evidence_clicked(int p_id);
  void on_evidence_double_clicked(int p_id);
  void on_evidence_edit_clicked();

  void on_evidence_hover(int p_id, bool p_state);

  void on_evidence_left_clicked();
  void on_evidence_right_clicked();
  void on_evidence_present_clicked();

  void on_hold_it_clicked();
  void on_objection_clicked();
  void on_take_that_clicked();
  void on_custom_objection_clicked();
  void show_custom_objection_menu(const QPoint &pos);

  void on_realization_clicked();
  void on_screenshake_clicked();

  void on_mute_clicked();
  void on_pair_clicked();
  void on_pair_order_dropdown_changed(int p_index);

  void on_defense_minus_clicked();
  void on_defense_plus_clicked();
  void on_prosecution_minus_clicked();
  void on_prosecution_plus_clicked();

  void on_text_color_changed(int p_color);
  void set_text_color_dropdown();

  void on_music_slider_moved(int p_value);
  void on_sfx_slider_moved(int p_value);
  void on_blip_slider_moved(int p_value);

  void on_log_limit_changed(int value);
  void on_pair_offset_changed(int value);
  void on_pair_vert_offset_changed(int value);

  void on_ooc_toggle_clicked();

  void on_witness_testimony_clicked();
  void on_cross_examination_clicked();
  void on_not_guilty_clicked();
  void on_guilty_clicked();

  void on_change_character_clicked();
  void on_call_mod_clicked();
  void on_settings_clicked();
  void on_announce_casing_clicked();

  void on_pre_clicked();
  void on_flip_clicked();
  void on_additive_clicked();
  void on_guard_clicked();

  void on_showname_enable_clicked();

  void on_evidence_button_clicked();

  void on_evidence_delete_clicked();
  void on_evidence_x_clicked();
  void on_evidence_ok_clicked();
  void on_evidence_switch_clicked();
  void on_evidence_transfer_clicked();

  void on_evidence_edited();

  void evidence_close();
  void evidence_switch(bool global);
  void on_evidence_save_clicked();
  void on_evidence_load_clicked();
  bool compare_evidence_changed(evi_type evi_a, evi_type evi_b);

  void on_back_to_lobby_clicked();

  void on_char_list_double_clicked(QTreeWidgetItem *p_item, int column);
  void on_char_select_left_clicked();
  void on_char_select_right_clicked();
  void on_char_search_changed();
  void on_char_taken_clicked();
  void on_char_passworded_clicked();

  void on_spectator_clicked();

  void on_switch_area_music_clicked();

  void on_casing_clicked();

  void ping_server();

  // Proceed to parse the oldest chatmessage and remove it from the stack
  void chatmessage_dequeue();
};

#endif // COURTROOM_H
