#ifndef COURTROOM_H
#define COURTROOM_H

#include "aoimage.h"
#include "aobutton.h"
#include "aocharbutton.h"
#include "aopacket.h"
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
  void set_window_title(QString p_title);
  void set_size_and_pos(QWidget *p_widget, QString p_identifier);
  void set_taken(int n_char, bool p_taken);
  void set_char_select_page();

  void enter_courtroom(int p_cid);

  void append_ms_chatmessage(QString f_message);
  void append_server_chatmessage(QString f_message);

  void handle_chatmessage(QStringList *p_contents);

  ~Courtroom();

private:
  AOApplication *ao_app;

  const int m_courtroom_width = 714;
  const int m_courtroom_height = 668;

  const int m_viewport_x = 0;
  const int m_viewport_y = 0;

  const int m_viewport_width = 256;
  const int m_viewport_height = 192;

  QVector<char_type> char_list;
  QVector<evi_type> evidence_list;
  QVector<QString> music_list;

  QSignalMapper *char_button_mapper;

  //0 is the first page, 1 second etc.
  //makes char arithmetic easier
  int current_char_page = 0;

  //character id, which index of the char_list the player is
  int m_cid = 0;

  //wether the ooc chat is server or master chat, true is server
  bool server_ooc = true;

  AOImage *ui_background;

  //T0D0: add viewport elements like background, desk, etc.

  QPlainTextEdit *ui_ic_chatlog;

  QPlainTextEdit *ui_ms_chatlog;
  QPlainTextEdit *ui_server_chatlog;


  QListWidget *ui_mute_list;
  QListWidget *ui_area_list;
  QListWidget *ui_music_list;

  QLineEdit *ui_ic_chat_message;

  QLineEdit *ui_ooc_chat_message;
  QLineEdit *ui_ooc_chat_name;

  QLineEdit *ui_area_password;
  QLineEdit *ui_music_search;

  //T0D0: add emote buttons

  AOButton *ui_emote_left;
  AOButton *ui_emote_right;

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
\
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

  //char select stuff under here

  AOImage *ui_char_select_background;

  QVector<AOCharButton*> ui_char_button_list;
  AOImage *ui_selector;

  AOButton *ui_back_to_lobby;

  QLineEdit *ui_char_password;

  AOButton *ui_char_select_left;
  AOButton *ui_char_select_right;

  AOButton *ui_spectator;
private slots:
  void on_ooc_return_pressed();
  void on_ooc_toggle_clicked();

  void on_change_character_clicked();
  void on_reload_theme_clicked();
  void on_call_mod_clicked();

  void on_back_to_lobby_clicked();

  void on_char_select_left_clicked();
  void on_char_select_right_clicked();

  void on_spectator_clicked();

  void char_clicked(int n_char);

};

#endif // COURTROOM_H
