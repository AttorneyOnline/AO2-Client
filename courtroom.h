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
#include "aomusicplayer.h"
#include "aosfxplayer.h"
#include "aoblipplayer.h"
#include "aoevidencebutton.h"
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

class AOApplication;

class Courtroom : public QMainWindow
{
  Q_OBJECT
public:
  explicit Courtroom(AOApplication *p_ao_app);

  void append_char(char_type p_char){char_list.append(p_char);}
  void append_evidence(evi_type p_evi){evidence_list.append(p_evi);}
  void append_music(QString f_music){music_list.append(f_music);}

  void set_widgets();
  void set_font(QWidget *widget, int weight);
  void set_fonts();
  void set_window_title(QString p_title);
  void set_size_and_pos(QWidget *p_widget, QString p_identifier);
  void set_taken(int n_char, bool p_taken);
  void set_background(QString p_background);



  void done_received();

  void set_mute_list();

  //sets desk and bg based on pos in chatmessage
  void set_scene();

  //sets text color based on text color in chatmessage
  void set_text_color();

  void set_ip_list(QString p_list);

  void set_mute(bool p_muted, int p_cid);
  void set_ban(int p_cid);

  //implementations in path_functions.cpp
  QString get_background_path();
  QString get_default_background_path();

  int get_cid() {return m_cid;}
  QString get_current_char() {return current_char;}

  void enter_courtroom(int p_cid);
  void list_music();

  void append_ms_chatmessage(QString f_message);
  void append_server_chatmessage(QString f_name, QString f_message);

  void handle_chatmessage(QStringList *p_contents);
  void handle_chatmessage_2();
  void handle_chatmessage_3();

  void append_ic_text(QString p_text);

  void handle_song(QStringList *p_contents);

  void play_preanim();

  void handle_wtce(QString p_wtce);
  void set_hp_bar(int p_bar, int p_state);

  void check_connection_received();

  ~Courtroom();

private:
  AOApplication *ao_app;

  int m_courtroom_width = 714;
  int m_courtroom_height = 668;

  int m_viewport_x = 0;
  int m_viewport_y = 0;

  int m_viewport_width = 256;
  int m_viewport_height = 192;

  QVector<char_type> char_list;
  QVector<evi_type> evidence_list;
  QVector<QString> music_list;

  QSignalMapper *char_button_mapper;

  //triggers ping_server() every 60 seconds
  QTimer *keepalive_timer;

  //how long we wait for the server to respond on a ping
  QTimer *disconnect_timer;

  //determines how fast messages tick onto screen
  QTimer *chat_tick_timer;
  int chat_tick_interval = 60;
  //which tick position(character in chat message) we are at
  int tick_pos = 0;
  //used to determine how often blips sound
  int blip_pos = 0;
  int blip_rate = 1;
  int rainbow_counter = 0;
  bool blank_blip = false;

  //delay before chat messages starts ticking
  QTimer *text_delay_timer;

  //delay before sfx plays
  QTimer *sfx_delay_timer;

  QTimer *realization_timer;

  QTimer *testimony_show_timer;
  QTimer *testimony_hide_timer;

  //every time point in char.inis times this equals the final time
  const int time_mod = 40;

  static const int chatmessage_size = 15;
  QString m_chatmessage[chatmessage_size];
  bool chatmessage_is_empty = false;

  QString previous_ic_message = "";

  bool testimony_in_progress = false;

  const int testimony_show_time = 1500;
  const int testimony_hide_time = 500;

  QMap<QString, bool> mute_map;

  bool is_muted = false;

  //state of animation, 0 = objecting, 1 = preanim, 2 = talking, 3 = idle
  int anim_state = 3;

  //state of text ticking, 0 = not yet ticking, 1 = ticking in progress, 2 = ticking done
  int text_state = 2;

  //character id, which index of the char_list the player is
  int m_cid = -1;
  //cid and this may differ in cases of ini-editing
  QString current_char = "";

  int objection_state = 0;
  int realization_state = 0;
  int text_color = 0;

  int defense_bar_state = 0;
  int prosecution_bar_state = 0;

  int current_char_page = 0;
  int char_columns = 10;
  int char_rows = 9;
  int max_chars_on_page = 90;

  int current_emote_page = 0;
  int current_emote = 0;
  int emote_columns = 5;
  int emote_rows = 2;
  int max_emotes_on_page = 10;

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

  AOMusicPlayer *music_player;
  AOSfxPlayer *sfx_player;
  AOSfxPlayer *objection_player;
  AOBlipPlayer *blip_player;

  AOSfxPlayer *modcall_player;

  AOImage *ui_background;

  QWidget *ui_viewport;
  AOScene *ui_vp_background;
  AOMovie *ui_vp_speedlines;
  AOCharMovie *ui_vp_player_char;
  AOScene *ui_vp_desk;
  AOScene *ui_vp_legacy_desk;
  AOImage *ui_vp_chatbox;
  QLabel *ui_vp_showname;
  QTextEdit *ui_vp_message;
  AOImage *ui_vp_testimony;
  AOImage *ui_vp_realization;
  AOMovie *ui_vp_wtce;
  AOMovie *ui_vp_objection;

  QPlainTextEdit *ui_ic_chatlog;

  QTextBrowser *ui_ms_chatlog;
  QTextBrowser *ui_server_chatlog;

  QListWidget *ui_mute_list;
  QListWidget *ui_area_list;
  QListWidget *ui_music_list;

  QLineEdit *ui_ic_chat_message;

  QLineEdit *ui_ooc_chat_message;
  QLineEdit *ui_ooc_chat_name;

  //QLineEdit *ui_area_password;
  QLineEdit *ui_music_search;

  QWidget *ui_emotes;
  QVector<AOEmoteButton*> ui_emote_list;
  AOButton *ui_emote_left;
  AOButton *ui_emote_right;

  QComboBox *ui_emote_dropdown;

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

  AOButton *ui_change_character;
  AOButton *ui_reload_theme;
  AOButton *ui_call_mod;

  QCheckBox *ui_pre;
  QCheckBox *ui_flip;
  QCheckBox *ui_guard;

  AOButton *ui_custom_objection;
  AOButton *ui_realization;
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
  QLabel *ui_evidence_name;
  QVector<AOEvidenceButton*> ui_evidence_list;
  AOImage *ui_evidence_left;
  AOImage *ui_evidence_right;
  AOImage *ui_evidence_overlay;
  AOButton *ui_evidence_x;
  QPlainTextEdit *ui_evidence_description;

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

  void select_emote(int p_id);

  void on_emote_clicked(int p_id);

  void on_emote_left_clicked();
  void on_emote_right_clicked();

  void on_emote_dropdown_changed(int p_index);

  void on_evidence_clicked(int p_id);

  void on_evidence_hover(int p_id, bool p_state);

  void on_evidence_left_clicked();
  void on_evidence_right_clicked();

  void on_hold_it_clicked();
  void on_objection_clicked();
  void on_take_that_clicked();
  void on_custom_objection_clicked();

  void on_realization_clicked();

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

  void on_pre_clicked();
  void on_flip_clicked();
  void on_guard_clicked();

  void on_evidence_button_clicked();

  void on_evidence_x_clicked();

  void on_back_to_lobby_clicked();

  void on_char_select_left_clicked();
  void on_char_select_right_clicked();

  void on_spectator_clicked();

  void char_clicked(int n_char);

  void ping_server();
  void connection_timeout();

};

#endif // COURTROOM_H
