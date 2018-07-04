#include "courtroom.h"

#include "aoapplication.h"
#include "lobby.h"
#include "hardware_functions.h"
#include "file_functions.h"
#include "datatypes.h"
#include "debug_functions.h"

#include <QDebug>
#include <QScrollBar>
#include <QRegExp>
#include <QBrush>
#include <QTextCharFormat>
#include <QFont>
#include <QTime>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

Courtroom::Courtroom(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

  //initializing sound device
  BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, 0, NULL);
  BASS_PluginLoad("bassopus.dll", BASS_UNICODE);

  keepalive_timer = new QTimer(this);
  keepalive_timer->start(60000);

  chat_tick_timer = new QTimer(this);

  text_delay_timer = new QTimer(this);
  text_delay_timer->setSingleShot(true);

  sfx_delay_timer = new QTimer(this);
  sfx_delay_timer->setSingleShot(true);

  realization_timer = new QTimer(this);
  realization_timer->setSingleShot(true);

  testimony_show_timer = new QTimer(this);
  testimony_show_timer->setSingleShot(true);

  testimony_hide_timer = new QTimer(this);
  testimony_hide_timer->setSingleShot(true);

  char_button_mapper = new QSignalMapper(this);

  m_blip_player = new AOBlipPlayer(this, ao_app);
  m_blip_player->set_volume(0);
  m_sfx_player = new AOSfxPlayer(this, ao_app);
  m_sfx_player->set_volume(0);
  m_shout_player = new AOShoutPlayer(this, ao_app);
  m_shout_player->set_volume(0);
  m_mod_player = new AOSfxPlayer(this, ao_app);
  m_mod_player->set_volume(50);
  m_music_player = new AOMusicPlayer(this, ao_app);
  m_music_player->set_volume(0);

  ui_background = new AOImage(this, ao_app);

  ui_viewport = new QWidget(this);
  ui_vp_background = new AOScene(ui_viewport, ao_app);
  ui_vp_speedlines = new AOMovie(ui_viewport, ao_app);
  ui_vp_speedlines->set_play_once(false);
  ui_vp_player_char = new AOCharMovie(ui_viewport, ao_app);
  ui_vp_desk = new AOScene(ui_viewport, ao_app);
  ui_vp_legacy_desk = new AOScene(ui_viewport, ao_app);

  ui_vp_music_display_a = new AOImage(this, ao_app);
  ui_vp_music_display_b = new AOImage(this, ao_app);
  ui_vp_music_area = new QWidget(ui_vp_music_display_a);
  ui_vp_music_name = new QTextEdit(ui_vp_music_area);
  ui_vp_music_name->setText("Danganronpa Online");
  ui_vp_music_name->setFrameStyle(QFrame::NoFrame);
  ui_vp_music_name->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_music_name->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_music_name->setReadOnly(true);
  music_anim = new QPropertyAnimation(ui_vp_music_name, "geometry");

  ui_vp_evidence_display = new AOEvidenceDisplay(this, ao_app);

  ui_vp_chatbox = new AOImage(this, ao_app);
  ui_vp_showname = new QLabel(ui_vp_chatbox);
  ui_vp_message = new QTextEdit(ui_vp_chatbox);
  ui_vp_message->setFrameStyle(QFrame::NoFrame);
  ui_vp_message->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setReadOnly(true);

  ui_vp_testimony = new AOImage(this, ao_app);
  ui_vp_effect = new AOMovie(this, ao_app);
  ui_vp_wtce = new AOMovie(this, ao_app);
  ui_vp_objection = new AOMovie(this, ao_app);

  ui_ic_chatlog = new QTextEdit(this);
  ui_ic_chatlog->setReadOnly(true);

  ui_ms_chatlog = new AOTextArea(this);
  ui_ms_chatlog->setReadOnly(true);
  ui_ms_chatlog->setOpenExternalLinks(true);
  ui_ms_chatlog->hide();

  ui_server_chatlog = new AOTextArea(this);
  ui_server_chatlog->setReadOnly(true);
  ui_server_chatlog->setOpenExternalLinks(true);

  ui_mute_list = new QListWidget(this);
  //ui_area_list = new QListWidget(this);
  ui_music_list = new QListWidget(this);
  ui_sfx_list = new QListWidget(this);

  ui_ic_chat_message = new QLineEdit(this);
  ui_ic_chat_message->setFrame(false);

  ui_muted = new AOImage(ui_ic_chat_message, ao_app);
  ui_muted->hide();

  ui_ooc_chat_message = new QLineEdit(this);
  ui_ooc_chat_message->setFrame(false);

  ui_ooc_chat_name = new QLineEdit(this);
  ui_ooc_chat_name->setFrame(false);
  ui_ooc_chat_name->setPlaceholderText("Name");

  //ui_area_password = new QLineEdit(this);
  //ui_area_password->setFrame(false);
  ui_music_search = new QLineEdit(this);
  ui_music_search->setFrame(false);

  ui_sfx_search = new QLineEdit(this);
  ui_sfx_search->setFrame(false);

  ui_note_area = new AONoteArea(this, ao_app);
  ui_note_area->add_button = new AOButton(ui_note_area, ao_app);
  ui_note_area->m_layout = new QVBoxLayout(ui_note_area);
  note_scroll_area = new QScrollArea(this);

  note_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  note_scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  note_scroll_area->setWidgetResizable(false);

  ui_set_notes = new AOButton(this, ao_app);

  construct_emotes();

  ui_emote_left = new AOButton(this, ao_app);
  ui_emote_right = new AOButton(this, ao_app);

  ui_emote_dropdown = new QComboBox(this);
  ui_pos_dropdown = new QComboBox(this);
  ui_pos_dropdown->addItem("wit");
  ui_pos_dropdown->addItem("def");
  ui_pos_dropdown->addItem("pro");
  ui_pos_dropdown->addItem("jud");
  ui_pos_dropdown->addItem("hld");
  ui_pos_dropdown->addItem("hlp");

  ui_defense_bar = new AOImage(this, ao_app);
  ui_prosecution_bar = new  AOImage(this, ao_app);

  ui_music_label = new QLabel(this);
  ui_sfx_label = new QLabel(this);
  ui_blip_label = new QLabel(this);

  shouts_enabled.resize(7);
  ui_shouts.resize(7); // 7 is the number of shouts

  for(int i = 0; i < ui_shouts.size(); ++i)
  {
    ui_shouts[i] = new AOButton(this, ao_app);
    ui_shouts[i]->setProperty("shout_id", i+1);
  }

  ui_shout_up = new AOButton(this, ao_app);
  ui_shout_up->setProperty("cycle_id", 1);
  ui_shout_down = new AOButton(this, ao_app);
  ui_shout_down->setProperty("cycle_id", 0);

  ui_ooc_toggle = new AOButton(this, ao_app);
  ui_witness_testimony = new AOButton(this, ao_app);
  ui_cross_examination = new AOButton(this, ao_app);

  ui_change_character = new AOButton(this, ao_app);
  ui_reload_theme = new AOButton(this, ao_app);
  ui_call_mod = new AOButton(this, ao_app);

  ui_theme_list = new QComboBox(this);
  ui_confirm_theme = new AOButton(this, ao_app);
  ui_note_button = new AOButton(this, ao_app);

  ui_pre = new QCheckBox(this);
  ui_pre->setText("Pre");
  ui_flip = new QCheckBox(this);
  ui_flip->setText("Flip");
  ui_flip->hide();
  ui_guard = new QCheckBox(this);
  ui_guard->setText("Guard");
  ui_guard->hide();

  ui_effect_flash = new AOButton(this, ao_app);
  ui_effect_flash->setProperty("effect_id", 1);
  ui_effect_gloom = new AOButton(this, ao_app);
  ui_effect_gloom->setProperty("effect_id", 2);
  ui_mute = new AOButton(this, ao_app);

  ui_defense_plus = new AOButton(this, ao_app);
  ui_defense_minus = new AOButton(this, ao_app);

  ui_prosecution_plus = new AOButton(this, ao_app);
  ui_prosecution_minus = new AOButton(this, ao_app);

  ui_text_color = new QComboBox(this);
  ui_text_color->addItem("White");
  ui_text_color->addItem("Green");
  ui_text_color->addItem("Red");
  ui_text_color->addItem("Orange");
  ui_text_color->addItem("Blue");
  if (ao_app->yellow_text_enabled)
    ui_text_color->addItem("Yellow");

  ui_music_slider = new QSlider(Qt::Horizontal, this);
  ui_music_slider->setRange(0, 100);
  ui_music_slider->setValue(ao_app->get_default_music());

  ui_sfx_slider = new QSlider(Qt::Horizontal, this);
  ui_sfx_slider->setRange(0, 100);
  ui_sfx_slider->setValue(ao_app->get_default_sfx());

  ui_blip_slider = new QSlider(Qt::Horizontal, this);
  ui_blip_slider->setRange(0, 100);
  ui_blip_slider->setValue(ao_app->get_default_blip());

  ui_evidence_button = new AOButton(this, ao_app);

  ui_vp_notepad_image = new AOImage(this, ao_app);
  ui_vp_notepad = new QTextEdit(this);
  ui_vp_notepad->setFrameStyle(QFrame::NoFrame);

  construct_evidence();

  construct_char_select();

  connect(keepalive_timer, SIGNAL(timeout()), this, SLOT(ping_server()));

  connect(ui_vp_objection, SIGNAL(done()), this, SLOT(objection_done()));
  connect(ui_vp_player_char, SIGNAL(done()), this, SLOT(preanim_done()));

  connect(text_delay_timer, SIGNAL(timeout()), this, SLOT(start_chat_ticking()));
  connect(sfx_delay_timer, SIGNAL(timeout()), this, SLOT(play_sfx()));

  connect(chat_tick_timer, SIGNAL(timeout()), this, SLOT(chat_tick()));

  connect(realization_timer, SIGNAL(timeout()), this, SLOT(realization_done()));

  connect(testimony_show_timer, SIGNAL(timeout()), this, SLOT(hide_testimony()));
  connect(testimony_hide_timer, SIGNAL(timeout()), this, SLOT(show_testimony()));

  connect(ui_emote_left, SIGNAL(clicked()), this, SLOT(on_emote_left_clicked()));
  connect(ui_emote_right, SIGNAL(clicked()), this, SLOT(on_emote_right_clicked()));

  connect(ui_emote_dropdown, SIGNAL(activated(int)), this, SLOT(on_emote_dropdown_changed(int)));
  connect(ui_pos_dropdown, SIGNAL(activated(int)), this, SLOT(on_pos_dropdown_changed(int)));

  connect(ui_mute_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_mute_list_clicked(QModelIndex)));

  connect(ui_ic_chat_message, SIGNAL(returnPressed()), this, SLOT(on_chat_return_pressed()));

  connect(ui_ooc_chat_message, SIGNAL(returnPressed()), this, SLOT(on_ooc_return_pressed()));

  connect(ui_music_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_music_list_double_clicked(QModelIndex)));

  for(int i = 0; i < 7; ++i)
      connect(ui_shouts[i], SIGNAL(clicked(bool)), this, SLOT(on_shout_clicked()));

  connect(ui_shout_up, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));
  connect(ui_shout_down, SIGNAL(clicked(bool)), this, SLOT(on_cycle_clicked()));

  connect(ui_effect_flash, SIGNAL(clicked()), this, SLOT(on_effect_button_clicked()));
  connect(ui_effect_gloom, SIGNAL(clicked()), this, SLOT(on_effect_button_clicked()));

  connect(ui_mute, SIGNAL(clicked()), this, SLOT(on_mute_clicked()));

  connect(ui_defense_minus, SIGNAL(clicked()), this, SLOT(on_defense_minus_clicked()));
  connect(ui_defense_plus, SIGNAL(clicked()), this, SLOT(on_defense_plus_clicked()));
  connect(ui_prosecution_minus, SIGNAL(clicked()), this, SLOT(on_prosecution_minus_clicked()));
  connect(ui_prosecution_plus, SIGNAL(clicked()), this, SLOT(on_prosecution_plus_clicked()));

  connect(ui_text_color, SIGNAL(currentIndexChanged(int)), this, SLOT(on_text_color_changed(int)));

  connect(ui_music_slider, SIGNAL(valueChanged(int)), this, SLOT(on_music_slider_moved(int)));
  connect(ui_sfx_slider, SIGNAL(valueChanged(int)), this, SLOT(on_sfx_slider_moved(int)));
  connect(ui_blip_slider, SIGNAL(valueChanged(int)), this, SLOT(on_blip_slider_moved(int)));

  connect(ui_ooc_toggle, SIGNAL(clicked()), this, SLOT(on_ooc_toggle_clicked()));

  connect(ui_music_search, SIGNAL(textChanged(QString)), this, SLOT(on_music_search_edited(QString)));
  connect(ui_sfx_search, SIGNAL(textChanged(QString)), this, SLOT(on_sfx_search_edited(QString)));

  connect(ui_witness_testimony, SIGNAL(clicked()), this, SLOT(on_witness_testimony_clicked()));
  connect(ui_cross_examination, SIGNAL(clicked()), this, SLOT(on_cross_examination_clicked()));

  connect(ui_change_character, SIGNAL(clicked()), this, SLOT(on_change_character_clicked()));
  connect(ui_reload_theme, SIGNAL(clicked()), this, SLOT(on_reload_theme_clicked()));
  connect(ui_call_mod, SIGNAL(clicked()), this, SLOT(on_call_mod_clicked()));

  connect(ui_confirm_theme, SIGNAL(clicked()), this, SLOT(on_confirm_theme_clicked()));
  connect(ui_note_button, SIGNAL(clicked()), this, SLOT(on_note_button_clicked()));

  connect(ui_vp_notepad, SIGNAL(textChanged()), this, SLOT(on_note_text_changed()));

  connect(ui_pre, SIGNAL(clicked()), this, SLOT(on_pre_clicked()));
  connect(ui_flip, SIGNAL(clicked()), this, SLOT(on_flip_clicked()));
  connect(ui_guard, SIGNAL(clicked()), this, SLOT(on_guard_clicked()));

  connect(ui_sfx_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_sfx_list_clicked()));

  connect(ui_evidence_button, SIGNAL(clicked()), this, SLOT(on_evidence_button_clicked()));

  connect(ui_note_area->add_button, SIGNAL(clicked(bool)), this, SLOT(on_add_button_clicked()));
  connect(ui_set_notes, SIGNAL(clicked(bool)), this, SLOT(on_set_notes_clicked()));
  set_widgets();

  set_char_select();
}

Courtroom::~Courtroom()
{
  delete music_anim;
}

void Courtroom::set_mute_list()
{
  mute_map.clear();

  //maps which characters are muted based on cid, none are muted by default
  for (int n_cid = 0 ; n_cid < char_list.size() ; n_cid++)
  {
    mute_map.insert(n_cid, false);
  }

  QStringList sorted_mute_list;

  for (char_type i_char : char_list)
    sorted_mute_list.append(i_char.name);

  sorted_mute_list.sort();

  for (QString i_name : sorted_mute_list)
  {
    //mute_map.insert(i_name, false);
    ui_mute_list->addItem(i_name);
  }
}

void Courtroom::set_widgets()
{
  blip_rate = ao_app->read_blip_rate();
  blank_blip = ao_app->get_blank_blip();

  QString filename = "courtroom_design.ini";
  pos_size_type f_courtroom = ao_app->get_element_dimensions("courtroom", filename);

  if (f_courtroom.width < 0 || f_courtroom.height < 0)
  {
    qDebug() << "W: did not find courtroom width or height in " << filename;

    this->resize(714, 668);
  }
  else
  {
    m_courtroom_width = f_courtroom.width;
    m_courtroom_height = f_courtroom.height;

    this->resize(f_courtroom.width, f_courtroom.height);
  }

  set_fonts();

  ui_background->move(0, 0);
  ui_background->resize(m_courtroom_width, m_courtroom_height);
  ui_background->set_image("courtroombackground.png");

  set_size_and_pos(ui_viewport, "viewport");

  ui_vp_background->move(0, 0);
  ui_vp_background->resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_speedlines->move(0, 0);
  ui_vp_speedlines->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_player_char->move(0, 0);
  ui_vp_player_char->combo_resize(ui_viewport->width(), ui_viewport->height());

  //the AO2 desk element
  ui_vp_desk->move(0, 0);
  ui_vp_desk->resize(ui_viewport->width(), ui_viewport->height());

  //the size of the ui_vp_legacy_desk element relies on various factors and is set in set_scene()

  double y_modifier = 147.0 / 192.0;
  int final_y = y_modifier * ui_viewport->height();
  ui_vp_legacy_desk->move(0, final_y);
  ui_vp_legacy_desk->hide();

  ui_vp_evidence_display->move(0, 0);
  ui_vp_evidence_display->resize(ui_viewport->width(), ui_viewport->height());

  set_size_and_pos(ui_vp_notepad_image, "notepad_image");
  ui_vp_notepad_image->set_image("notepad_image.png");
  ui_vp_notepad_image->hide();

  set_size_and_pos(ui_vp_notepad, "notepad");
  ui_vp_notepad->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_notepad->verticalScrollBar()->hide();
  ui_vp_notepad->verticalScrollBar()->resize(0, 0);
  ui_vp_notepad->hide();

  set_size_and_pos(ui_vp_showname, "showname");

  set_size_and_pos(ui_vp_message, "message");
  ui_vp_message->setTextInteractionFlags(Qt::NoTextInteraction);
  ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                               "color: white");

  ui_vp_testimony->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_testimony->resize(ui_viewport->width(), ui_viewport->height());
  ui_vp_testimony->set_image("testimony.png");
  ui_vp_testimony->hide();

  ui_vp_effect->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_effect->resize(ui_viewport->width(), ui_viewport->height());
  ui_vp_effect->hide();

  ui_vp_wtce->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_wtce->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_objection->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_objection->combo_resize(ui_viewport->width(), ui_viewport->height());

  set_size_and_pos(ui_ic_chatlog, "ic_chatlog");

  set_size_and_pos(ui_ms_chatlog, "ms_chatlog");

  set_size_and_pos(ui_server_chatlog, "server_chatlog");

  set_size_and_pos(ui_mute_list, "mute_list");
  ui_mute_list->hide();

  //set_size_and_pos(ui_area_list, "area_list");
  //ui_area_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_music_list, "music_list");

  set_size_and_pos(ui_sfx_list, "sfx_list");

  if (is_ao2_bg)
  {
    set_size_and_pos(ui_ic_chat_message, "ao2_ic_chat_message");
    set_size_and_pos(ui_vp_chatbox, "ao2_chatbox");
  }
  else
  {
    set_size_and_pos(ui_ic_chat_message, "ic_chat_message");
    set_size_and_pos(ui_vp_chatbox, "chatbox");
  }

  set_size_and_pos(ui_vp_music_area, "music_area");
  ui_vp_music_area->show();
  set_size_and_pos(ui_vp_music_name, "music_name");

  set_size_and_pos(ui_vp_music_display_a, "music_display_a");
  ui_vp_music_display_a->set_image("music_display_a.png");
  ui_vp_music_display_a->show();

  set_size_and_pos(ui_vp_music_display_b, "music_display_b");
  ui_vp_music_display_b->set_image("music_display_b.png");
  ui_vp_music_display_b->show();

  ui_ic_chat_message->setStyleSheet("QLineEdit{background-color: rgba(100, 100, 100, 255);}");

  ui_vp_chatbox->set_image("chatmed.png");
  ui_vp_chatbox->hide();

  ui_muted->resize(ui_ic_chat_message->width(), ui_ic_chat_message->height());
  ui_muted->set_image("muted.png");

  set_size_and_pos(ui_ooc_chat_message, "ooc_chat_message");
  ui_ooc_chat_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_ooc_chat_name, "ooc_chat_name");
  ui_ooc_chat_name->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  //set_size_and_pos(ui_area_password, "area_password");
  set_size_and_pos(ui_music_search, "music_search");

  set_size_and_pos(ui_sfx_search, "sfx_search");

  set_size_and_pos(ui_emotes, "emotes");

  set_size_and_pos(ui_emote_left, "emote_left");
  ui_emote_left->set_image("arrow_left.png");

  set_size_and_pos(ui_emote_right, "emote_right");
  ui_emote_right->set_image("arrow_right.png");

  set_size_and_pos(ui_emote_dropdown, "emote_dropdown");

  set_size_and_pos(ui_pos_dropdown, "pos_dropdown");

  set_size_and_pos(ui_defense_bar, "defense_bar");
  ui_defense_bar->set_image("defensebar" + QString::number(defense_bar_state) + ".png");

  set_size_and_pos(ui_prosecution_bar, "prosecution_bar");
  ui_prosecution_bar->set_image("prosecutionbar" + QString::number(prosecution_bar_state) + ".png");

  set_size_and_pos(ui_music_label, "music_label");
  ui_music_label->setText("Music");
  set_size_and_pos(ui_sfx_label, "sfx_label");
  ui_sfx_label->setText("Sfx");
  set_size_and_pos(ui_blip_label, "blip_label");
  ui_blip_label->setText("Blips");

  set_size_and_pos(ui_shouts[0], "hold_it");
  ui_shouts[0]->show();
  set_size_and_pos(ui_shouts[1], "objection");
  ui_shouts[1]->show();
  set_size_and_pos(ui_shouts[2], "take_that");
  ui_shouts[2]->show();
  set_size_and_pos(ui_shouts[3], "custom_objection");
  set_size_and_pos(ui_shouts[4], "got_it");
  set_size_and_pos(ui_shouts[5], "cross_swords");
  set_size_and_pos(ui_shouts[6], "counter_alt");
  reset_shout_buttons();

  set_size_and_pos(ui_shout_up, "shout_up");
  ui_shout_up->set_image("shoutup.png");
  ui_shout_up->hide();
  set_size_and_pos(ui_shout_down, "shout_down");
  ui_shout_down->set_image("shoutdown.png");
  ui_shout_down->hide();

  if( ao_app->read_design_ini( "enable_single_shout", ao_app->get_theme_path() + "courtroom_config.ini" ) == "true" ) // courtroom_config.ini necessary
    {
      for(int i = 0; i < ui_shouts.size(); ++i)
        {
          move_widget(ui_shouts[i], "bullet");
        }
      set_shouts();

      ui_shout_up->show();
      ui_shout_down->show();
    }

  set_size_and_pos(ui_ooc_toggle, "ooc_toggle");
  ui_ooc_toggle->setText("Server");

  set_size_and_pos(ui_witness_testimony, "witness_testimony");
  ui_witness_testimony->set_image("witnesstestimony.png");
  set_size_and_pos(ui_cross_examination, "cross_examination");
  ui_cross_examination->set_image("crossexamination.png");

  set_size_and_pos(ui_change_character, "change_character");
//  ui_change_character->setText("Change character");
  ui_change_character->set_image("changecharacter.png");

  set_size_and_pos(ui_reload_theme, "reload_theme");
//  ui_reload_theme->setText("Reload theme");
  ui_reload_theme->set_image("reloadtheme.png");

  set_size_and_pos(ui_call_mod, "call_mod");
//  ui_call_mod->setText("Call mod")
  ui_call_mod->set_image("callmod.png");

  set_size_and_pos(ui_theme_list, "theme_list");

  set_size_and_pos(ui_confirm_theme, "confirm_theme");
//  ui_confirm_theme->setText("^");
  ui_confirm_theme->set_image("confirmtheme.png");
  set_size_and_pos(ui_note_button, "note_button");
//  ui_note_button->setText("><:");
  ui_note_button->set_image("notebutton.png");

  set_size_and_pos(ui_pre, "pre");
  ui_pre->setText("Pre");

  set_size_and_pos(ui_flip, "flip");

  set_size_and_pos(ui_guard, "guard");

  set_size_and_pos(ui_effect_flash, "effect_flash");
  set_size_and_pos(ui_effect_gloom, "effect_gloom");
  reset_effect_buttons();

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

  set_size_and_pos(ui_evidence_button, "evidence_button");
  ui_evidence_button->set_image("evidencebutton.png");

  set_size_and_pos(ui_evidence, "evidence_background");
  ui_evidence->set_image("evidencebackground.png");

  set_size_and_pos(ui_evidence_name, "evidence_name");

  set_size_and_pos(ui_evidence_buttons, "evidence_buttons");

  set_size_and_pos(ui_evidence_left, "evidence_left");
  ui_evidence_left->set_image("arrow_left.png");

  set_size_and_pos(ui_evidence_right, "evidence_right");
  ui_evidence_right->set_image("arrow_right.png");

  set_size_and_pos(ui_evidence_present, "evidence_present");
  ui_evidence_present->set_image("present_disabled.png");

  set_size_and_pos(ui_evidence_overlay, "evidence_overlay");
  ui_evidence_overlay->set_image("evidenceoverlay.png");

  set_size_and_pos(ui_evidence_delete, "evidence_delete");
  ui_evidence_delete->set_image("deleteevidence.png");

  set_size_and_pos(ui_evidence_image_name, "evidence_image_name");

  set_size_and_pos(ui_evidence_image_button, "evidence_image_button");

  set_size_and_pos(ui_evidence_x, "evidence_x");
  ui_evidence_x->set_image("evidencex.png");

  set_size_and_pos(ui_evidence_description, "evidence_description");

  ui_selector->set_image("char_selector.png");
  ui_selector->hide();

  set_size_and_pos(ui_back_to_lobby, "back_to_lobby");
  ui_back_to_lobby->setText("Back to Lobby");

  set_size_and_pos(ui_char_password, "char_password");

  set_size_and_pos(ui_char_buttons, "char_buttons");

  set_size_and_pos(ui_char_select_left, "char_select_left");
  ui_char_select_left->set_image("arrow_left.png");

  set_size_and_pos(ui_char_select_right, "char_select_right");
  ui_char_select_right->set_image("arrow_right.png");

  set_size_and_pos(ui_spectator, "spectator");

  handle_music_anim("music_name", "music_area");

  set_size_and_pos(ui_set_notes, "set_notes_button");
  ui_set_notes->set_image("set_notes.png");
  ui_note_area->m_layout->setSpacing(10);
  set_size_and_pos(ui_note_area, "note_area");
  set_size_and_pos(note_scroll_area, "note_area");
  note_scroll_area->setWidget(ui_note_area);
  ui_note_area->set_image("note_area.png");
  ui_note_area->add_button->set_image("add_button.png");
  ui_note_area->add_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  ui_note_area->setLayout(ui_note_area->m_layout);
  ui_note_area->show();
  note_scroll_area->hide();

  list_note_files();

  if(!contains_add_button)
    {
      ui_note_area->m_layout->addWidget(ui_note_area->add_button);
      contains_add_button = true;
    }

  set_dropdowns();
}

void Courtroom::set_fonts()
{
  set_font(ui_vp_showname, "showname");
  set_font(ui_vp_message, "message");
  set_font(ui_ic_chatlog, "ic_chatlog");
  set_font(ui_ms_chatlog, "ms_chatlog");
  set_font(ui_server_chatlog, "server_chatlog");
  set_font(ui_music_list, "music_list");
  set_font(ui_sfx_list, "sfx_list");
  set_font(ui_vp_music_name, "music_name");
  set_font(ui_vp_notepad, "notepad");
}

void Courtroom::set_font(QWidget *widget, QString p_identifier)
{
  QString design_file = "courtroom_fonts.ini";
  int f_weight = ao_app->get_font_size(p_identifier, design_file);
  QString class_name = widget->metaObject()->className();

  QString font_name = ao_app->get_font_name("font_" + p_identifier, design_file);

  widget->setFont(QFont(font_name, f_weight));

  QColor f_color = ao_app->get_color(p_identifier + "_color", design_file);

  int bold = ao_app->get_font_size(p_identifier + "_bold", design_file); // is the font bold or not?

  QString is_bold = "";
  if(bold == 1) is_bold = "bold";

  QString style_sheet_string = class_name + " { background-color: rgba(0, 0, 0, 0);\n" +
                               "color: rgba(" +
                               QString::number(f_color.red()) + ", " +
                               QString::number(f_color.green()) + ", " +
                               QString::number(f_color.blue()) + ", 255);\n"
                                                                 "font: " + is_bold + "; }";

  widget->setStyleSheet(style_sheet_string);
}

void Courtroom::set_dropdown(QWidget *widget, QString target_tag)
{
  QString style_sheet_string = ao_app->get_stylesheet(target_tag);

  widget->setStyleSheet(style_sheet_string);
}

void Courtroom::set_dropdowns()
{
  set_dropdown(ui_text_color, "[TEXT COLOR]");
  set_dropdown(ui_pos_dropdown, "[POS DROPDOWN]");
  set_dropdown(ui_theme_list, "[THEME LIST]");
  set_dropdown(ui_emote_dropdown, "[EMOTE DROPDOWN]");
  set_dropdown(ui_mute_list, "[MUTE LIST]");
}

void Courtroom::move_widget(QWidget *p_widget, QString p_identifier)
{
  QString filename = "courtroom_design.ini";

  pos_size_type design_ini_result = ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0)
  {
    qDebug() << "W: could not find \"" << p_identifier << "\" in " << filename;
    p_widget->hide();
  }
  else
  {
    p_widget->move(design_ini_result.x, design_ini_result.y);
  }
}

void Courtroom::set_shouts()
{
  for(int i = 0; i < ui_shouts.size(); i++)
    {
      ui_shouts[i]->hide();
    }
  ui_shouts[m_shout_state]->show();
}

void Courtroom::handle_music_anim(QString p_identifier_a, QString p_identifier_b)
{
  QString file_a = "courtroom_design.ini";
  QString file_b = "courtroom_fonts.ini";
  pos_size_type res_a = ao_app->get_element_dimensions(p_identifier_a, file_a);
  pos_size_type res_b = ao_app->get_element_dimensions(p_identifier_b, file_a);
  float speed = static_cast<float>(ao_app->get_font_size(p_identifier_a + "_speed", file_b));

  QFont f_font = ui_vp_music_name->font();
  QFontMetrics fm(f_font);
  int dist = fm.width(ui_vp_music_name->toPlainText());
  int time = static_cast<int>(1000000*dist/speed);
  music_anim->setLoopCount(-1);
  music_anim->setDuration(time);
  music_anim->setStartValue(QRect(res_b.width + res_b.x, res_a.y, res_a.width, res_a.height));
  music_anim->setEndValue(QRect(-dist + res_a.x, res_a.y, res_a.width, res_a.height));
  music_anim->start();
}

void Courtroom::set_window_title(QString p_title)
{
  this->setWindowTitle(p_title);
}

void Courtroom::set_char_rpc()
{
  rpc_char_list.clear();

  QFile config_file(ao_app->get_base_path() + "configs/rpccharlist.ini");
  if (!config_file.open(QIODevice::ReadOnly))
    { qDebug() << "Error reading rpccharlist.ini"; return; }

  QTextStream in(&config_file);

  while(!in.atEnd())
  {
    QString f_line = in.readLine().trimmed();

    QStringList line_elements = f_line.split("-");

    rpc_char_list.append(line_elements.at(1).trimmed().toLower());
  }

  config_file.close();
}

void Courtroom::set_size_and_pos(QWidget *p_widget, QString p_identifier)
{
  QString filename = "courtroom_design.ini";


  pos_size_type design_ini_result = ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0)
  {
    qDebug() << "W: could not find \"" << p_identifier << "\" in " << filename;
    p_widget->hide();
  }
  else
  {
    p_widget->move(design_ini_result.x, design_ini_result.y);
    p_widget->resize(design_ini_result.width, design_ini_result.height);
  }
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
  f_char.evidence_string = char_list.at(n_char).evidence_string;

  char_list.replace(n_char, f_char);
}

void Courtroom::done_received()
{
  m_cid = -1;

  m_music_player->set_volume(0);
  m_sfx_player->set_volume(0);
  m_shout_player->set_volume(0);
  m_blip_player->set_volume(0);

  set_char_select_page();

  set_mute_list();

  set_char_select();

  show();

  ui_spectator->show();
}

void Courtroom::set_background(QString p_background)
{
  testimony_in_progress = false;

  current_background = p_background;
  QString bg_path = get_background_path();

  is_ao2_bg = file_exists(bg_path + "defensedesk.png") &&
              file_exists(bg_path + "prosecutiondesk.png") &&
              file_exists(bg_path + "stand.png");

  if (is_ao2_bg)
  {
    set_size_and_pos(ui_vp_chatbox, "ao2_chatbox");
    set_size_and_pos(ui_ic_chat_message, "ao2_ic_chat_message");
  }
  else
  {
    set_size_and_pos(ui_vp_chatbox, "chatbox");
    set_size_and_pos(ui_ic_chat_message, "ic_chat_message");
  }
}

void Courtroom::enter_courtroom(int p_cid)
{
  m_cid = p_cid;

  QString f_char;

  set_char_rpc();

  if (m_cid == -1)
  {
    ao_app->discord->state_spectate();
    f_char = "";
  }
  else
  {
    f_char = ao_app->get_char_name(char_list.at(m_cid).name);
    if(!rpc_char_list.contains(f_char.toLower()))
      {
        ao_app->discord->toggle(1);
      }
    else
      {
        ao_app->discord->toggle(0);
      }

    ao_app->discord->state_character(f_char.toStdString());
  }

  current_char = f_char;

  current_emote_page = 0;
  current_emote = 0;

  if (m_cid == -1)
    ui_emotes->hide();
  else
    ui_emotes->show();

  set_emote_page();
  set_emote_dropdown();

  current_evidence_page = 0;
  current_evidence = 0;

  m_shout_state = 0;

  set_evidence_page();

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

  check_shouts();

  if (ao_app->custom_objection_enabled)
  {

    for(int i = 0; i < ui_shouts.size(); ++i)
      {
        if(shouts_enabled[i])
          {
            ui_shouts[i]->show();
          }
        else
          {
            ui_shouts[i]->hide();
          }
      }
  }
  else
  {
    for(int i = 3; i < ui_shouts.size(); i++) // the non-official shouts
      ui_shouts[i]->hide();
  }

  if (ao_app->flipping_enabled)
    ui_flip->show();
  else
    ui_flip->hide();

  list_music();
  list_sfx();
  list_themes();

  ui_sfx_list->setCurrentItem(ui_sfx_list->item(0)); // prevents undefined errors

  m_music_player->set_volume(ui_music_slider->value());
  m_sfx_player->set_volume(ui_sfx_slider->value());
  m_shout_player->set_volume(ui_sfx_slider->value());
  m_blip_player->set_volume(ui_blip_slider->value());

  testimony_in_progress = false;

  set_widgets();

  //ui_server_chatlog->setHtml(ui_server_chatlog->toHtml());

  ui_char_select_background->hide();

  ui_ic_chat_message->setEnabled(m_cid != -1);
  ui_ic_chat_message->setFocus();
}

void Courtroom::list_music()
{
  ui_music_list->clear();

  QString f_file = "courtroom_design.ini";

  QBrush found_brush(ao_app->get_color("found_song_color", f_file));
  QBrush missing_brush(ao_app->get_color("missing_song_color", f_file));

  int n_listed_songs = 0;

  for (int n_song = 0 ; n_song < music_list.size() ; ++n_song)
  {
    QString i_song = music_list.at(n_song);

    if (i_song.toLower().contains(ui_music_search->text().toLower()))
    {
      ui_music_list->addItem(i_song);

      QString song_path = ao_app->get_base_path() + "sounds/music/" + i_song.toLower();

      if (file_exists(song_path))
        ui_music_list->item(n_listed_songs)->setBackground(found_brush);
      else
        ui_music_list->item(n_listed_songs)->setBackground(missing_brush);

      ++n_listed_songs;
    }
  }
}

void Courtroom::list_sfx()
{
  ui_sfx_list->clear();
  sfx_names.clear();

  QString f_file = "courtroom_design.ini";

  QStringList sfx_list = ao_app->get_sfx_list();

  QBrush found_brush(ao_app->get_color("found_song_color", f_file));
  QBrush missing_brush(ao_app->get_color("missing_song_color", f_file));

  ui_sfx_list->addItem("Default");
  ui_sfx_list->addItem("Silence");

  sfx_names.append("1"); // Default
  sfx_names.append("1"); // Silence

  int n_listed_sfxs = 0;

  for (int n_sfx = 0 ; n_sfx < sfx_list.size() ; ++n_sfx)
  {
    sfx_names.append(sfx_list.at(n_sfx).split("=").at(0).trimmed());
    QString i_sfx = sfx_names.at(n_sfx);
    QString d_sfx = "";
    if(sfx_list.at(n_sfx).split("=").size() < 2)
      d_sfx = i_sfx;
    else d_sfx = sfx_list.at(n_sfx).split("=").at(1).trimmed();

    if (i_sfx.toLower().contains(ui_sfx_search->text().toLower()))
    {
      ui_sfx_list->addItem(d_sfx);

      QString sfx_path = ao_app->get_base_path() + "sounds/general/" + i_sfx.toLower();

      if (file_exists(sfx_path))
        ui_sfx_list->item(n_listed_sfxs)->setBackground(found_brush);
      else
        ui_sfx_list->item(n_listed_sfxs)->setBackground(missing_brush);

      ++n_listed_sfxs;
    }
  }
}

void Courtroom::list_note_files()
{
  QString f_config = ao_app->get_base_path() + "configs/filesabstract.ini";
  QFile f_file(f_config);
  if(!f_file.open(QIODevice::ReadOnly))
    { qDebug() << "Couldn't open" << f_config; return; }

  note_list.clear();

  QString f_filestring = "";
  QString f_filename = "";

  QTextStream in(&f_file);

  QVBoxLayout *f_layout = ui_note_area->m_layout;

  while(!in.atEnd())
    {
      QString line = in.readLine().trimmed();

      QStringList f_contents = line.split("=");
      if(f_contents.size() < 2)
        continue;

      int f_index = f_contents.at(0).toInt();
      f_filestring = f_filename = f_contents.at(1).trimmed();

      if(f_contents.size() > 2)
        f_filename = f_contents.at(2).trimmed();

      while(f_index >= f_layout->count())
        on_add_button_clicked();

      AONotePicker *f_notepicker = static_cast<AONotePicker*>(f_layout->itemAt(f_index)->widget());
      f_notepicker->m_line->setText(f_filename);
      f_notepicker->real_file = f_filestring;
    }
}

void Courtroom::load_note()
{
  QString f_text = ao_app->read_note(current_file);
  ui_vp_notepad->setText(f_text);
}

void Courtroom::save_note()
{
  QString f_text = ui_vp_notepad->toPlainText();

  ao_app->write_note(f_text, current_file);
}

void Courtroom::save_textlog(QString p_text)
{
  QString f_file = ao_app->get_base_path() + icchatlogsfilename;

  ao_app->append_note(p_text, f_file);
}

void Courtroom::list_themes()
{
  QString themes = ao_app->get_base_path() + "themes/";
  QDir dir(themes);
  ui_theme_list->clear();

  QStringList lis = dir.entryList();
  for(QString s : lis)
  {
    if(s != "." && s != "..")
        ui_theme_list->addItem(s);
  }
}

void Courtroom::append_ms_chatmessage(QString f_name, QString f_message)
{
  ui_ms_chatlog->append_chatmessage(f_name, f_message);
}

void Courtroom::append_server_chatmessage(QString p_name, QString p_message)
{
  ui_server_chatlog->append_chatmessage(p_name, p_message);
}

void Courtroom::on_chat_return_pressed()
{
  if (ui_ic_chat_message->text() == "" || is_muted)
    return;

  if ((anim_state < 3 || text_state < 2) &&
      m_objection_state == 0)
    return;

  //  qDebug() << "prev_emote = " << prev_emote << "current_emote = " << current_emote;

  if(prev_emote == current_emote) same_emote = true;
  else same_emote = false;

  //  qDebug() << "same_emote = " << same_emote;
  //MS#
  //deskmod#
  //pre-emote#
  //character#
  //emote#
  //message#
  //side#
  //sfx-name#
  //emote_modifier#
  //char_id#
  //sfx_delay#
  //objection_modifier#
  //evidence#
  //placeholder#
  //realization#
  //text_color#%

  QStringList packet_contents;

  QString f_side = ao_app->get_char_side(current_char);

  QString f_desk_mod = "chat";

  if (ao_app->desk_mod_enabled)
  {
    f_desk_mod = QString::number(ao_app->get_desk_mod(current_char, current_emote));
    if (f_desk_mod == "-1")
      f_desk_mod = "chat";
  }

  packet_contents.append(f_desk_mod);

  packet_contents.append(ao_app->get_pre_emote(current_char, current_emote));

  packet_contents.append(current_char);

  packet_contents.append(ao_app->get_emote(current_char, current_emote));

  packet_contents.append(ui_ic_chat_message->text());

  packet_contents.append(f_side);

  //  packet_contents.append(ao_app->get_sfx_name(current_char, current_emote));
  //  packet_contents.append(ui_sfx_search->text());

  int row = ui_sfx_list->currentRow();
  if (row == -1 || row == 0) // default
    packet_contents.append(ao_app->get_sfx_name(current_char, current_emote));
  else if (QListWidgetItem *item = ui_sfx_list->item(row)) // selection
    packet_contents.append(sfx_names.at(row));

  int f_emote_mod = ao_app->get_emote_mod(current_char, current_emote);

  //needed or else legacy won't understand what we're saying
  if (m_objection_state > 0)
  {
    if (f_emote_mod == 5)
      f_emote_mod = 6;
    else
      f_emote_mod = 2;
  }
  else if (ui_pre->isChecked())
  {
    if (f_emote_mod == 0)
      f_emote_mod = 1;
    else if (f_emote_mod == 5 && ao_app->prezoom_enabled)
      f_emote_mod = 4;
  }
  else
  {
    if (f_emote_mod == 1)
      f_emote_mod = 0;
    else if (f_emote_mod == 4)
      f_emote_mod = 5;
  }

  packet_contents.append(QString::number(f_emote_mod));
  packet_contents.append(QString::number(m_cid));

  packet_contents.append(QString::number(ao_app->get_sfx_delay(current_char, current_emote)));

  QString f_obj_state;

  if (m_objection_state < 0 || (!ao_app->custom_objection_enabled && m_objection_state > 3))
    f_obj_state = "0";
  else
    f_obj_state = QString::number(m_objection_state);

  packet_contents.append(f_obj_state);

  if (is_presenting_evidence)
    //the evidence index is shifted by 1 because 0 is no evidence per legacy standards
    //besides, older clients crash if we pass -1
    packet_contents.append(QString::number(current_evidence + 1));
  else
    packet_contents.append("0");

  QString f_flip;

  if (ao_app->flipping_enabled)
  {
    if (ui_flip->isChecked())
      f_flip = "1";
    else
      f_flip = "0";
  }
  else
    f_flip = QString::number(m_cid);

  packet_contents.append(f_flip);

  packet_contents.append(QString::number(m_effect_state));

  QString f_text_color;

  if (m_text_color < 0)
    f_text_color = "0";
  else if (m_text_color > 4 && !ao_app->yellow_text_enabled)
    f_text_color = "0";
  else
    f_text_color = QString::number(m_text_color);

  packet_contents.append(f_text_color);

  prev_emote = current_emote;

  ao_app->send_server_packet(new AOPacket("MS", packet_contents));
}

void Courtroom::handle_char_anim(AOCharMovie *charPlayer)
{
  int time = ao_app->read_config("opacity_time").toInt();
  int op = ao_app->read_config("char_opacity").toInt();

  QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect;
  QPropertyAnimation *anim = new QPropertyAnimation(opacity, "opacity");

  charPlayer->setGraphicsEffect(opacity);

  anim->setDuration(time);
  anim->setStartValue(1.0);
  anim->setEndValue(op);
  anim->setEasingCurve(QEasingCurve::InCubic);
  anim->start();
}

void Courtroom::handle_char_anim_2(AOCharMovie *charPlayer)
{
  int time = ao_app->read_config("opacity_time").toInt();
  int op = ao_app->read_config("char_opacity").toInt();

  QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect;
  QPropertyAnimation *anim = new QPropertyAnimation(opacity, "opacity");

  charPlayer->setGraphicsEffect(opacity);

  anim->setDuration(time);
  anim->setStartValue(op);
  anim->setEndValue(1.0);
  anim->setEasingCurve(QEasingCurve::InCubic);
  anim->start();
}

void Courtroom::handle_chatmessage(QStringList *p_contents)
{
  if (p_contents->size() < chatmessage_size)
    return;

  for (int n_string = 0 ; n_string < chatmessage_size ; ++n_string)
  {
    m_chatmessage[n_string] = p_contents->at(n_string);
    //    qDebug() << "m_chatmessage[" << n_string << "] = " << m_chatmessage[n_string];
  }

  int f_char_id = m_chatmessage[CHAR_ID].toInt();

  if (f_char_id < 0 || f_char_id >= char_list.size())
    return;

  if (mute_map.value(m_chatmessage[CHAR_ID].toInt()))
    return;

  QString f_showname = ao_app->get_showname(char_list.at(f_char_id).name);

  QString f_message = f_showname + ": " + m_chatmessage[MESSAGE] + '\n';

  if (f_message == previous_ic_message)
    return;

  text_state = 0;
  anim_state = 0;
  ui_vp_objection->stop();
  ui_vp_player_char->stop();
  chat_tick_timer->stop();
  ui_vp_evidence_display->reset();

  // reset effect
  ui_vp_effect->stop();

  chatmessage_is_empty = m_chatmessage[MESSAGE] == " " || m_chatmessage[MESSAGE] == "";

  if (m_chatmessage[MESSAGE] == ui_ic_chat_message->text() && m_chatmessage[CHAR_ID].toInt() == m_cid)
  {
    ui_ic_chat_message->clear();
    ui_sfx_list->setCurrentItem(ui_sfx_list->item(0));

    m_objection_state = 0;
    reset_shout_buttons();

    m_effect_state = 0;
    reset_effect_buttons();

    is_presenting_evidence = false;
    ui_evidence_present->set_image("present_disabled.png");
  }

  append_ic_text(": " + m_chatmessage[MESSAGE], f_showname);

  if(ao_app->read_config("enable_logging") == "true")
    save_textlog("[" + QTime::currentTime().toString() + "] " + f_showname + ": " + m_chatmessage[MESSAGE]);

  previous_ic_message = f_message;

  int objection_mod = m_chatmessage[OBJECTION_MOD].toInt();
  QString f_char = m_chatmessage[CHAR_NAME];
  QString f_custom_theme = ao_app->get_char_shouts(f_char);

  //if an objection is used
  if(!same_emote)
  {
    handle_char_anim(ui_vp_player_char);
    handle_char_anim_2(ui_vp_player_char);
  }
  if (objection_mod > 0)
  {
    int emote_mod = m_chatmessage[EMOTE_MOD].toInt();
    if (emote_mod == 0)
      m_chatmessage[EMOTE_MOD] = 1;

    //handles cases 1-7 (5-7 are DRO only)
    if(objection_mod >= 1 && objection_mod <= ui_shouts.size())
      {
        ui_vp_objection->play(shout_names[objection_mod-1], f_char, f_custom_theme);
        m_shout_player->play(shout_names[objection_mod-1] + ".wav", f_char);
      }
    else
      qDebug() << "W: Shout identifier unknown" << objection_mod;

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
  ui_vp_speedlines->stop();
  ui_vp_player_char->stop();

  QString real_name = char_list.at(m_chatmessage[CHAR_ID].toInt()).name;

  QString f_showname = ao_app->get_showname(real_name);

  ui_vp_showname->setText(f_showname);

  ui_vp_message->clear();
  ui_vp_chatbox->hide();

  QString chatbox = ao_app->get_chat(m_chatmessage[CHAR_NAME]);

  if (chatbox == "")
    ui_vp_chatbox->set_image("chatmed.png");
  else
  {
    QString chatbox_path = ao_app->get_base_path() + "misc/" + chatbox + ".png";
    ui_vp_chatbox->set_image_from_path(chatbox_path);
  }

  set_scene();
  set_text_color();

  int emote_mod = m_chatmessage[EMOTE_MOD].toInt();

  if (ao_app->flipping_enabled && m_chatmessage[FLIP].toInt() == 1)
    ui_vp_player_char->set_flipped(true);
  else
    ui_vp_player_char->set_flipped(false);


  switch (emote_mod)
  {
  case 1: case 2: case 6:
    play_preanim();
    break;
  default:
    qDebug() << "W: invalid emote mod: " << QString::number(emote_mod);
    //intentional fallthru
  case 0: case 5:
    handle_chatmessage_3();
  }
}

void Courtroom::handle_chatmessage_3()
{
  start_chat_ticking();

  int f_evi_id = m_chatmessage[EVIDENCE_ID].toInt();
  QString f_side = m_chatmessage[SIDE];

  if (f_evi_id > 0 && f_evi_id <= local_evidence_list.size())
  {
    //shifted by 1 because 0 is no evidence per legacy standards
    QString f_image = local_evidence_list.at(f_evi_id - 1).image;
    //def jud and hlp should display the evidence icon on the RIGHT side
    bool is_left_side = !(f_side == "def" || f_side == "hlp" || f_side == "jud");
    ui_vp_evidence_display->show_evidence(f_image, is_left_side, ui_sfx_slider->value());
  }

  int emote_mod = m_chatmessage[EMOTE_MOD].toInt();

  if (emote_mod == 5 ||
      emote_mod == 6)
  {
    QString side = m_chatmessage[SIDE];
    ui_vp_desk->hide();
    ui_vp_legacy_desk->hide();

    if (side == "pro" ||
        side == "hlp" ||
        side == "wit")
      ui_vp_speedlines->play("prosecution_speedlines");
    else
      ui_vp_speedlines->play("defense_speedlines");

  }

  int f_anim_state = 0;
  //BLUE is from an enum in datatypes.h
  bool text_is_blue = m_chatmessage[TEXT_COLOR].toInt() == BLUE;

  if (!text_is_blue && text_state == 1)
    //talking
    f_anim_state = 2;
  else
    //idle
    f_anim_state = 3;

  if (f_anim_state <= anim_state)
    return;

  ui_vp_player_char->stop();
  QString f_char = m_chatmessage[CHAR_NAME];
  QString f_emote = m_chatmessage[EMOTE];

  switch (f_anim_state)
  {
  case 2:
    ui_vp_player_char->play_talking(f_char, f_emote);
    anim_state = 2;
    break;
  default:
    qDebug() << "W: invalid anim_state: " << f_anim_state;
  case 3:
    //    handle_char_anim(ui_vp_player_char);
    ui_vp_player_char->play_idle(f_char, f_emote);
    anim_state = 3;
  }

  QString effect = m_chatmessage[EFFECT_STATE];
  if (effect == "1")
  {
    m_sfx_player->play(ao_app->get_sfx("effect_flash"));
    ui_vp_effect->set_play_once(true);
    ui_vp_effect->play("effect_flash");
    realization_timer->start(60);
  }
  else if (effect == "2")
  {
    m_sfx_player->play(ao_app->get_sfx("effect_gloom"));
    ui_vp_effect->set_play_once(false);
    ui_vp_effect->play("effect_gloom");
  }

  QString f_message = m_chatmessage[MESSAGE];
  QStringList call_words = ao_app->get_call_words();

  for (QString word : call_words)
  {
    if (f_message.contains(word, Qt::CaseInsensitive))
    {
      m_mod_player->play(ao_app->get_sfx("word_call"));
      ao_app->alert(this);

      break;
    }
  }

}

void Courtroom::append_ic_text(QString p_text, QString p_name)
{
  QTextCharFormat bold;
  QTextCharFormat normal;
  bold.setFontWeight(QFont::Bold);
  normal.setFontWeight(QFont::Normal);
  const QTextCursor old_cursor = ui_ic_chatlog->textCursor();
  const int old_scrollbar_value = ui_ic_chatlog->verticalScrollBar()->value();
  const bool is_scrolled_up = old_scrollbar_value == ui_ic_chatlog->verticalScrollBar()->maximum();

  ui_ic_chatlog->clear();

  // new record
  m_ic_records.append(std::make_shared<record_type>(p_name, p_text));

  int len = m_ic_records.length();
  if (len > 200) // magic numbers, woo!
  {
    m_ic_records = m_ic_records.mid(len - 200);
  }

  for (record_type_ptr record : m_ic_records)
  {
    ui_ic_chatlog->moveCursor(QTextCursor::Start);
    ui_ic_chatlog->textCursor().insertText(record->name, bold);
    ui_ic_chatlog->textCursor().insertText(record->line + '\n', normal);
  }

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
    ui_ic_chatlog->verticalScrollBar()->setValue(ui_ic_chatlog->verticalScrollBar()->maximum());
  }
}

void Courtroom::play_preanim()
{
  QString f_char = m_chatmessage[CHAR_NAME];
  QString f_preanim = m_chatmessage[PRE_EMOTE];

  //all time values in char.inis are multiplied by a constant(time_mod) to get the actual time
  int ao2_duration = ao_app->get_ao2_preanim_duration(f_char, f_preanim);
  int text_delay = ao_app->get_text_delay(f_char, f_preanim) * time_mod;
  int sfx_delay = m_chatmessage[SFX_DELAY].toInt() * 60;

  int preanim_duration;

  if (ao2_duration < 0)
    preanim_duration = ao_app->get_preanim_duration(f_char, f_preanim);
  else
    preanim_duration = ao2_duration;

  sfx_delay_timer->start(sfx_delay);

  if (!file_exists(ao_app->get_character_path(f_char) + f_preanim.toLower() + ".gif") ||
      preanim_duration < 0)
  {
    anim_state = 1;
    preanim_done();
    qDebug() << "could not find " + ao_app->get_character_path(f_char) + f_preanim.toLower() + ".gif";
    return;
  }

  ui_vp_player_char->play_pre(f_char, f_preanim, preanim_duration);
  anim_state = 1;
  if (text_delay >= 0)
    text_delay_timer->start(text_delay);

}

void Courtroom::preanim_done()
{
  handle_chatmessage_3();
}

void Courtroom::realization_done()
{
  ui_vp_effect->stop();
}

void Courtroom::start_chat_ticking()
{
  ui_vp_message->clear();
  set_text_color();
  rainbow_counter = 0;
  //we need to ensure that the text isn't already ticking because this function can be called by two logic paths
  if (text_state != 0)
    return;

  if (chatmessage_is_empty)
  {
    //since the message is empty, it's technically done ticking
    text_state = 2;
    return;
  }

  ui_vp_chatbox->show();

  tick_pos = 0;
  blip_pos = 0;
  chat_tick_timer->start(chat_tick_interval);

  QString f_gender = ao_app->get_gender(m_chatmessage[CHAR_NAME]);

  m_blip_player->set_file(f_gender);

  //means text is currently ticking
  text_state = 1;
}

void Courtroom::chat_tick()
{
  //note: this is called fairly often(every 60 ms when char is talking)
  //do not perform heavy operations here

  QString f_message = m_chatmessage[MESSAGE];

  if (tick_pos >= f_message.size())
  {
    text_state = 2;
    chat_tick_timer->stop();
    anim_state = 3;
    ui_vp_player_char->play_idle(m_chatmessage[CHAR_NAME], m_chatmessage[EMOTE]);
  }

  else
  {
    QString f_character = f_message.at(tick_pos);
    f_character = f_character.toHtmlEscaped();

    if (f_character == " ")
      ui_vp_message->insertPlainText(" ");
    else if (m_chatmessage[TEXT_COLOR].toInt() == RAINBOW)
    {
      QString html_color;

      switch (rainbow_counter)
      {
      case 0:
        html_color = "#FF0000";
        break;
      case 1:
        html_color = "#FF7F00";
        break;
      case 2:
        html_color = "#FFFF00";
        break;
      case 3:
        html_color = "#00FF00";
        break;
      default:
        html_color = "#2d96ff";
        rainbow_counter = -1;
      }

      ++rainbow_counter;

      ui_vp_message->insertHtml("<font color=\"" + html_color + "\">" + f_character + "</font>");
    }
    else
      ui_vp_message->insertHtml(f_character);

    QScrollBar *scroll = ui_vp_message->verticalScrollBar();
    scroll->setValue(scroll->maximum());

    if(blank_blip)
      qDebug() << "blank_blip found true";

    if ((f_message.at(tick_pos) != ' ' || blank_blip))
    {

      if (blip_pos % blip_rate == 0)
      {
        blip_pos = 0;

        // play blip
        m_blip_player->play();
      }

      ++blip_pos;
    }

    ++tick_pos;
  }
}

void Courtroom::show_testimony()
{
  if (!testimony_in_progress || m_chatmessage[SIDE] != "wit")
    return;

  ui_vp_testimony->show();

  testimony_show_timer->start(testimony_show_time);
}

void Courtroom::hide_testimony()
{
  ui_vp_testimony->hide();

  if (!testimony_in_progress)
    return;

  testimony_hide_timer->start(testimony_hide_time);
}

void Courtroom::play_sfx()
{
  QString sfx_name = m_chatmessage[SFX_NAME];
  if (sfx_name == "1")
    return;

  m_sfx_player->play(sfx_name + ".wav");
}

void Courtroom::set_scene()
{
  if (testimony_in_progress)
    show_testimony();

  //witness is default if pos is invalid
  QString f_background = "witnessempty";
  QString f_desk_image = "stand";
  QString f_desk_mod = m_chatmessage[DESK_MOD];
  QString f_side = m_chatmessage[SIDE];

  if (f_side == "def")
  {
    f_background = "defenseempty";
    if (is_ao2_bg)
      f_desk_image = "defensedesk";
    else
      f_desk_image = "bancodefensa";
  }
  else if (f_side == "pro")
  {
    f_background = "prosecutorempty";
    if (is_ao2_bg)
      f_desk_image = "prosecutiondesk";
    else
      f_desk_image = "bancoacusacion";
  }
  else if (f_side == "jud")
  {
    f_background = "judgestand";
    f_desk_image = "judgedesk";
  }
  else if (f_side == "hld")
  {
    f_background = "helperstand";
    f_desk_image = "helperdesk";
  }
  else if (f_side == "hlp")
  {
    f_background = "prohelperstand";
    f_desk_image = "prohelperdesk";
  }
  else
  {
    if (is_ao2_bg)
      f_desk_image = "stand";
    else
      f_desk_image = "estrado";
  }

  ui_vp_background->set_image(f_background);
  ui_vp_desk->set_image(f_desk_image);
  ui_vp_legacy_desk->set_legacy_desk(f_desk_image);

  if (f_desk_mod == "0" || (f_desk_mod != "1" &&
                            (f_side == "jud" ||
                             f_side == "hld" ||
                             f_side == "hlp")))
  {
    ui_vp_desk->hide();
    ui_vp_legacy_desk->hide();
  }
  else if (is_ao2_bg || (f_side == "jud" ||
                         f_side == "hld" ||
                         f_side == "hlp"))
  {
    ui_vp_legacy_desk->hide();
    ui_vp_desk->show();
  }
  else
  {
    if (f_side == "wit")
    {
      ui_vp_desk->show();
      ui_vp_legacy_desk->hide();
    }
    else
    {
      ui_vp_desk->hide();
      ui_vp_legacy_desk->show();
    }
  }
}

void Courtroom::set_text_color()
{
  switch (m_chatmessage[TEXT_COLOR].toInt())
  {
  case GREEN:
    ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: rgb(0, 255, 0)");
    break;
  case RED:
    ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: red");
    break;
  case ORANGE:
    ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: orange");
    break;
  case BLUE:
    ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: rgb(45, 150, 255)");
    break;
  case YELLOW:
    ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: yellow");
    break;
  default:
    qDebug() << "W: undefined text color: " << m_chatmessage[TEXT_COLOR];
  case WHITE:
    ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: white");

  }
}

void Courtroom::set_ip_list(QString p_list)
{
  QString f_list = p_list.replace("|", ":").replace("*", "\n");

  ui_server_chatlog->append(f_list);
}

void Courtroom::set_mute(bool p_muted, int p_cid)
{
  if (p_cid != m_cid && p_cid != -1)
    return;

  if (p_muted)
    ui_muted->show();
  else
  {
    ui_muted->hide();
    ui_ic_chat_message->setFocus();
  }

  ui_muted->resize(ui_ic_chat_message->width(), ui_ic_chat_message->height());
  ui_muted->set_image("muted.png");

  is_muted = p_muted;
  ui_ic_chat_message->setEnabled(!p_muted);
}

void Courtroom::set_ban(int p_cid)
{
  if (p_cid != m_cid && p_cid != -1)
    return;

  call_notice("You have been banned.");

  ao_app->construct_lobby();
  ao_app->destruct_courtroom();
}

void Courtroom::handle_song(QStringList *p_contents)
{

  QStringList f_contents = *p_contents;

  if (f_contents.size() < 2)
    return;

  QString f_song = f_contents.at(0);
  int n_char = f_contents.at(1).toInt();

  if (n_char < 0 || n_char >= char_list.size())
  {
    m_music_player->play(f_song);
  }
  else
  {
    QString str_char = char_list.at(n_char).name;

    if (!mute_map.value(n_char))
    {
      QString music_change_log = ao_app->read_config("music_change_log");

      if (music_change_log == ("false"))
      {
        m_music_player->play(f_song);
      }
      else
      {
        append_ic_text(" has played a song: " + f_song, str_char);
        m_music_player->play(f_song);
      }
    }
  }

  int pos = f_song.lastIndexOf(QChar('.'));
  QString r_song = f_song.left(pos);

  ui_vp_music_name->setText(r_song);

  handle_music_anim("music_name", "music_area");
}

void Courtroom::handle_wtce(QString p_wtce)
{
  QString sfx_file = "courtroom_sounds.ini";

  //witness testimony
  if (p_wtce == "testimony1")
  {
    m_sfx_player->play(ao_app->get_sfx("witness_testimony"));
    ui_vp_wtce->play("witnesstestimony");
    testimony_in_progress = true;
    show_testimony();
  }
  //cross examination
  else if (p_wtce == "testimony2")
  {
    m_sfx_player->play(ao_app->get_sfx("cross_examination"));
    ui_vp_wtce->play("crossexamination");
    testimony_in_progress = false;
  }
}

void Courtroom::set_hp_bar(int p_bar, int p_state)
{
  if (p_state < 0 || p_state > 10)
    return;

  if (p_bar == 1)
  {
    ui_defense_bar->set_image("defensebar" + QString::number(p_state) + ".png");
    defense_bar_state = p_state;
  }
  else if (p_bar == 2)
  {
    ui_prosecution_bar->set_image("prosecutionbar" + QString::number(p_state) + ".png");
    prosecution_bar_state = p_state;
  }
}

void Courtroom::check_shouts()
{
    QString char_path = ao_app->get_character_path(current_char);
    QString theme_path = ao_app->get_theme_path();
    for(int i = 0; i < ui_shouts.size(); ++i)
    {
        if(file_exists(char_path + shout_names[i] + ".gif") || file_exists(theme_path + shout_names[i] + ".gif"))
            shouts_enabled[i] = true;
        else shouts_enabled[i] = false;
    }
}

void Courtroom::mod_called(QString p_ip)
{
  ui_server_chatlog->append(p_ip);
  if (ui_guard->isChecked())
  {
    m_mod_player->play(ao_app->get_sfx("mod_call"));
    ao_app->alert(this);
  }
}

void Courtroom::on_ooc_return_pressed()
{
  QString ooc_message = ui_ooc_chat_message->text();

  if (ooc_message == "" || ui_ooc_chat_name->text() == "")
    return;

  if (ooc_message.startsWith("/pos"))
  {
    if (ooc_message.startsWith("/pos jud"))
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
  }
  else if (ooc_message.startsWith("/login"))
    ui_guard->show();
  else if (ooc_message.startsWith("/rainbow") && ao_app->yellow_text_enabled && !rainbow_appended)
  {
    ui_text_color->addItem("Rainbow");
    ui_ooc_chat_message->clear();
    rainbow_appended = true;
    return;
  }

  QStringList packet_contents;
  packet_contents.append(ui_ooc_chat_name->text());
  packet_contents.append(ooc_message);

  AOPacket *f_packet = new AOPacket("CT", packet_contents);

  if (server_ooc)
    ao_app->send_server_packet(f_packet);
  else
    ao_app->send_ms_packet(f_packet);

  ui_ooc_chat_message->clear();

  ui_ooc_chat_message->setFocus();
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

void Courtroom::on_music_search_edited(QString p_text)
{
  //preventing compiler warnings
  p_text += "a";
  list_music();
}

void Courtroom::on_sfx_search_edited(QString p_text)
{
  //preventing compiler warnings
  p_text += "a";
  list_sfx();
}

void Courtroom::on_pos_dropdown_changed(int p_index)
{
  ui_ic_chat_message->setFocus();

  if (p_index < 0 || p_index > 5)
    return;

  QString f_pos;

  switch (p_index)
  {
  case 0:
    f_pos = "wit";
    break;
  case 1:
    f_pos = "def";
    break;
  case 2:
    f_pos = "pro";
    break;
  case 3:
    f_pos = "jud";
    break;
  case 4:
    f_pos = "hld";
    break;
  case 5:
    f_pos = "hlp";
    break;
  default:
    f_pos = "";
  }

  if (f_pos == "" || ui_ooc_chat_name->text() == "")
    return;

  ao_app->send_server_packet(new AOPacket("CT#" + ui_ooc_chat_name->text() + "#/pos " + f_pos + "#%"));
}

void Courtroom::on_mute_list_clicked(QModelIndex p_index)
{
  QListWidgetItem *f_item = ui_mute_list->item(p_index.row());
  QString f_char = f_item->text();
  QString real_char;

  if (f_char.endsWith(" [x]"))
    real_char = f_char.left(f_char.size() - 4);
  else
    real_char = f_char;

  int f_cid = -1;

  for (int n_char = 0 ; n_char < char_list.size() ; n_char++)
  {
    if (char_list.at(n_char).name == real_char)
      f_cid = n_char;
  }

  if (f_cid < 0 || f_cid >= char_list.size())
  {
    qDebug() << "W: " << real_char << " not present in char_list";
    return;
  }

  if (mute_map.value(f_cid))
  {
    mute_map.insert(f_cid, false);
    f_item->setText(real_char);
  }
  else
  {
    mute_map.insert(f_cid, true);
    f_item->setText(real_char + " [x]");
  }



  /*
  if (f_char.endsWith(" [x]"))
  {
    real_char = f_char.left(f_char.size() - 4);
    mute_map.remove(real_char);
    mute_map.insert(real_char, false);
    f_item->setText(real_char);
  }
  else
  {
    real_char = f_char;
    mute_map.remove(real_char);
    mute_map.insert(real_char, true);
    f_item->setText(real_char + " [x]");
  }
  */
}

void Courtroom::on_music_list_double_clicked(QModelIndex p_model)
{
  if (is_muted)
    return;

  QString p_song = ui_music_list->item(p_model.row())->text();

  ao_app->send_server_packet(new AOPacket("MC#" + p_song + "#" + QString::number(m_cid) + "#%"), false);
}

void Courtroom::reset_shout_buttons()
{
  for(int i = 0; i < ui_shouts.size(); ++i)
    ui_shouts[i]->set_image(shout_names[i] + ".png");

  if(m_objection_state != 0)
    ui_shouts[m_objection_state-1]->set_image(shout_names[m_objection_state-1] + "_selected.png");
}

void Courtroom::on_shout_clicked()
{
  AOButton *f_shout_button = static_cast<AOButton*>(sender());
  int f_shout_id = f_shout_button->property("shout_id").toInt();

  // update based on current button selected
  if (f_shout_id == m_objection_state)
    m_objection_state = 0;
  else
    m_objection_state = f_shout_id;

  reset_shout_buttons();

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_cycle_clicked()
{
  AOButton *f_cycle_button = static_cast<AOButton*>(sender());
  int f_cycle_id = f_cycle_button->property("cycle_id").toInt();

  switch(f_cycle_id)
    {
    case 1:
      cycle_shout(-1);
    break;
    case 0:
      cycle_shout(1);
    break;
    default:
    break;
    }

  set_shouts();
}

void Courtroom::cycle_shout(int p_index)
{
  int n = ui_shouts.size();
  do { m_shout_state = (m_shout_state - p_index + n) % n; } while( !shouts_enabled[m_shout_state] );

  set_shouts();
}

void Courtroom::reset_effect_buttons()
{
  if (m_effect_state == 0)
  {
    ui_effect_flash->set_image("effect_flash.png");
    ui_effect_gloom->set_image("effect_gloom.png");
  }
  else if (m_effect_state == 1)
  {
    ui_effect_flash->set_image("effect_flash_pressed.png");
    ui_effect_gloom->set_image("effect_gloom.png");
  }
  else if (m_effect_state == 2)
  {
    ui_effect_flash->set_image("effect_flash.png");
    ui_effect_gloom->set_image("effect_gloom_pressed.png");
  }
}

void Courtroom::on_effect_button_clicked()
{
  AOButton *f_button = static_cast<AOButton*>(this->sender());

  int f_effect_id = f_button->property("effect_id").toInt();
  if (m_effect_state == f_effect_id)
    m_effect_state = 0;
  else
    m_effect_state = f_effect_id;

  reset_effect_buttons();

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_mute_clicked()
{
  if (ui_mute_list->isHidden())
  {
    ui_mute_list->show();
    ui_mute->set_image("mute_pressed.png");
  }
  else
  {
    ui_mute_list->hide();
    ui_mute->set_image("mute.png");
  }
}

void Courtroom::on_defense_minus_clicked()
{
  int f_state = defense_bar_state - 1;

  if (f_state >= 0)
    ao_app->send_server_packet(new AOPacket("HP#1#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_defense_plus_clicked()
{
  int f_state = defense_bar_state + 1;

  if (f_state <= 10)
    ao_app->send_server_packet(new AOPacket("HP#1#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_prosecution_minus_clicked()
{
  int f_state = prosecution_bar_state - 1;

  if (f_state >= 0)
    ao_app->send_server_packet(new AOPacket("HP#2#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_prosecution_plus_clicked()
{
  int f_state = prosecution_bar_state + 1;

  if (f_state <= 10)
    ao_app->send_server_packet(new AOPacket("HP#2#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_text_color_changed(int p_color)
{
  m_text_color = p_color;
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_music_slider_moved(int p_value)
{
  m_music_player->set_volume(p_value);
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_sfx_slider_moved(int p_value)
{
  m_sfx_player->set_volume(p_value);
  m_shout_player->set_volume(p_value);
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_blip_slider_moved(int p_value)
{
  m_blip_player->set_volume(p_value);
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_witness_testimony_clicked()
{
  if (is_muted)
    return;

  ao_app->send_server_packet(new AOPacket("RT#testimony1#%"));

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_cross_examination_clicked()
{
  if (is_muted)
    return;

  ao_app->send_server_packet(new AOPacket("RT#testimony2#%"));

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_change_character_clicked()
{
  m_music_player->set_volume(0);
  m_sfx_player->set_volume(0);
  m_sfx_player->set_volume(0);
  m_blip_player->set_volume(0);

  set_char_select();

  ui_char_select_background->show();
  ui_spectator->hide();
}

void Courtroom::on_reload_theme_clicked()
{
  ao_app->reload_theme();

  //to update status on the background
  set_background(current_background);
  enter_courtroom(m_cid);

  anim_state = 4;
  text_state = 3;
}

void Courtroom::on_back_to_lobby_clicked()
{
  ao_app->construct_lobby();
  ao_app->w_lobby->list_servers();
  ao_app->destruct_courtroom();
}

void Courtroom::on_confirm_theme_clicked()
{
  ao_app->write_theme(ui_theme_list->currentText());
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
  enter_courtroom(-1);

  ui_emotes->hide();

  ui_char_select_background->hide();
}

void Courtroom::on_call_mod_clicked()
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::warning(this, "Warning", "Are you sure you want to call a mod? They will get angry at you if the reason is no good.", QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

  if(reply == QMessageBox::Yes)
    {
      ao_app->send_server_packet(new AOPacket("ZZ#%"));
      qDebug() << "Called mod";
    }
  else
    qDebug() << "Did not call mod";

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_pre_clicked()
{
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_flip_clicked()
{
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_guard_clicked()
{
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_evidence_button_clicked()
{
  if (ui_evidence->isHidden())
  {
    ui_evidence->show();
    ui_evidence_overlay->hide();
  }
  else
  {
    ui_evidence->hide();
  }
}

void Courtroom::on_note_button_clicked()
{
  if(!note_shown)
  {
    load_note();
    ui_vp_notepad_image->show();
    ui_vp_notepad->show();
    note_shown = true;
  }
  else
  {
    save_note();
    ui_vp_notepad_image->hide();
    ui_vp_notepad->hide();
    note_shown = false;
  }
}

void Courtroom::on_note_text_changed()
{
  ao_app->write_note(ui_vp_notepad->toPlainText(), current_file);
}

void Courtroom::ping_server()
{
  ao_app->send_server_packet(new AOPacket("CH#" + QString::number(m_cid) + "#%"));
}

void Courtroom::on_sfx_list_clicked()
{
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_set_notes_clicked()
{
  if(note_scroll_area->isHidden())
    note_scroll_area->show();
  else
    note_scroll_area->hide();
}
