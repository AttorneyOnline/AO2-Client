#ifndef COURTROOM_H
#define COURTROOM_H

#include "aoimage.h"
#include "aobutton.h"
#include "aocharbutton.h"
#include "aopacket.h"

#include <QMainWindow>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QVector>

class AOApplication;

class Courtroom : public QMainWindow
{
  Q_OBJECT
public:
  explicit Courtroom(AOApplication *parent = 0);
  void set_widgets();
  ~Courtroom();

private:
  AOApplication *ao_app;

  const int m_courtroom_width = 714;
  const int m_courtroom_height = 668;

  const int m_viewport_x = 0;
  const int m_viewport_y = 0;

  const int m_viewport_width = 256;
  const int m_viewport_height = 192;

  AOImage *ui_background;
  //viewport elements like background, desk, etc.

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

  //emote buttons

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

  AOButton *ui_spectator;

};

#endif // COURTROOM_H
