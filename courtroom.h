#ifndef COURTROOM_H
#define COURTROOM_H

#include "aoimage.h"
#include "aobutton.h"
#include "aocharbutton.h"
#include "aoemotebutton.h"
#include "aopacket.h"
#include "aoscene.h"
#include "aomovie.h"
#include "aocharmovie.h"

#include "aobasshandle.hpp"
#include "aoblipplayer.h"
#include "aomusicplayer.h"
#include "aosfxplayer.h"
#include "aoshoutplayer.hpp"

#include "aoevidencebutton.h"
#include "aotextarea.h"
#include "aolineedit.h"
#include "aotextedit.h"
#include "aoevidencedisplay.h"
#include "aonotepad.h"
#include "aonotearea.hpp"
#include "datatypes.h"

#include <QMainWindow>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QVector>
#include <QCloseEvent>
#include <QSignalMapper>
#include <QMap>
#include <QTextBrowser>
#include <QRect>
#include <QComboBox>
#include <QDateTime>
#include <QPropertyAnimation>

class AOApplication;

class Courtroom : public QMainWindow
{
  Q_OBJECT
public:
  explicit Courtroom(AOApplication *p_ao_app);
  virtual ~Courtroom();

  void append_char(char_type p_char){char_list.append(p_char);}
  void append_evidence(evi_type p_evi){evidence_list.append(p_evi);}
  void append_music(QString f_music){music_list.append(f_music);}

  //sets position of widgets based on theme ini files
  void set_widgets();
  //sets font size based on theme ini files
  void set_font(QWidget *widget, QString p_identifier);
  //helper function that calls above function on the relevant widgets
  void set_fonts();

  //sets dropdown menu stylesheet
  void set_dropdown(QWidget *widget, QString target_tag);

  //helper funciton that call above function on the relevant widgets
  void set_dropdowns();

  void set_window_title(QString p_title);

  //reads theme inis and sets size and pos based on the identifier
  void set_size_and_pos(QWidget *p_widget, QString p_identifier);

  //sets status as taken on character with cid n_char and places proper shading on charselect
  void set_taken(int n_char, bool p_taken);

  //sets the current background to argument. also does some checks to see if it's a legacy bg
  void set_background(QString p_background);

  //sets the evidence list member variable to argument
  void set_evidence_list(QVector<evi_type> &p_evi_list);

  //called when a DONE#% from the server was received
  void done_received();

  //sets the local mute list based on characters available on the server
  void set_mute_list();

  //sets desk and bg based on pos in chatmessage
  void set_scene();

  //sets text color based on text color in chatmessage
  void set_text_color();

  //takes in serverD-formatted IP list as prints a converted version to server OOC
  //admittedly poorly named
  void set_ip_list(QString p_list);

  //disables chat if current cid matches second argument
  //enables if p_muted is false
  void set_mute(bool p_muted, int p_cid);

  //send a message that the player is banned and quits the server
  void set_ban(int p_cid);

  //implementations in path_functions.cpp
  QString get_background_path();
  QString get_default_background_path();

  //cid = character id, returns the cid of the currently selected character
  int get_cid() {return m_cid;}
  QString get_current_char() {return current_char;}

  //properly sets up some varibles: resets user state
  void enter_courtroom(int p_cid);

  //helper function that populates ui_music_list with the contents of music_list
  void list_music();

  void list_sfx();

  void list_themes();

  void list_note_files();

  void set_note_files();

  void move_widget(QWidget *p_widget, QString p_identifier);

  void set_shouts();

  //these are for OOC chat
  void append_ms_chatmessage(QString f_name, QString f_message);
  void append_server_chatmessage(QString p_name, QString p_message);

  //these functions handle chatmessages sequentially.
  //The process itself is very convoluted and merits separate documentation
  //But the general idea is objection animation->pre animation->talking->idle
  void handle_chatmessage(QStringList *p_contents);
  void handle_chatmessage_2();
  void handle_chatmessage_3();

  //handles character portrait animation
  void handle_char_anim(AOCharMovie *charPlayer);
  void handle_char_anim_2(AOCharMovie *charPlayer);

  //adds text to the IC chatlog. p_name first as bold then p_text then a newlin
  //this function keeps the chatlog scrolled to the top unless there's text selected
  // or the user isn't already scrolled to the top
  void append_ic_text(QString p_text, QString p_name = "");

  //prints who played the song to IC chat and plays said song(if found on local filesystem)
  //takes in a list where the first element is the song name and the second is the char id of who played it
  void handle_song(QStringList *p_contents);

  //animates music text
  void handle_music_anim(QString p_identifier_a, QString p_identifier_b);

  void play_preanim();

  //plays the witness testimony or cross examination animation based on argument
  void handle_wtce(QString p_wtce);

  //sets the hp bar of defense(p_bar 1) or pro(p_bar 2)
  //state is an number between 0 and 10 inclusive
  void set_hp_bar(int p_bar, int p_state);

  void check_connection_received();

  //checks whether shout files are found
  void check_shouts();

private:
  AOApplication *ao_app = nullptr;

  int m_courtroom_width = 714;
  int m_courtroom_height = 668;

  int m_viewport_x = 0;
  int m_viewport_y = 0;

  int m_viewport_width = 256;
  int m_viewport_height = 192;

  QVector<char_type> char_list;
  QVector<evi_type> evidence_list;
  QVector<QString> music_list;
  QVector<QString> sfx_names;
  QVector<QString> note_list;

  QSignalMapper *char_button_mapper;

  //triggers ping_server() every 60 seconds
  QTimer *keepalive_timer;

  //determines how fast messages tick onto screen
  QTimer *chat_tick_timer;
  int chat_tick_interval = 60;
  //which tick position(character in chat message) we are at
  int tick_pos = 0;
  //used to determine how often blips sound
  int blip_pos = 0;
  int blip_rate = 1;
  int rainbow_counter = 0;
  bool rainbow_appended = false;
  bool blank_blip = false;
  bool note_shown = false;
  bool contains_add_button = false;

  //////////////
  QScrollArea *note_scroll_area;

  //delay before chat messages starts ticking
  QTimer *text_delay_timer;

  //delay before sfx plays
  QTimer *sfx_delay_timer;

  //keeps track of how long realization is visible(it's just a white square and should be visible less than a second)
  QTimer *realization_timer;

  //times how long the blinking testimony should be shown(green one in the corner)
  QTimer *testimony_show_timer;
  //times how long the blinking testimony should be hidden
  QTimer *testimony_hide_timer;

  //Generate a File Name based on the time you launched the client
  QString icchatlogsfilename = QDateTime::currentDateTime().toString("'logs/'ddd MMMM yyyy hh.mm.ss.z'.txt'");

  //every time point in char.inis times this equals the final time
  const int time_mod = 40;

  static const int chatmessage_size = 15;
  QString m_chatmessage[chatmessage_size];
  bool chatmessage_is_empty = false;

  QString previous_ic_message = "";

  bool testimony_in_progress = false;

  //in milliseconds
  const int testimony_show_time = 1500;

  //in milliseconds
  const int testimony_hide_time = 500;

  //char id, muted or not
  QMap<int, bool> mute_map;

  //QVector<int> muted_cids;

  bool is_muted = false;

  //state of animation, 0 = objecting, 1 = preanim, 2 = talking, 3 = idle
  int anim_state = 3;

  //state of text ticking, 0 = not yet ticking, 1 = ticking in progress, 2 = ticking done
  int text_state = 2;

  //character id, which index of the char_list the player is
  int m_cid = -1;
  //cid and this may differ in cases of ini-editing
  QString current_char = "";

  QString current_file = "";

  int m_objection_state = 0;
  int m_effect_state = 0;
  int m_text_color = 0;
  int m_shout_state = 0;
  bool is_presenting_evidence = false;

  int defense_bar_state = 0;
  int prosecution_bar_state = 0;

  int current_char_page = 0;
  int char_columns = 10;
  int char_rows = 9;
  int max_chars_on_page = 90;

  int current_emote_page = 0;
  int current_emote = 0;
  int prev_emote = 0;
  int emote_columns = 5;
  int emote_rows = 2;
  int max_emotes_on_page = 10;

  bool same_emote = false;

//  int note_amount = 0;

  QVector<evi_type> local_evidence_list;

  int current_evidence_page = 0;
  int current_evidence = 0;
  int evidence_columns = 6;
  int evidence_rows = 3;
  int max_evidence_on_page = 18;

  //is set to true if the bg folder contains defensedesk.png, prosecutiondesk.png and stand.png
  bool is_ao2_bg = false;

  //whether the ooc chat is server or master chat, true is server
  bool server_ooc = true;

  QString current_background = "gs4";

  AOBlipPlayer*  m_blip_player = nullptr;
  AOSfxPlayer*   m_mod_player = nullptr;
  AOMusicPlayer* m_music_player = nullptr;
  AOSfxPlayer*   m_sfx_player = nullptr;
  AOShoutPlayer* m_shout_player = nullptr;

  AOImage *ui_background;

  QWidget *ui_viewport;
  AOScene *ui_vp_background;
  AOMovie *ui_vp_speedlines;
  AOCharMovie *ui_vp_player_char;
  AOScene *ui_vp_desk;
  AOScene *ui_vp_legacy_desk;
  AOEvidenceDisplay *ui_vp_evidence_display;

  AONoteArea *ui_note_area;

//  AONotepad *ui_vp_notepad;

  // list of characters that require a second application ID
  // note that since it's hardcoded, it won't be of much use in other servers
  QVector<QString> rpc_char_list;

  AOImage *ui_vp_notepad_image;
  QTextEdit *ui_vp_notepad;

  AOImage* ui_vp_chatbox = nullptr;
  QLabel* ui_vp_showname = nullptr;
  QTextEdit* ui_vp_message = nullptr;
  AOImage* ui_vp_testimony = nullptr;
  AOMovie* ui_vp_effect = nullptr;
  AOMovie* ui_vp_wtce = nullptr;
  AOMovie* ui_vp_objection = nullptr;

  AOImage* ui_vp_music_display_a = nullptr;
  AOImage* ui_vp_music_display_b = nullptr;

  QTextEdit*          ui_vp_music_name = nullptr;
  QPropertyAnimation* music_anim = nullptr;

  QWidget *ui_vp_music_area;

  QTextEdit* ui_ic_chatlog = nullptr;
  QVector<record_type_ptr> m_ic_records;

  AOTextArea *ui_ms_chatlog;
  AOTextArea *ui_server_chatlog;

  QListWidget *ui_mute_list;
  QListWidget *ui_area_list;
  QListWidget *ui_music_list;
  QListWidget *ui_sfx_list;

//  QListWidget *ui_sfx_list;

  QLineEdit *ui_ic_chat_message;

  QLineEdit *ui_ooc_chat_message;
  QLineEdit *ui_ooc_chat_name;

  //QLineEdit *ui_area_password;
  QLineEdit *ui_music_search;

  QLineEdit *ui_sfx_search;

  QWidget *ui_emotes;
  QVector<AOEmoteButton*> ui_emote_list;
  AOButton *ui_emote_left;
  AOButton *ui_emote_right;

  QComboBox *ui_emote_dropdown;
  QComboBox *ui_pos_dropdown;

  AOImage *ui_defense_bar;
  AOImage *ui_prosecution_bar;

  QLabel *ui_music_label;
  QLabel *ui_sfx_label;
  QLabel *ui_blip_label;

  //buttons to cycle through shouts
  AOButton* ui_shout_up = nullptr;
  AOButton* ui_shout_down = nullptr;

  //holds all the shout button objects
  QVector<AOButton*> ui_shouts;

  //holds all the names for sound files for the shouts
  QVector<QString> shout_names = {"holdit", "objection", "takethat", "custom", "gotit", "crossswords", "counteralt"};

  //holds whether the sound file exists for a determined shout
  QVector<bool> shouts_enabled;

//  AOButton* ui_shout_hold_it      = nullptr; // 1
//  AOButton* ui_shout_objection    = nullptr; // 2
//  AOButton* ui_shout_take_that    = nullptr; // 3
//  AOButton* ui_shout_custom       = nullptr; // 4
//  AOButton* ui_shout_got_it       = nullptr; // 5
//  AOButton* ui_shout_cross_swords = nullptr; // 6
//  AOButton* ui_shout_counter_alt  = nullptr; // 7

  AOButton *ui_ooc_toggle;

  AOButton *ui_witness_testimony;
  AOButton *ui_cross_examination;

  AOButton *ui_change_character;
  AOButton *ui_reload_theme;
  AOButton *ui_call_mod;

  QComboBox *ui_theme_list;

  AOButton *ui_confirm_theme;

  AOButton *ui_set_notes;

  QCheckBox *ui_pre;
  QCheckBox *ui_flip;
  QCheckBox *ui_guard;

  AOButton* ui_effect_flash = nullptr;
  AOButton* ui_effect_gloom = nullptr;

  AOButton* ui_mute = nullptr;

  AOButton *ui_defense_plus;
  AOButton *ui_defense_minus;

  AOButton *ui_prosecution_plus;
  AOButton *ui_prosecution_minus;

  QComboBox *ui_text_color;

  QSlider *ui_music_slider;
  QSlider *ui_sfx_slider;
  QSlider *ui_blip_slider;

  AOImage *ui_muted;

  AOButton *ui_note_button;

  AOButton *ui_evidence_button;
  AOImage *ui_evidence;
  AOLineEdit *ui_evidence_name;
  QWidget *ui_evidence_buttons;
  QVector<AOEvidenceButton*> ui_evidence_list;
  AOButton *ui_evidence_left;
  AOButton *ui_evidence_right;
  AOButton *ui_evidence_present;
  AOImage *ui_evidence_overlay;
  AOButton *ui_evidence_delete;
  AOLineEdit *ui_evidence_image_name;
  AOButton *ui_evidence_image_button;
  AOButton *ui_evidence_x;
  AOTextEdit *ui_evidence_description;

  AOImage *ui_char_select_background;

  //abstract widget to hold char buttons
  QWidget *ui_char_buttons;

  QVector<AOCharButton*> ui_char_button_list;
  AOImage *ui_selector;

  AOButton *ui_back_to_lobby;

  QLineEdit *ui_char_password;

  AOButton *ui_char_select_left;
  AOButton *ui_char_select_right;

  AOButton *ui_spectator;

  void construct_char_select();
  void set_char_select();
  void set_char_select_page();

  void construct_emotes();
  void set_emote_page();
  void set_emote_dropdown();

  void construct_evidence();
  void set_evidence_page();

  void load_note();
  void save_note();
  void save_textlog(QString p_text);

  void set_char_rpc();


public slots:
  void objection_done();
  void preanim_done();

  void realization_done();

  void show_testimony();
  void hide_testimony();

  void mod_called(QString p_ip);

private slots:
  void start_chat_ticking();
  void play_sfx();

  void chat_tick();

  void on_mute_list_clicked(QModelIndex p_index);

  void on_chat_return_pressed();

  void on_ooc_return_pressed();

  void on_music_search_edited(QString p_text);
  void on_music_list_double_clicked(QModelIndex p_model);

  void on_sfx_search_edited(QString p_text);

  void select_emote(int p_id);

  void on_emote_clicked(int p_id);

  void on_emote_left_clicked();
  void on_emote_right_clicked();

  void on_emote_dropdown_changed(int p_index);
  void on_pos_dropdown_changed(int p_index);

  void on_evidence_name_edited();
  void on_evidence_image_name_edited();
  void on_evidence_image_button_clicked();
  void on_evidence_clicked(int p_id);
  void on_evidence_double_clicked(int p_id);

  void on_evidence_hover(int p_id, bool p_state);

  void on_evidence_left_clicked();
  void on_evidence_right_clicked();
  void on_evidence_present_clicked();

  void on_cycle_clicked();

  void cycle_shout(int p_index);

  void on_add_button_clicked();

  void on_delete_button_clicked();

  void on_set_file_button_clicked();

  void on_file_selected();

  /**
   * @brief reset the shout button's texture to default
   * DOES NOT MODIFY OBJECTION_STATE
   */
  void reset_shout_buttons();

  /**
   * @brief a general purpose function to toggle button selection
   */
  void on_shout_clicked();

  void reset_effect_buttons();
  void on_effect_button_clicked();

  void on_mute_clicked();

  void on_defense_minus_clicked();
  void on_defense_plus_clicked();
  void on_prosecution_minus_clicked();
  void on_prosecution_plus_clicked();

  void on_text_color_changed(int p_color);

  void on_music_slider_moved(int p_value);
  void on_sfx_slider_moved(int p_value);
  void on_blip_slider_moved(int p_value);

  void on_ooc_toggle_clicked();

  void on_witness_testimony_clicked();
  void on_cross_examination_clicked();

  void on_change_character_clicked();
  void on_reload_theme_clicked();
  void on_call_mod_clicked();

  void on_confirm_theme_clicked();
  void on_note_button_clicked();

  void on_set_notes_clicked();

  void on_note_text_changed();

  void on_pre_clicked();
  void on_flip_clicked();
  void on_guard_clicked();

  void on_sfx_list_clicked();

  void on_evidence_button_clicked();

  void on_evidence_delete_clicked();
  void on_evidence_x_clicked();

  void on_back_to_lobby_clicked();

  void on_char_select_left_clicked();
  void on_char_select_right_clicked();

  void on_spectator_clicked();

  void char_clicked(int n_char);

  void ping_server();
};

#endif // COURTROOM_H
