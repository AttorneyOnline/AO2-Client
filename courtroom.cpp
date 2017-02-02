#include "courtroom.h"

#include "aoapplication.h"
#include "lobby.h"
#include "hardware_functions.h"
#include "file_functions.h"
#include "datatypes.h"
#include "debug_functions.h"

#include <QDebug>
#include <QScrollBar>

Courtroom::Courtroom(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

  chat_tick_timer = new QTimer(this);

  text_delay = new QTimer(this);
  text_delay->setSingleShot(true);

  sfx_delay = new QTimer(this);
  sfx_delay->setSingleShot(true);

  char_button_mapper = new QSignalMapper(this);

  sfx_player = new QSoundEffect(this);

  ui_background = new AOImage(this, ao_app);

  ui_vp_background = new AOScene(this);
  ui_vp_player_char = new AOCharMovie(this, ao_app);
  ui_vp_desk = new AOScene(this);
  ui_vp_legacy_desk = new AOScene(this);
  ui_vp_chatbox = new AOImage(this, ao_app);
  ui_vp_showname = new QLabel(ui_vp_chatbox);
  ui_vp_message = new QPlainTextEdit(ui_vp_chatbox);
  ui_vp_testimony = new AOImage(this, ao_app);
  ui_vp_realization = new AOImage(this, ao_app);
  ui_vp_wtce = new AOMovie(this, ao_app);
  ui_vp_objection = new AOMovie(this, ao_app);

  ui_ic_chatlog = new QPlainTextEdit(this);
  ui_ic_chatlog->setReadOnly(true);

  ui_ms_chatlog = new QPlainTextEdit(this);
  ui_ms_chatlog->setReadOnly(true);
  ui_ms_chatlog->hide();

  ui_server_chatlog = new QPlainTextEdit(this);
  ui_server_chatlog->setReadOnly(true);

  ui_mute_list = new QListWidget(this);
  ui_area_list = new QListWidget(this);
  ui_music_list = new QListWidget(this);

  ui_ic_chat_message = new QLineEdit(this);
  ui_ic_chat_message->setFrame(false);

  ui_ooc_chat_message = new QLineEdit(this);
  ui_ooc_chat_message->setFrame(false);

  ui_ooc_chat_name = new QLineEdit(this);
  ui_ooc_chat_name->setFrame(false);
  ui_ooc_chat_name->setPlaceholderText("Name");

  ui_area_password = new QLineEdit(this);
  ui_area_password->setFrame(false);
  ui_music_search = new QLineEdit(this);
  ui_music_search->setFrame(false);

  //emote buttons

  ui_emote_left = new AOButton(this, ao_app);
  ui_emote_right = new AOButton(this, ao_app);

  ui_defense_bar = new AOImage(this, ao_app);
  ui_prosecution_bar = new  AOImage(this, ao_app);

  ui_music_label = new QLabel(this);
  ui_sfx_label = new QLabel(this);
  ui_blip_label = new QLabel(this);

  ui_hold_it = new AOButton(this, ao_app);
  ui_objection = new AOButton(this, ao_app);
  ui_take_that = new AOButton(this, ao_app);

  ui_ooc_toggle = new AOButton(this, ao_app);
  ui_witness_testimony = new AOButton(this, ao_app);
  ui_cross_examination = new AOButton(this, ao_app);

  ui_change_character = new AOButton(this, ao_app);
  ui_reload_theme = new AOButton(this, ao_app);
  ui_call_mod = new AOButton(this, ao_app);

  ui_pre = new QCheckBox(this);
  ui_flip = new QCheckBox(this);
  ui_guard = new QCheckBox(this);

  ui_custom_objection = new AOButton(this, ao_app);
  ui_realization = new AOButton(this, ao_app);
  ui_mute = new AOButton(this, ao_app);

  ui_defense_plus = new AOButton(this, ao_app);
  ui_defense_minus = new AOButton(this, ao_app);

  ui_prosecution_plus = new AOButton(this, ao_app);
  ui_prosecution_minus = new AOButton(this, ao_app);

  ui_text_color = new QComboBox(this);

  ui_music_slider = new QSlider(Qt::Horizontal, this);
  ui_music_slider->setRange(0, 100);
  ui_music_slider->setValue(50);

  ui_sfx_slider = new QSlider(Qt::Horizontal, this);
  ui_sfx_slider->setRange(0, 100);
  ui_sfx_slider->setValue(50);

  ui_blip_slider = new QSlider(Qt::Horizontal, this);
  ui_blip_slider->setRange(0, 100);
  ui_blip_slider->setValue(50);

  ui_muted = new AOImage(this, ao_app);

  /////////////char select widgets under here///////////////

  ui_char_select_background = new AOImage(this, ao_app);

  //constructing character button grid
  const int base_x_pos{25};
  const int base_y_pos{36};

  const int x_modifier{67};
  int x_mod_count{0};

  const int y_modifier{67};
  int y_mod_count{0};

  for (int n = 0 ; n < 90 ; ++n)
  {
    int x_pos = base_x_pos + (x_modifier * x_mod_count);
    int y_pos = base_y_pos + (y_modifier * y_mod_count);

    ui_char_button_list.append(new AOCharButton(ui_char_select_background, ao_app, x_pos, y_pos));

    connect(ui_char_button_list.at(n), SIGNAL(clicked()), char_button_mapper, SLOT(map())) ;
    char_button_mapper->setMapping (ui_char_button_list.at(n), n) ;

    ++x_mod_count;

    //if char number is divisible by ten with rest 9 then the next charicon should start on a new line
    if (n % 10 == 9 && n != 0)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }

  connect (char_button_mapper, SIGNAL(mapped(int)), this, SLOT(char_clicked(int))) ;

  ui_selector = new AOImage(ui_char_select_background, ao_app);
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->resize(62, 62);

  ui_back_to_lobby = new AOButton(ui_char_select_background, ao_app);

  ui_char_password = new QLineEdit(ui_char_select_background);

  ui_char_select_left = new AOButton(ui_char_select_background, ao_app);
  ui_char_select_right = new AOButton(ui_char_select_background, ao_app);

  ui_spectator = new AOButton(ui_char_select_background, ao_app);

  connect(ui_vp_objection, SIGNAL(done()), this, SLOT(objection_done()));

  connect(ui_ooc_chat_message, SIGNAL(returnPressed()), this, SLOT(on_ooc_return_pressed()));
  connect(ui_ooc_toggle, SIGNAL(clicked()), this, SLOT(on_ooc_toggle_clicked()));

  connect(ui_witness_testimony, SIGNAL(clicked()), this, SLOT(on_witness_testimony_clicked()));
  connect(ui_cross_examination, SIGNAL(clicked()), this, SLOT(on_cross_examination_clicked()));

  connect(ui_change_character, SIGNAL(clicked()), this, SLOT(on_change_character_clicked()));
  connect(ui_reload_theme, SIGNAL(clicked()), this, SLOT(on_reload_theme_clicked()));
  connect(ui_call_mod, SIGNAL(clicked()), this, SLOT(on_call_mod_clicked()));

  connect(ui_back_to_lobby, SIGNAL(clicked()), this, SLOT(on_back_to_lobby_clicked()));

  connect(ui_char_select_left, SIGNAL(clicked()), this, SLOT(on_char_select_left_clicked()));
  connect(ui_char_select_right, SIGNAL(clicked()), this, SLOT(on_char_select_right_clicked()));

  connect(ui_spectator, SIGNAL(clicked()), this, SLOT(on_spectator_clicked()));

  set_widgets();
}

void Courtroom::set_widgets()
{
  QString design_ini_path = ao_app->get_theme_path() + "courtroom_design.ini";
  QString default_ini_path = ao_app->get_base_path() + "themes/default/courtroom_design.ini";

  pos_size_type f_courtroom = ao_app->get_pos_and_size("courtroom", design_ini_path);
  pos_size_type f_viewport = ao_app->get_pos_and_size("viewport", design_ini_path);

  if (f_courtroom.width < 0 || f_courtroom.height < 0)
  {
    f_courtroom = ao_app->get_pos_and_size("courtroom", default_ini_path);
    if (f_courtroom.width < 0 || f_courtroom.height < 0)
    {
      qDebug() << "ERROR: did not find courtroom width or height in courtroom_design.ini!";
      //T0D0: add error box then quit application, this is not recoverable
    }
  }



  if (f_viewport.width < 0 || f_viewport.height < 0)
  {
    f_viewport = ao_app->get_pos_and_size("viewport", default_ini_path);
    if (f_viewport.width < 0 || f_viewport.height < 0)
    {
      qDebug() << "ERROR: did not find viewport width or height in courtroom_design.ini!";
      //T0D0: same, critical error
    }
  }

  m_courtroom_width = f_courtroom.width;
  m_courtroom_height = f_courtroom.height;

  m_viewport_x = f_viewport.x;
  m_viewport_y = f_viewport.y;
  m_viewport_width = f_viewport.width;
  m_viewport_height = f_viewport.height;

  this->setFixedSize(m_courtroom_width, m_courtroom_height);

  ui_background->set_image("courtroombackground.png");
  ui_background->move(0, 0);
  ui_background->resize(m_courtroom_width, m_courtroom_height);

  ui_vp_background->move(m_viewport_x, m_viewport_y);
  ui_vp_background->resize(m_viewport_width, m_viewport_height);

  ui_vp_player_char->move(m_viewport_x, m_viewport_y);
  ui_vp_player_char->combo_resize(m_viewport_width, m_viewport_height);

  ui_vp_desk->move(m_viewport_x, m_viewport_y);
  ui_vp_desk->resize(m_viewport_width, m_viewport_height);
  qDebug() << "resized ui_vp_desk to " << m_viewport_width << " and " << m_viewport_height;

  ui_vp_chatbox->move(m_viewport_x, m_viewport_y);
  ui_vp_chatbox->resize(m_viewport_width, m_viewport_height);

  ui_vp_showname->move(m_viewport_x, m_viewport_y);
  ui_vp_showname->resize(m_viewport_width, m_viewport_height);

  ui_vp_message->move(m_viewport_x, m_viewport_y);
  ui_vp_message->resize(m_viewport_width, m_viewport_height);

  //T0D0: resize it properly
  //D3BUG START
  //obscures some things
  ui_vp_message->hide();
  //D3BUG END

  ui_vp_testimony->move(m_viewport_x, m_viewport_y);
  ui_vp_testimony->resize(m_viewport_width, m_viewport_height);

  ui_vp_realization->move(m_viewport_x, m_viewport_y);
  ui_vp_realization->resize(m_viewport_width, m_viewport_height);

  ui_vp_wtce->move(m_viewport_x, m_viewport_y);
  ui_vp_wtce->combo_resize(m_viewport_width, m_viewport_height);

  ui_vp_objection->move(m_viewport_x, m_viewport_y);
  ui_vp_objection->combo_resize(m_viewport_width, m_viewport_height);

  set_size_and_pos(ui_ic_chatlog, "ic_chatlog");
  ui_ic_chatlog->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                               "color: white;");

  set_size_and_pos(ui_ms_chatlog, "ms_chatlog");
  ui_ms_chatlog->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_server_chatlog, "server_chatlog");
  ui_server_chatlog->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_mute_list, "mute_list");
  ui_mute_list->hide();

  set_size_and_pos(ui_area_list, "area_list");
  ui_area_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_music_list, "music_list");
  ui_area_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_ic_chat_message, "ic_chat_message");
  ui_ic_chat_message->setStyleSheet("background-color: rgba(89, 89, 89, 0);");

  set_size_and_pos(ui_ooc_chat_message, "ooc_chat_message");
  ui_ooc_chat_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_ooc_chat_name, "ooc_chat_name");
  ui_ooc_chat_name->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_area_password, "area_password");
  set_size_and_pos(ui_music_search, "music_search");

  //emote buttons

  set_size_and_pos(ui_emote_left, "emote_left");
  ui_emote_left->set_image("arrow_left.png");

  set_size_and_pos(ui_emote_right, "emote_right");
  ui_emote_right->set_image("arrow_right.png");

  set_size_and_pos(ui_defense_bar, "defense_bar");
  ui_defense_bar->set_image("defensebar10.png");

  set_size_and_pos(ui_prosecution_bar, "prosecution_bar");
  ui_prosecution_bar->set_image("prosecutionbar10.png");

  set_size_and_pos(ui_music_label, "music_label");
  ui_music_label->setText("Music");
  set_size_and_pos(ui_sfx_label, "sfx_label");
  ui_sfx_label->setText("Sfx");
  set_size_and_pos(ui_blip_label, "blip_label");
  ui_blip_label->setText("Blips");

  set_size_and_pos(ui_hold_it, "hold_it");
  ui_hold_it->set_image("holdit.png");
  set_size_and_pos(ui_objection, "objection");
  ui_objection->set_image("objection.png");
  set_size_and_pos(ui_take_that, "take_that");
  ui_take_that->set_image("takethat.png");

  set_size_and_pos(ui_ooc_toggle, "ooc_toggle");
  ui_ooc_toggle->setText("Server");

  set_size_and_pos(ui_witness_testimony, "witness_testimony");
  ui_witness_testimony->set_image("witnesstestimony.png");
  set_size_and_pos(ui_cross_examination, "cross_examination");
  ui_cross_examination->set_image("crossexamination.png");

  set_size_and_pos(ui_change_character, "change_character");
  ui_change_character->setText("Change character");

  set_size_and_pos(ui_reload_theme, "reload_theme");
  ui_reload_theme->setText("Reload theme");

  set_size_and_pos(ui_call_mod, "call_mod");
  ui_call_mod->setText("Call mod");

  set_size_and_pos(ui_pre, "pre");
  ui_pre->setText("Pre");

  set_size_and_pos(ui_flip, "flip");
  ui_flip->setText("Flip");

  set_size_and_pos(ui_guard, "guard");
  ui_guard->setText("Guard");

  set_size_and_pos(ui_custom_objection, "custom_objection");
  ui_custom_objection->set_image("custom.png");

  set_size_and_pos(ui_realization, "realization");
  ui_realization->set_image("realization.png");

  set_size_and_pos(ui_mute, "mute_button");
  ui_mute->set_image("mute.png");

  set_size_and_pos(ui_defense_plus, "defense_plus");
  ui_defense_plus->set_image("defplus.png");

  set_size_and_pos(ui_defense_minus, "defense_minus");
  ui_defense_minus->set_image("defminus.png");

  set_size_and_pos(ui_prosecution_plus, "prosecution_plus");
  ui_prosecution_plus->set_image("proplus.png");

  set_size_and_pos(ui_prosecution_minus, "prosecution_minus");
  ui_prosecution_minus->set_image("prominus.png");

  set_size_and_pos(ui_text_color, "text_color");

  set_size_and_pos(ui_music_slider, "music_slider");
  set_size_and_pos(ui_sfx_slider, "sfx_slider");
  set_size_and_pos(ui_blip_slider, "blip_slider");

  //T0D0: scale this according to ui_ic_chat_message
  //set_size_and_pos(ui_muted, "muted");

  //char select stuff under here

  ui_char_select_background->set_image("charselect_background.png");
  ui_char_select_background->move(0, 0);
  ui_char_select_background->resize(m_courtroom_width, m_courtroom_height);

  //buttons are in the constructor

  ui_selector->set_image("char_selector.png");
  ui_selector->hide();

  ui_back_to_lobby->setText("Back to Lobby");
  set_size_and_pos(ui_back_to_lobby, "back_to_lobby");

  set_size_and_pos(ui_char_password, "char_password");

  ui_char_select_left->set_image("arrow_left.png");
  ui_char_select_left->move(2, 325);
  ui_char_select_left->resize(20, 20);

  ui_char_select_right->set_image("arrow_right.png");
  ui_char_select_right->move(691, 325);
  ui_char_select_right->resize(20, 20);

  ui_spectator->setText("Spectator");
  set_size_and_pos(ui_spectator, "spectator");

}

void Courtroom::set_window_title(QString p_title)
{
  this->setWindowTitle(p_title);
}

void Courtroom::set_size_and_pos(QWidget *p_widget, QString p_identifier)
{
  QString design_ini_path = ao_app->get_theme_path() + "courtroom_design.ini";
  QString default_ini_path = ao_app->get_base_path() + "themes/default/courtroom_design.ini";

  pos_size_type design_ini_result = ao_app->get_pos_and_size(p_identifier, design_ini_path);

  if (design_ini_result.width < 0 || design_ini_result.height < 0)
  {
    design_ini_result = ao_app->get_pos_and_size(p_identifier, default_ini_path);

    if (design_ini_result.width < 0 || design_ini_result.height < 0)
    {
      //at this point it's pretty much game over
      call_error(" could not find \"" + p_identifier + "\" in courtroom_design.ini");
    }
  }

  p_widget->move(design_ini_result.x, design_ini_result.y);
  p_widget->resize(design_ini_result.width, design_ini_result.height);
}

void Courtroom::set_taken(int n_char, bool p_taken)
{
  if (n_char >= char_list.size())
  {
    qDebug() << "W: set_taken attempted to set an index bigger than char_list size";
    return;
  }

  char_type f_char;
  f_char.name = char_list.at(n_char).name;
  f_char.description = char_list.at(n_char).description;
  f_char.taken = p_taken;

  char_list.replace(n_char, f_char);
}

void Courtroom::set_char_select_page()
{
  ui_char_select_background->show();

  ui_char_select_left->hide();
  ui_char_select_right->hide();

  for (AOCharButton *i_button : ui_char_button_list)
    i_button->hide();

  int total_pages = char_list.size() / 90;
  int chars_on_page = 0;

  if (char_list.size() % 90 != 0)
  {
    ++total_pages;
    //i. e. not on the last page
    if (total_pages > current_char_page + 1)
      chars_on_page = 90;
    else
      chars_on_page = char_list.size() % 90;

  }
  else
    chars_on_page = 90;

  qDebug() << "chars_on_page: " << chars_on_page;

  if (total_pages > current_char_page + 1)
    ui_char_select_right->show();

  if (current_char_page > 0)
    ui_char_select_left->show();

  for (int n_button = 0 ; n_button < chars_on_page ; ++n_button)
  {
    int n_real_char = n_button + current_char_page * 90;
    AOCharButton *f_button = ui_char_button_list.at(n_button);

    f_button->reset();
    f_button->set_image(char_list.at(n_real_char).name);
    f_button->show();

    if (char_list.at(n_real_char).taken)
      f_button->set_taken();
  }

}

void Courtroom::set_background(QString p_background)
{
  current_background = p_background;
  QString bg_path = get_background_path();

  is_ao2_bg = file_exists(bg_path + "defensedesk.png") &&
              file_exists(bg_path + "prosecutiondesk.png") &&
              file_exists(bg_path + "stand.png");

  //T0D0: find some way to compensate for legacy resolution
}

void Courtroom::enter_courtroom(int p_cid)
{
  m_cid = p_cid;
  QString f_char = char_list.at(m_cid).name;

  //T0D0: set emote buttons

  QString side = ao_app->get_char_side(f_char);

  if (side == "jud")
  {
    ui_witness_testimony->show();
    ui_cross_examination->show();
    ui_defense_minus->show();
    ui_defense_plus->show();
    ui_prosecution_minus->show();
    ui_prosecution_plus->show();
  }
  else
  {
    ui_witness_testimony->hide();
    ui_cross_examination->hide();
    ui_defense_minus->hide();
    ui_defense_plus->hide();
    ui_prosecution_minus->hide();
    ui_prosecution_plus->hide();
  }

  ui_char_select_background->hide();
}

void Courtroom::append_ms_chatmessage(QString f_message)
{
  const QTextCursor old_cursor = ui_ms_chatlog->textCursor();
  const int old_scrollbar_value = ui_ms_chatlog->verticalScrollBar()->value();
  const bool is_scrolled_down = old_scrollbar_value == ui_ms_chatlog->verticalScrollBar()->maximum();

  ui_ms_chatlog->moveCursor(QTextCursor::End);

  ui_ms_chatlog->appendPlainText(f_message);

  if (old_cursor.hasSelection() || !is_scrolled_down)
  {
      // The user has selected text or scrolled away from the bottom: maintain position.
      ui_ms_chatlog->setTextCursor(old_cursor);
      ui_ms_chatlog->verticalScrollBar()->setValue(old_scrollbar_value);
  }
  else
  {
      // The user hasn't selected any text and the scrollbar is at the bottom: scroll to the bottom.
      ui_ms_chatlog->moveCursor(QTextCursor::End);
      ui_ms_chatlog->verticalScrollBar()->setValue(ui_ms_chatlog->verticalScrollBar()->maximum());
  }
}

void Courtroom::append_server_chatmessage(QString f_message)
{
  const QTextCursor old_cursor = ui_server_chatlog->textCursor();
  const int old_scrollbar_value = ui_server_chatlog->verticalScrollBar()->value();
  const bool is_scrolled_down = old_scrollbar_value == ui_server_chatlog->verticalScrollBar()->maximum();

  ui_server_chatlog->moveCursor(QTextCursor::End);

  ui_server_chatlog->appendPlainText(f_message);

  if (old_cursor.hasSelection() || !is_scrolled_down)
  {
      // The user has selected text or scrolled away from the bottom: maintain position.
      ui_server_chatlog->setTextCursor(old_cursor);
      ui_server_chatlog->verticalScrollBar()->setValue(old_scrollbar_value);
  }
  else
  {
      // The user hasn't selected any text and the scrollbar is at the bottom: scroll to the bottom.
      ui_server_chatlog->moveCursor(QTextCursor::End);
      ui_server_chatlog->verticalScrollBar()->setValue(ui_server_chatlog->verticalScrollBar()->maximum());
  }
}

void Courtroom::handle_chatmessage(QStringList *p_contents)
{
  text_state = 0;

  chatmessage_is_empty = m_chatmessage[MESSAGE] == " " || m_chatmessage[MESSAGE] == "";

  for (int n_string = 0 ; n_string < chatmessage_size ; ++n_string)
  {
    m_chatmessage[n_string] = p_contents->at(n_string);
  }

  QString f_message = m_chatmessage[CHAR_NAME] + ": " + m_chatmessage[MESSAGE] + '\n';

  const QTextCursor old_cursor = ui_ic_chatlog->textCursor();
  const int old_scrollbar_value = ui_ic_chatlog->verticalScrollBar()->value();
  const bool is_scrolled_up = old_scrollbar_value == ui_ic_chatlog->verticalScrollBar()->minimum();

  ui_ic_chatlog->moveCursor(QTextCursor::Start);

  ui_ic_chatlog->textCursor().insertText(f_message);

  if (old_cursor.hasSelection() || !is_scrolled_up)
  {
      // The user has selected text or scrolled away from the top: maintain position.
      ui_ic_chatlog->setTextCursor(old_cursor);
      ui_ic_chatlog->verticalScrollBar()->setValue(old_scrollbar_value);
  }
  else
  {
      // The user hasn't selected any text and the scrollbar is at the top: scroll to the top.
      ui_ic_chatlog->moveCursor(QTextCursor::Start);
      ui_ic_chatlog->verticalScrollBar()->setValue(ui_ic_chatlog->verticalScrollBar()->minimum());
  }

  int objection_mod = m_chatmessage[OBJECTION_MOD].toInt();

  //if an objection is used
  if (objection_mod <= 4 && objection_mod >= 1)
  {
    switch (objection_mod)
    {
    case 1:
      ui_vp_objection->play("holdit");
      break;
    case 2:
      ui_vp_objection->play("objection");
      break;
    case 3:
      ui_vp_objection->play("takethat");
      break;
    //case 4 is AO2 only
    case 4:
      ui_vp_objection->play("custom", m_chatmessage[CHAR_NAME]);
      break;
    default:
      qDebug() << "W: Logic error in objection switch statement!";
    }

    //means we are in a state of objecting
    anim_state = 0;

    int emote_mod = m_chatmessage[EMOTE_MOD].toInt();

    switch (emote_mod)
    {
    //we change the chatmessage from no preanim to preanim, see documentation
    case 0: case 2:
      m_chatmessage[EMOTE_MOD] = QString::number(++emote_mod);
      break;
    case 5:
      m_chatmessage[EMOTE_MOD] = QString::number(--emote_mod);
      break;
    }
  }
  else
    handle_chatmessage_2();
}

void Courtroom::objection_done()
{
  handle_chatmessage_2();
}

void Courtroom::handle_chatmessage_2()
{
  ui_vp_player_char->stop();

  QString remote_name = m_chatmessage[CHAR_NAME];
  QString local_showname = ao_app->get_showname(remote_name);

  //empty string means we couldnt find showname in char ini
  if (local_showname == "")
    ui_vp_showname->setText(remote_name);
  else
    ui_vp_showname->setText(local_showname);

  ui_vp_message->clear();
  ui_vp_chatbox->hide();

  set_scene();
  set_text_color();
}

void Courtroom::set_scene()
{
  //witness is default if pos is invalid
  QString f_image = "witnessempty.png";

  QString f_side = m_chatmessage[SIDE];

  if (f_side == "def")
    f_image = "defenseempty.png";
  else if (f_side == "pro")
    f_image = "prosecutorempty.png";
  else if (f_side == "jud")
    f_image = "judgestand.png";
  else if (f_side == "hld")
    f_image = "helperstand.png";
  else if (f_side == "hlp")
    f_image = "prohelperstand.png";

  ui_vp_background->set_image(f_image);

  //now for the hard part: desks
  int emote_mod = m_chatmessage[EMOTE_MOD].toInt();

  //desks show on emote modifier 0, 1 and 4, we're done with this function if theyre not any of those
  //we're also done if one of the non-desk positions is the current one(jud, hlp and hld)
  if (emote_mod == 2 ||
      emote_mod == 3 ||
      emote_mod == 5 ||
      f_side == "jud" ||
      f_side == "hlp" ||
      f_side == "hld")
  {
    ui_vp_desk->hide();
    ui_vp_legacy_desk->hide();
    return;
  }

  if (is_ao2_bg)
  {
    qDebug() << "found is_ao2_bg to be true";
    QString desk_image = "stand.png";

    if (f_side == "def")
      desk_image = "defensedesk.png";
    else if (f_side == "pro")
      desk_image = "prosecutiondesk.png";

    ui_vp_desk->set_image(desk_image);
    ui_vp_legacy_desk->hide();
    ui_vp_desk->show();
  }
  else if (f_side == "def" || f_side == "pro")
  {
    QString desk_image;

    if (f_side == "def")
      desk_image = "bancodefensa.png";
    else
      desk_image = "bancoacusacion.png";

    ui_vp_legacy_desk->set_legacy_desk(desk_image);
    ui_vp_desk->hide();
    ui_vp_legacy_desk->show();
  }
  //assume wit or invalid side
  else
  {
    ui_vp_desk->set_image("estrado.png");
    ui_vp_legacy_desk->hide();
    ui_vp_desk->show();
  }
}

void Courtroom::set_text_color()
{
  switch(m_chatmessage[TEXT_COLOR].toInt())
  {
  case 0:
    ui_vp_message->setStyleSheet("QPlainTextEdit{color: white;}");
    break;
  case 1:
    ui_vp_message->setStyleSheet("QPlainTextEdit{color: rgb(0, 255, 0);}");
    break;
  case 2:
    ui_vp_message->setStyleSheet("QPlainTextEdit{color: red;}");
    break;
  case 3:
    ui_vp_message->setStyleSheet("QPlainTextEdit{color: orange;}");
    break;
  case 4:
    ui_vp_message->setStyleSheet("QPlainTextEdit{color: rgb(45, 150, 255);}");
    break;
  case 5:
    ui_vp_message->setStyleSheet("QPlainTextEdit{color: yellow;}");
    break;
  default:
    ui_vp_message->setStyleSheet("QPlainTextEdit{color: white;}");
  }
}

void Courtroom::handle_wtce(QString p_wtce)
{
  //witness testimony
  if (p_wtce == "testimony1")
  {
    QString wt_path = ao_app->get_sounds_path() + "sfx-testimony2.wav";
    QUrl wt_sfx(QUrl::fromLocalFile(wt_path));

    sfx_player->stop();
    sfx_player->setSource(wt_sfx);

    sfx_player->play();
    ui_vp_wtce->play("witnesstestimony.gif");
  }
  //cross examination
  else if (p_wtce == "testimony2")
  {
    QString ce_path = ao_app->get_sounds_path() + "sfx-testimony.wav";
    QUrl ce_sfx(QUrl::fromLocalFile(ce_path));

    sfx_player->stop();
    sfx_player->setSource(ce_sfx);

    sfx_player->play();
    ui_vp_wtce->play("crossexamination.gif");
  }
}

void Courtroom::on_ooc_return_pressed()
{
  if (ui_ooc_chat_message->text() == "" || ui_ooc_chat_name->text() == "")
    return;

  QStringList packet_contents;
  packet_contents.append(ui_ooc_chat_name->text());
  packet_contents.append(ui_ooc_chat_message->text());

  AOPacket *f_packet = new AOPacket("CT", packet_contents);

  if (server_ooc)
    ao_app->send_server_packet(f_packet);
  else
    ao_app->send_ms_packet(f_packet);

  ui_ooc_chat_message->clear();
}

void Courtroom::on_ooc_toggle_clicked()
{
  if (server_ooc)
  {
    ui_ms_chatlog->show();
    ui_server_chatlog->hide();
    ui_ooc_toggle->setText("Master");

    server_ooc = false;
  }
  else
  {
    ui_ms_chatlog->hide();
    ui_server_chatlog->show();
    ui_ooc_toggle->setText("Server");

    server_ooc = true;
  }
}

void Courtroom::on_witness_testimony_clicked()
{
  ao_app->send_server_packet(new AOPacket("RT#testimony1#%"));
}

void Courtroom::on_cross_examination_clicked()
{
  ao_app->send_server_packet(new AOPacket("RT#testimony2#%"));
}

void Courtroom::on_change_character_clicked()
{
  ui_char_select_background->show();
}

void Courtroom::on_reload_theme_clicked()
{
  ao_app->set_user_theme();

  set_widgets();
  //to update status on the background
  set_background(current_background);
}

void Courtroom::on_back_to_lobby_clicked()
{
  ao_app->construct_lobby();
  ao_app->w_lobby->list_servers();
  ao_app->destruct_courtroom();
}

void Courtroom::on_char_select_left_clicked()
{
  --current_char_page;
  set_char_select_page();
}

void Courtroom::on_char_select_right_clicked()
{
  ++current_char_page;
  set_char_select_page();
}

void Courtroom::on_spectator_clicked()
{
  ui_char_select_background->hide();
}

void Courtroom::on_call_mod_clicked()
{
  ao_app->send_server_packet(new AOPacket("ZZ#%"));
}

void Courtroom::char_clicked(int n_char)
{
  int n_real_char = n_char + current_char_page * 90;

  QString char_ini_path = ao_app->get_character_path(char_list.at(n_real_char).name) + "char.ini";

  if (!file_exists(char_ini_path))
  {
    qDebug() << "did not find " << char_ini_path;
    //T0D0: call error
    return;
  }

  ao_app->send_server_packet(new AOPacket("CC#" + QString::number(ao_app->s_pv) + "#" + QString::number(n_real_char) + "#" + get_hdid() + "#%"));
}

Courtroom::~Courtroom()
{

}
