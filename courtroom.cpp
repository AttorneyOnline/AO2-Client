#include "courtroom.h"

#include "aoapplication.h"

Courtroom::Courtroom(AOApplication *parent) : QMainWindow()
{
  ao_app = parent;

  ui_background = new AOImage(this);

  //viewport elements like background, desk, etc.

  ui_ic_chatlog = new QPlainTextEdit(this);

  ui_ms_chatlog = new QPlainTextEdit(this);
  ui_server_chatlog = new QPlainTextEdit(this);


  ui_mute_list = new QListWidget(this);
  ui_area_list = new QListWidget(this);
  ui_music_list = new QListWidget(this);

  ui_ic_chat_message = new QLineEdit(this);

  ui_ooc_chat_message = new QLineEdit(this);
  ui_ooc_chat_name = new QLineEdit(this);

  ui_area_password = new QLineEdit(this);
  ui_music_search = new QLineEdit(this);

  //emote buttons

  ui_emote_left = new AOButton(this);
  ui_emote_right = new AOButton(this);

  ui_defense_bar = new AOImage(this);
  ui_prosecution_bar = new  AOImage(this);

  ui_music_label = new QLabel(this);
  ui_sfx_label = new QLabel(this);
  ui_blip_label = new QLabel(this);

  ui_hold_it = new AOButton(this);
  ui_objection = new AOButton(this);
  ui_take_that = new AOButton(this);

  ui_ooc_toggle = new AOButton(this);
  ui_witness_testimony = new AOButton(this);
  ui_cross_examination = new AOButton(this);

  ui_change_character = new AOButton(this);
  ui_reload_theme = new AOButton(this);
  ui_call_mod = new AOButton(this);

  ui_pre = new QCheckBox(this);
  ui_flip = new QCheckBox(this);
  ui_guard = new QCheckBox(this);

  ui_custom_objection = new AOButton(this);
  ui_realization = new AOButton(this);
  ui_mute = new AOButton(this);

  ui_defense_plus = new AOButton(this);
  ui_defense_minus = new AOButton(this);

  ui_prosecution_plus = new AOButton(this);
  ui_prosecution_minus = new AOButton(this);

  ui_text_color = new QComboBox(this);

  ui_music_slider = new QSlider(this);
  ui_sfx_slider = new QSlider(this);
  ui_blip_slider = new QSlider(this);

  ui_muted = new AOImage(this);

  /////////////char select widgets under here///////////////

  ui_char_select_background = new AOImage(this);

  //T0D0: allocate and position charbuttons
  //QVector<AOCharButton*> ui_char_button_list;

  ui_selector = new AOImage(ui_char_select_background);

  ui_back_to_lobby = new AOButton(ui_char_select_background);

  ui_char_password = new QLineEdit(ui_char_select_background);

  ui_spectator = new AOButton(ui_char_select_background);

  set_widgets();
}

void Courtroom::set_widgets()
{
  this->setFixedSize(m_courtroom_width, m_courtroom_height);

  ui_background->set_image("courtroombackground.png");
  ui_background->move(0, 0);
  ui_background->resize(m_courtroom_width, m_courtroom_height);

  //viewport elements like background, desk, etc.

  ui_ic_chatlog->move(231, 319);
  ui_ic_chatlog->resize(260, 0);

  ui_ms_chatlog->move(490, 1);
  ui_ms_chatlog->move(224, 277);

  ui_server_chatlog->move(490, 1);
  ui_server_chatlog->resize(224, 277);



  ui_mute_list->move(260, 160);
  ui_mute_list->resize(231, 159);

  ui_area_list->move(266, 494);
  ui_area_list->resize();

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
}

Courtroom::~Courtroom()
{
  delete ui_background;

  //T0D0: deallocate viewport elements like background, desk, etc.

  delete ui_ic_chatlog;

  delete ui_server_chatlog;
  delete ui_ms_chatlog;

  delete ui_mute_list;
  delete ui_area_list;
  delete ui_music_list;

  delete ui_ic_chat_message;

  delete ui_ooc_chat_message;
  delete ui_ooc_chat_name;

  delete ui_area_password;
  delete ui_music_search;

  //T0D0: deallocate emote buttons

  delete ui_emote_left;
  delete ui_emote_right;

  delete ui_defense_bar;
  delete ui_prosecution_bar;

  delete ui_music_label;
  delete ui_sfx_label;
  delete ui_blip_label;

  delete ui_hold_it;
  delete ui_objection;
  delete ui_take_that;

  delete ui_ooc_toggle;
  delete ui_witness_testimony;
  delete ui_cross_examination;

  delete ui_change_character;
  delete ui_reload_theme;
  delete ui_call_mod;

  delete ui_pre;
  delete ui_flip;
  delete ui_guard;
\
  delete ui_custom_objection;
  delete ui_realization;
  delete ui_mute;

  delete ui_defense_plus;
  delete ui_defense_minus;

  delete ui_prosecution_plus;
  delete ui_prosecution_minus;

  delete ui_text_color;

  delete ui_music_slider;
  delete ui_sfx_slider;
  delete ui_blip_slider;

  delete ui_muted;

  delete ui_char_select_background;

  //T0D0: deallocate char buttons
  //QVector<AOCharButton*> ui_char_button_list;

  delete ui_selector;

  delete ui_back_to_lobby;

  delete ui_char_password;

  delete ui_spectator;
}
