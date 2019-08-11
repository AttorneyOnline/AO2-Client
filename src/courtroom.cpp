#include "courtroom.h"

Courtroom::Courtroom(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;
  #ifdef BASSAUDIO
  // Change the default audio output device to be the one the user has given
  // in his config.ini file for now.
  unsigned int a = 0;
  BASS_DEVICEINFO info;

  if (ao_app->get_audio_output_device() == "default")
  {
      BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
      load_bass_opus_plugin();
  }
  else
  {
      for (a = 0; BASS_GetDeviceInfo(a, &info); a++)
      {
          if (ao_app->get_audio_output_device() == info.name)
          {
              BASS_SetDevice(a);
              BASS_Init(static_cast<int>(a), 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
              load_bass_opus_plugin();
              qDebug() << info.name << "was set as the default audio output device.";
              break;
          }
      }
  }
  #endif

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

  music_player = new AOMusicPlayer(this, ao_app);
  music_player->set_volume(0);

  sfx_player = new AOSfxPlayer(this, ao_app);
  sfx_player->set_volume(0);

  objection_player = new AOSfxPlayer(this, ao_app);
  objection_player->set_volume(0);

  blip_player = new AOBlipPlayer(this, ao_app);
  blip_player->set_volume(0);

  modcall_player = new AOSfxPlayer(this, ao_app);
  modcall_player->set_volume(50);

  ui_background = new AOImage(this, ao_app);

  ui_viewport = new QWidget(this);
  ui_vp_background = new AOScene(ui_viewport, ao_app);
  ui_vp_speedlines = new AOMovie(ui_viewport, ao_app);
  ui_vp_speedlines->set_play_once(false);
  ui_vp_player_char = new AOCharMovie(ui_viewport, ao_app);
  ui_vp_sideplayer_char = new AOCharMovie(ui_viewport, ao_app);
  ui_vp_sideplayer_char->hide();
  ui_vp_desk = new AOScene(ui_viewport, ao_app);
  ui_vp_legacy_desk = new AOScene(ui_viewport, ao_app);

  ui_vp_evidence_display = new AOEvidenceDisplay(this, ao_app);

  ui_vp_chatbox = new AOImage(this, ao_app);
  ui_vp_showname = new QLabel(ui_vp_chatbox);
  ui_vp_message = new QTextEdit(ui_vp_chatbox);
  ui_vp_message->setFrameStyle(QFrame::NoFrame);
  ui_vp_message->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setReadOnly(true);

  ui_vp_testimony = new AOImage(this, ao_app);
  ui_vp_realization = new AOImage(this, ao_app);
  ui_vp_wtce = new AOMovie(this, ao_app);
  ui_vp_objection = new AOMovie(this, ao_app);

  ui_ic_chatlog = new QTextEdit(this);
  ui_ic_chatlog->setReadOnly(true);

  log_maximum_blocks = ao_app->get_max_log_size();
  log_goes_downwards = ao_app->get_log_goes_downwards();

  ui_ms_chatlog = new AOTextArea(this);
  ui_ms_chatlog->setReadOnly(true);
  ui_ms_chatlog->setOpenExternalLinks(true);
  ui_ms_chatlog->hide();

  ui_server_chatlog = new AOTextArea(this);
  ui_server_chatlog->setReadOnly(true);
  ui_server_chatlog->setOpenExternalLinks(true);

  ui_area_list = new QListWidget(this);
  ui_area_list->hide();
  ui_music_list = new QListWidget(this);

  ui_ic_chat_name = new QLineEdit(this);
  ui_ic_chat_name->setFrame(false);
  ui_ic_chat_name->setPlaceholderText(tr("Showname"));

  ui_ic_chat_message = new QLineEdit(this);
  ui_ic_chat_message->setFrame(false);
  ui_ic_chat_message->setPlaceholderText(tr("Message"));

  ui_muted = new AOImage(ui_ic_chat_message, ao_app);
  ui_muted->hide();

  ui_ooc_chat_message = new QLineEdit(this);
  ui_ooc_chat_message->setFrame(false);

  ui_ooc_chat_name = new QLineEdit(this);
  ui_ooc_chat_name->setFrame(false);
  ui_ooc_chat_name->setPlaceholderText(tr("Name"));
  ui_ooc_chat_name->setMaxLength(30);
  ui_ooc_chat_name->setText(p_ao_app->get_default_username());

  //ui_area_password = new QLineEdit(this);
  //ui_area_password->setFrame(false);
  ui_music_search = new QLineEdit(this);
  ui_music_search->setFrame(false);
  ui_music_search->setPlaceholderText(tr("Search"));

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
  ui_pos_dropdown->addItem("jur");
  ui_pos_dropdown->addItem("sea");

  ui_defense_bar = new AOImage(this, ao_app);
  ui_prosecution_bar = new  AOImage(this, ao_app);

  ui_music_label = new QLabel(this);
  ui_sfx_label = new QLabel(this);
  ui_blip_label = new QLabel(this);

  ui_log_limit_label = new QLabel(this);

  ui_hold_it = new AOButton(this, ao_app);
  ui_objection = new AOButton(this, ao_app);
  ui_take_that = new AOButton(this, ao_app);

  ui_ooc_toggle = new AOButton(this, ao_app);
  ui_witness_testimony = new AOButton(this, ao_app);
  ui_cross_examination = new AOButton(this, ao_app);
  ui_guilty = new AOButton(this, ao_app);
  ui_not_guilty = new AOButton(this, ao_app);

  ui_change_character = new AOButton(this, ao_app);
  ui_reload_theme = new AOButton(this, ao_app);
  ui_call_mod = new AOButton(this, ao_app);
  ui_settings = new AOButton(this, ao_app);
  ui_announce_casing = new AOButton(this, ao_app);
  ui_switch_area_music = new AOButton(this, ao_app);

  ui_pre = new QCheckBox(this);
  ui_pre->setText(tr("Pre"));

  ui_flip = new QCheckBox(this);
  ui_flip->setText(tr("Flip"));
  ui_flip->hide();

  ui_guard = new QCheckBox(this);
  ui_guard->setText(tr("Guard"));
  ui_guard->hide();

  ui_casing = new QCheckBox(this);
  ui_casing->setChecked(ao_app->get_casing_enabled());
  ui_casing->setText(tr("Casing"));
  ui_casing->hide();

  ui_showname_enable = new QCheckBox(this);
  ui_showname_enable->setChecked(ao_app->get_showname_enabled_by_default());
  ui_showname_enable->setText(tr("Shownames"));

  ui_pre_non_interrupt = new QCheckBox(this);
  ui_pre_non_interrupt->setText(tr("No Interrupt"));
  ui_pre_non_interrupt->hide();

  ui_custom_objection = new AOButton(this, ao_app);
  ui_custom_objection->setContextMenuPolicy(Qt::CustomContextMenu);

  ui_realization = new AOButton(this, ao_app);
  ui_mute = new AOButton(this, ao_app);  

  ui_defense_plus = new AOButton(this, ao_app);
  ui_defense_minus = new AOButton(this, ao_app);

  ui_prosecution_plus = new AOButton(this, ao_app);
  ui_prosecution_minus = new AOButton(this, ao_app);

  ui_text_color = new QComboBox(this);
  ui_text_color->addItem(tr("White"));
  ui_text_color->addItem(tr("Green"));
  ui_text_color->addItem(tr("Red"));
  ui_text_color->addItem(tr("Orange"));
  ui_text_color->addItem(tr("Blue"));
  ui_text_color->addItem(tr("Yellow"));
  ui_text_color->addItem(tr("Rainbow"));
  ui_text_color->addItem(tr("Pink"));
  ui_text_color->addItem(tr("Cyan"));

  ui_music_slider = new QSlider(Qt::Horizontal, this);
  ui_music_slider->setRange(0, 100);
  ui_music_slider->setValue(ao_app->get_default_music());

  ui_sfx_slider = new QSlider(Qt::Horizontal, this);
  ui_sfx_slider->setRange(0, 100);
  ui_sfx_slider->setValue(ao_app->get_default_sfx());

  ui_blip_slider = new QSlider(Qt::Horizontal, this);
  ui_blip_slider->setRange(0, 100);
  ui_blip_slider->setValue(ao_app->get_default_blip());

  ui_log_limit_spinbox = new QSpinBox(this);
  ui_log_limit_spinbox->setRange(0, 10000);
  ui_log_limit_spinbox->setValue(ao_app->get_max_log_size());

  ui_mute_list = new QListWidget(this);

  ui_pair_list = new QListWidget(this);
  ui_pair_offset_spinbox = new QSpinBox(this);
  ui_pair_offset_spinbox->setRange(-100,100);
  ui_pair_offset_spinbox->setSuffix("% offset");
  ui_pair_button = new AOButton(this, ao_app);

  ui_evidence_button = new AOButton(this, ao_app);

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
  connect(ui_pos_dropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(on_pos_dropdown_changed(int)));

  connect(ui_mute_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_mute_list_clicked(QModelIndex)));

  connect(ui_ic_chat_message, SIGNAL(returnPressed()), this, SLOT(on_chat_return_pressed()));

  connect(ui_ooc_chat_message, SIGNAL(returnPressed()), this, SLOT(on_ooc_return_pressed()));

  connect(ui_music_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_music_list_double_clicked(QModelIndex)));
  connect(ui_area_list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_area_list_double_clicked(QModelIndex)));

  connect(ui_hold_it, SIGNAL(clicked()), this, SLOT(on_hold_it_clicked()));
  connect(ui_objection, SIGNAL(clicked()), this, SLOT(on_objection_clicked()));
  connect(ui_take_that, SIGNAL(clicked()), this, SLOT(on_take_that_clicked()));
  connect(ui_custom_objection, SIGNAL(clicked()), this, SLOT(on_custom_objection_clicked()));
  connect(ui_custom_objection, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

  connect(ui_realization, SIGNAL(clicked()), this, SLOT(on_realization_clicked()));

  connect(ui_mute, SIGNAL(clicked()), this, SLOT(on_mute_clicked()));

  connect(ui_defense_minus, SIGNAL(clicked()), this, SLOT(on_defense_minus_clicked()));
  connect(ui_defense_plus, SIGNAL(clicked()), this, SLOT(on_defense_plus_clicked()));
  connect(ui_prosecution_minus, SIGNAL(clicked()), this, SLOT(on_prosecution_minus_clicked()));
  connect(ui_prosecution_plus, SIGNAL(clicked()), this, SLOT(on_prosecution_plus_clicked()));

  connect(ui_text_color, SIGNAL(currentIndexChanged(int)), this, SLOT(on_text_color_changed(int)));

  connect(ui_music_slider, SIGNAL(valueChanged(int)), this, SLOT(on_music_slider_moved(int)));
  connect(ui_sfx_slider, SIGNAL(valueChanged(int)), this, SLOT(on_sfx_slider_moved(int)));
  connect(ui_blip_slider, SIGNAL(valueChanged(int)), this, SLOT(on_blip_slider_moved(int)));

  connect(ui_log_limit_spinbox, SIGNAL(valueChanged(int)), this, SLOT(on_log_limit_changed(int)));

  connect(ui_ooc_toggle, SIGNAL(clicked()), this, SLOT(on_ooc_toggle_clicked()));

  connect(ui_music_search, SIGNAL(textChanged(QString)), this, SLOT(on_music_search_edited(QString)));

  connect(ui_witness_testimony, SIGNAL(clicked()), this, SLOT(on_witness_testimony_clicked()));
  connect(ui_cross_examination, SIGNAL(clicked()), this, SLOT(on_cross_examination_clicked()));
  connect(ui_guilty, SIGNAL(clicked()), this, SLOT(on_guilty_clicked()));
  connect(ui_not_guilty, SIGNAL(clicked()), this, SLOT(on_not_guilty_clicked()));

  connect(ui_change_character, SIGNAL(clicked()), this, SLOT(on_change_character_clicked()));
  connect(ui_reload_theme, SIGNAL(clicked()), this, SLOT(on_reload_theme_clicked()));
  connect(ui_call_mod, SIGNAL(clicked()), this, SLOT(on_call_mod_clicked()));
  connect(ui_settings, SIGNAL(clicked()), this, SLOT(on_settings_clicked()));
  connect(ui_announce_casing, SIGNAL(clicked()), this, SLOT(on_announce_casing_clicked()));
  connect(ui_switch_area_music, SIGNAL(clicked()), this, SLOT(on_switch_area_music_clicked()));

  connect(ui_pre, SIGNAL(clicked()), this, SLOT(on_pre_clicked()));
  connect(ui_flip, SIGNAL(clicked()), this, SLOT(on_flip_clicked()));
  connect(ui_guard, SIGNAL(clicked()), this, SLOT(on_guard_clicked()));
  connect(ui_casing, SIGNAL(clicked()), this, SLOT(on_casing_clicked()));

  connect(ui_showname_enable, SIGNAL(clicked()), this, SLOT(on_showname_enable_clicked()));

  connect(ui_pair_button, SIGNAL(clicked()), this, SLOT(on_pair_clicked()));
  connect(ui_pair_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_pair_list_clicked(QModelIndex)));
  connect(ui_pair_offset_spinbox, SIGNAL(valueChanged(int)), this, SLOT(on_pair_offset_changed(int)));

  connect(ui_evidence_button, SIGNAL(clicked()), this, SLOT(on_evidence_button_clicked()));

  set_widgets();

  set_char_select();
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

void Courtroom::set_pair_list()
{
  QStringList sorted_pair_list;

  for (char_type i_char : char_list)
    sorted_pair_list.append(i_char.name);

  sorted_pair_list.sort();

  for (QString i_name : sorted_pair_list)
  {
    ui_pair_list->addItem(i_name);
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

  // If there is a point to it, show all CCCC features.
  // We also do this this soon so that set_size_and_pos can hide them all later, if needed.
  if (ao_app->cccc_ic_support_enabled)
  {
    ui_pair_button->show();
    ui_pre_non_interrupt->show();
    ui_showname_enable->show();
    ui_ic_chat_name->show();
    ui_ic_chat_name->setEnabled(true);
  }
  else
  {
    ui_pair_button->hide();
    ui_pre_non_interrupt->hide();
    ui_showname_enable->hide();
    ui_ic_chat_name->hide();
    ui_ic_chat_name->setEnabled(false);
  }

  if (ao_app->casing_alerts_enabled)
  {
    ui_announce_casing->show();
  }
  else
  {
    ui_announce_casing->hide();
  }

  // We also show the non-server-dependent client additions.
  // Once again, if the theme can't display it, set_move_and_pos will catch them.
  ui_settings->show();
  ui_log_limit_label->show();
  ui_log_limit_spinbox->show();

  ui_vp_background->move(0, 0);
  ui_vp_background->resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_speedlines->move(0, 0);
  ui_vp_speedlines->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_player_char->move(0, 0);
  ui_vp_player_char->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_sideplayer_char->move(0, 0);
  ui_vp_sideplayer_char->combo_resize(ui_viewport->width(), ui_viewport->height());

  //the AO2 desk element
  ui_vp_desk->move(0, 0);
  ui_vp_desk->resize(ui_viewport->width(), ui_viewport->height());

  //the size of the ui_vp_legacy_desk element relies on various factors and is set in set_scene()

  double y_modifier = 147.0 / 192.0;
  int final_y = static_cast<int>(y_modifier * ui_viewport->height());
  ui_vp_legacy_desk->move(0, final_y);
  ui_vp_legacy_desk->hide();

  ui_vp_evidence_display->move(0, 0);
  ui_vp_evidence_display->resize(ui_viewport->width(), ui_viewport->height());

  set_size_and_pos(ui_vp_showname, "showname");

  set_size_and_pos(ui_vp_message, "message");
  ui_vp_message->setTextInteractionFlags(Qt::NoTextInteraction);
  ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                               "color: white");

  ui_vp_testimony->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_testimony->resize(ui_viewport->width(), ui_viewport->height());
  ui_vp_testimony->set_image("testimony.png");
  ui_vp_testimony->hide();

  ui_vp_realization->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_realization->resize(ui_viewport->width(), ui_viewport->height());
  ui_vp_realization->set_image("realizationflash.png");
  ui_vp_realization->hide();

  ui_vp_wtce->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_wtce->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_objection->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_objection->combo_resize(ui_viewport->width(), ui_viewport->height());

  set_size_and_pos(ui_ic_chatlog, "ic_chatlog");

  set_size_and_pos(ui_ms_chatlog, "ms_chatlog");

  set_size_and_pos(ui_server_chatlog, "server_chatlog");

  set_size_and_pos(ui_mute_list, "mute_list");
  ui_mute_list->hide();

  set_size_and_pos(ui_pair_list, "pair_list");
  ui_pair_list->hide();
  set_size_and_pos(ui_pair_offset_spinbox, "pair_offset_spinbox");
  ui_pair_offset_spinbox->hide();
  set_size_and_pos(ui_pair_button, "pair_button");
  ui_pair_button->set_image("pair_button.png");

  set_size_and_pos(ui_area_list, "music_list");
  ui_area_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_music_list, "music_list");

  if (is_ao2_bg)
  {
    set_size_and_pos(ui_ic_chat_message, "ao2_ic_chat_message");
    set_size_and_pos(ui_vp_chatbox, "ao2_chatbox");
    set_size_and_pos(ui_ic_chat_name, "ao2_ic_chat_name");
  }
  else
  {
    set_size_and_pos(ui_ic_chat_message, "ic_chat_message");
    set_size_and_pos(ui_vp_chatbox, "chatbox");
    set_size_and_pos(ui_ic_chat_name, "ic_chat_name");
  }

  ui_ic_chat_message->setStyleSheet("QLineEdit{background-color: rgba(100, 100, 100, 255);}");
  ui_ic_chat_name->setStyleSheet("QLineEdit{background-color: rgba(180, 180, 180, 255);}");

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

  set_size_and_pos(ui_log_limit_label, "log_limit_label");
  ui_log_limit_label->setText("Log limit");

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

  set_size_and_pos(ui_guilty, "guilty");
  ui_guilty->set_image("guilty.png");
  set_size_and_pos(ui_not_guilty, "not_guilty");
  ui_not_guilty->set_image("notguilty.png");

  set_size_and_pos(ui_change_character, "change_character");
  ui_change_character->setText("Change character");

  set_size_and_pos(ui_reload_theme, "reload_theme");
  ui_reload_theme->setText("Reload theme");

  set_size_and_pos(ui_call_mod, "call_mod");
  ui_call_mod->setText("Call mod");

  set_size_and_pos(ui_settings, "settings");
  ui_settings->setText("Settings");

  set_size_and_pos(ui_announce_casing, "casing_button");
  ui_announce_casing->setText("Casing");

  set_size_and_pos(ui_switch_area_music, "switch_area_music");
  ui_switch_area_music->setText("A/M");

  set_size_and_pos(ui_pre, "pre");
  ui_pre->setText("Preanim");

  set_size_and_pos(ui_pre_non_interrupt, "pre_no_interrupt");
  set_size_and_pos(ui_flip, "flip");

  set_size_and_pos(ui_guard, "guard");

  set_size_and_pos(ui_casing, "casing");

  set_size_and_pos(ui_showname_enable, "showname_enable");

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

  set_size_and_pos(ui_log_limit_spinbox, "log_limit_spinbox");

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
  ui_back_to_lobby->setText(tr("Back to Lobby"));

  set_size_and_pos(ui_char_password, "char_password");

  set_size_and_pos(ui_char_buttons, "char_buttons");

  set_size_and_pos(ui_char_select_left, "char_select_left");
  ui_char_select_left->set_image("arrow_left.png");

  set_size_and_pos(ui_char_select_right, "char_select_right");
  ui_char_select_right->set_image("arrow_right.png");

  set_size_and_pos(ui_spectator, "spectator");
}

void Courtroom::set_fonts()
{
  set_font(ui_vp_showname, "showname");
  set_font(ui_vp_message, "message");
  set_font(ui_ic_chatlog, "ic_chatlog");
  set_font(ui_ms_chatlog, "ms_chatlog");
  set_font(ui_server_chatlog, "server_chatlog");
  set_font(ui_music_list, "music_list");
  set_font(ui_area_list, "music_list");

  // Set color of labels and checkboxes
  const QString design_file = "courtroom_fonts.ini";
  QColor f_color = ao_app->get_color("label_color", design_file);
  QString color_string = "color: rgba(" +
          QString::number(f_color.red()) + ", " +
          QString::number(f_color.green()) + ", " +
          QString::number(f_color.blue()) + ", 255); }";
  QString style_sheet_string = "QLabel {" + color_string + "}"
          "QCheckBox {" + color_string + "}";
  setStyleSheet(style_sheet_string);
}

void Courtroom::set_font(QWidget *widget, QString p_identifier)
{
  QString design_file = "courtroom_fonts.ini";
  int f_weight = ao_app->get_font_size(p_identifier, design_file);
  QString class_name = widget->metaObject()->className();

  QString fontt = ao_app->get_font_name(p_identifier + "_font", design_file);
   widget->setFont(QFont(fontt, f_weight));


  QColor f_color = ao_app->get_color(p_identifier + "_color", design_file);

  QString style_sheet_string = class_name + " { background-color: rgba(0, 0, 0, 0);\n" +
                                            "color: rgba(" +
                                             QString::number(f_color.red()) + ", " +
                                             QString::number(f_color.green()) + ", " +
                                             QString::number(f_color.blue()) + ", 255); }";

  widget->setStyleSheet(style_sheet_string);
}

void Courtroom::set_window_title(QString p_title)
{
  this->setWindowTitle(p_title);
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

  music_player->set_volume(0);
  sfx_player->set_volume(0);
  objection_player->set_volume(0);
  blip_player->set_volume(0);

  set_char_select_page();

  set_mute_list();
  set_pair_list();

  set_char_select();

  show();

  ui_spectator->show();
}

void Courtroom::set_background(QString p_background)
{
  testimony_in_progress = false;

  current_background = p_background;

  is_ao2_bg = file_exists(ao_app->get_background_path("defensedesk.png")) &&
              file_exists(ao_app->get_background_path("prosecutiondesk.png")) &&
              file_exists(ao_app->get_background_path("stand.png"));

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

  if (m_cid == -1)
  {
    if (ao_app->is_discord_enabled())
      ao_app->discord->state_spectate();
    f_char = "";
  }
  else
  {
    f_char = ao_app->get_char_name(char_list.at(m_cid).name);

    if (ao_app->is_discord_enabled())
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

  set_evidence_page();

  QString side = ao_app->get_char_side(f_char);

  if (side == "jud")
  {
    ui_witness_testimony->show();
    ui_cross_examination->show();
    ui_not_guilty->show();
    ui_guilty->show();
    ui_defense_minus->show();
    ui_defense_plus->show();
    ui_prosecution_minus->show();
    ui_prosecution_plus->show();
  }
  else
  {
    ui_witness_testimony->hide();
    ui_cross_examination->hide();
    ui_guilty->hide();
    ui_not_guilty->hide();
    ui_defense_minus->hide();
    ui_defense_plus->hide();
    ui_prosecution_minus->hide();
    ui_prosecution_plus->hide();
  }

  if (ao_app->custom_objection_enabled &&
      (file_exists(ao_app->get_character_path(current_char, "custom1.gif")) ||
      file_exists(ao_app->get_character_path(current_char, "custom1.apng"))) &&
      file_exists(ao_app->get_character_path(current_char, "custom1.wav")))
    ui_custom_objection->show();
  else
    ui_custom_objection->hide();

  if (ao_app->flipping_enabled)
    ui_flip->show();
  else
    ui_flip->hide();

  if (ao_app->casing_alerts_enabled)
    ui_casing->show();
  else
    ui_casing->hide();

  list_music();
  list_areas();

  music_player->set_volume(ui_music_slider->value());
  sfx_player->set_volume(ui_sfx_slider->value());
  objection_player->set_volume(ui_sfx_slider->value());
  blip_player->set_volume(ui_blip_slider->value());

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
  music_row_to_number.clear();

  QString f_file = "courtroom_design.ini";

  QBrush found_brush(ao_app->get_color("found_song_color", f_file));
  QBrush missing_brush(ao_app->get_color("missing_song_color", f_file));

  int n_listed_songs = 0;

  for (int n_song = 0 ; n_song < music_list.size() ; ++n_song)
  {
    QString i_song = music_list.at(n_song);
    QString i_song_listname = i_song;
    i_song_listname = i_song_listname.left(i_song_listname.lastIndexOf("."));

    if (i_song.toLower().contains(ui_music_search->text().toLower()))
    {
      ui_music_list->addItem(i_song_listname);
      music_row_to_number.append(n_song);

      QString song_path = ao_app->get_music_path(i_song);

      if (file_exists(song_path))
        ui_music_list->item(n_listed_songs)->setBackground(found_brush);
      else
        ui_music_list->item(n_listed_songs)->setBackground(missing_brush);

      ++n_listed_songs;
    }
  }
}

void Courtroom::list_areas()
{
  ui_area_list->clear();
  area_row_to_number.clear();

  QString f_file = "courtroom_design.ini";

  QBrush free_brush(ao_app->get_color("area_free_color", f_file));
  QBrush lfp_brush(ao_app->get_color("area_lfp_color", f_file));
  QBrush casing_brush(ao_app->get_color("area_casing_color", f_file));
  QBrush recess_brush(ao_app->get_color("area_recess_color", f_file));
  QBrush rp_brush(ao_app->get_color("area_rp_color", f_file));
  QBrush gaming_brush(ao_app->get_color("area_gaming_color", f_file));
  QBrush locked_brush(ao_app->get_color("area_locked_color", f_file));

  int n_listed_areas = 0;

  for (int n_area = 0 ; n_area < area_list.size() ; ++n_area)
  {
    QString i_area = "";
    i_area.append("[");
    i_area.append(QString::number(n_area));
    i_area.append("] ");

    i_area.append(area_list.at(n_area));

    if (ao_app->arup_enabled)
    {
      i_area.append("\n  ");

      i_area.append(arup_statuses.at(n_area));
      i_area.append(" | CM: ");
      i_area.append(arup_cms.at(n_area));

      i_area.append("\n  ");

      i_area.append(QString::number(arup_players.at(n_area)));
      i_area.append(" users | ");

      i_area.append(arup_locks.at(n_area));
    }

    if (i_area.toLower().contains(ui_music_search->text().toLower()))
    {
      ui_area_list->addItem(i_area);
      area_row_to_number.append(n_area);

      if (ao_app->arup_enabled)
      {
        // Colouring logic here.
        ui_area_list->item(n_listed_areas)->setBackground(free_brush);
        if (arup_locks.at(n_area) == "LOCKED")
        {
            ui_area_list->item(n_listed_areas)->setBackground(locked_brush);
        }
        else
        {
            if (arup_statuses.at(n_area) == "LOOKING-FOR-PLAYERS")
                ui_area_list->item(n_listed_areas)->setBackground(lfp_brush);
            else if (arup_statuses.at(n_area) == "CASING")
                ui_area_list->item(n_listed_areas)->setBackground(casing_brush);
            else if (arup_statuses.at(n_area) == "RECESS")
                ui_area_list->item(n_listed_areas)->setBackground(recess_brush);
            else if (arup_statuses.at(n_area) == "RP")
                ui_area_list->item(n_listed_areas)->setBackground(rp_brush);
            else if (arup_statuses.at(n_area) == "GAMING")
                ui_area_list->item(n_listed_areas)->setBackground(gaming_brush);
        }
      }
      else
      {
        ui_area_list->item(n_listed_areas)->setBackground(free_brush);
      }

      ++n_listed_areas;
    }
  }
}

void Courtroom::append_ms_chatmessage(QString f_name, QString f_message)
{
  ui_ms_chatlog->append_chatmessage(f_name, f_message, ao_app->get_color("ooc_default_color", "courtroom_design.ini").name());
}

void Courtroom::append_server_chatmessage(QString p_name, QString p_message, QString p_colour)
{
  QString colour = "#000000";

  if (p_colour == "0")
    colour = ao_app->get_color("ooc_default_color", "courtroom_design.ini").name();
  if (p_colour == "1")
    colour = ao_app->get_color("ooc_server_color", "courtroom_design.ini").name();

  ui_server_chatlog->append_chatmessage(p_name, p_message, colour);
}

void Courtroom::on_chat_return_pressed()
{
  if (ui_ic_chat_message->text() == "" || is_muted)
    return;

  if ((anim_state < 3 || text_state < 2) &&
      objection_state == 0)
    return;

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

  // Additionally, in our case:

  //showname#
  //other_charid#
  //self_offset#
  //noninterrupting_preanim#%

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

  packet_contents.append(ao_app->get_sfx_name(current_char, current_emote));

  int f_emote_mod = ao_app->get_emote_mod(current_char, current_emote);

  //needed or else legacy won't understand what we're saying
  if (objection_state > 0)
  {
    if (ui_pre->isChecked())
    {
      if (f_emote_mod == 5)
        f_emote_mod = 6;
      else
        f_emote_mod = 2;
    }
  }
  else if (ui_pre->isChecked() and !ui_pre_non_interrupt->isChecked())
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

  if ((objection_state == 4 && !ao_app->custom_objection_enabled) ||
    (objection_state < 0))
    f_obj_state = "0";
  else
    f_obj_state = QString::number(objection_state);

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

  packet_contents.append(QString::number(realization_state));

  QString f_text_color;

  if (text_color < 0)
    f_text_color = "0";
  else if (text_color > 8)
    f_text_color = "0";
  else
    f_text_color = QString::number(text_color);

  packet_contents.append(f_text_color);

  // If the server we're on supports CCCC stuff, we should use it!
  if (ao_app->cccc_ic_support_enabled)
  {
    // If there is a showname entered, use that -- else, just send an empty packet-part.
    if (!ui_ic_chat_name->text().isEmpty())
    {
      packet_contents.append(ui_ic_chat_name->text());
    }
    else
    {
      packet_contents.append("");
    }

    // Similarly, we send over whom we're paired with, unless we have chosen ourselves.
    // Or a charid of -1 or lower, through some means.
    if (other_charid > -1 && other_charid != m_cid)
    {
      packet_contents.append(QString::number(other_charid));
      packet_contents.append(QString::number(offset_with_pair));
    }
    else
    {
      packet_contents.append("-1");
      packet_contents.append("0");
    }

    // Finally, we send over if we want our pres to not interrupt.
    if (ui_pre_non_interrupt->isChecked() && ui_pre->isChecked())
    {
      packet_contents.append("1");
    }
    else
    {
      packet_contents.append("0");
    }
  }

  ao_app->send_server_packet(new AOPacket("MS", packet_contents));
}

void Courtroom::handle_chatmessage(QStringList *p_contents)
{
  // Instead of checking for whether a message has at least chatmessage_size
  // amount of packages, we'll check if it has at least 15.
  // That was the original chatmessage_size.
  if (p_contents->size() < 15)
    return;

  for (int n_string = 0 ; n_string < chatmessage_size ; ++n_string)
  {
    //m_chatmessage[n_string] = p_contents->at(n_string);

    // Note that we have added stuff that vanilla clients and servers simply won't send.
    // So now, we have to check if the thing we want even exists amongst the packet's content.
    // We also have to check if the server even supports CCCC's IC features, or if it's just japing us.
    // Also, don't forget! A size 15 message will have indices from 0 to 14.
    if (n_string < p_contents->size() &&
       (n_string < 15 || ao_app->cccc_ic_support_enabled))
    {
      m_chatmessage[n_string] = p_contents->at(n_string);
    }
    else
    {
      m_chatmessage[n_string] = "";
    }
  }

  int f_char_id = m_chatmessage[CHAR_ID].toInt();

  if (f_char_id < 0 || f_char_id >= char_list.size())
    return;

  if (mute_map.value(m_chatmessage[CHAR_ID].toInt()))
    return;

  QString f_showname;
  if (m_chatmessage[SHOWNAME].isEmpty() || !ui_showname_enable->isChecked())
  {
      f_showname = ao_app->get_showname(char_list.at(f_char_id).name);
  }
  else
  {
      f_showname = m_chatmessage[SHOWNAME];
  }


  QString f_message = f_showname + ": " + m_chatmessage[MESSAGE] + '\n';

  if (f_message == previous_ic_message)
    return;

  text_state = 0;
  anim_state = 0;
  ui_vp_objection->stop();
  ui_vp_player_char->stop();
  chat_tick_timer->stop();
  ui_vp_evidence_display->reset();

  chatmessage_is_empty = m_chatmessage[MESSAGE] == " " || m_chatmessage[MESSAGE] == "";

  if (m_chatmessage[MESSAGE] == ui_ic_chat_message->text() && m_chatmessage[CHAR_ID].toInt() == m_cid)
  {
    ui_ic_chat_message->clear();
    objection_state = 0;
    realization_state = 0;
    is_presenting_evidence = false;
    ui_pre->setChecked(false);
    ui_hold_it->set_image("holdit.png");
    ui_objection->set_image("objection.png");
    ui_take_that->set_image("takethat.png");
    ui_custom_objection->set_image("custom.png");
    ui_realization->set_image("realization.png");
    ui_evidence_present->set_image("present_disabled.png");
  }

  chatlogpiece* temp = new chatlogpiece(ao_app->get_showname(char_list.at(f_char_id).name), f_showname, ": " + m_chatmessage[MESSAGE], false);
  ic_chatlog_history.append(*temp);

  while(ic_chatlog_history.size() > log_maximum_blocks && log_maximum_blocks > 0)
  {
    ic_chatlog_history.removeFirst();
  }

  append_ic_text(": " + m_chatmessage[MESSAGE], f_showname);

  previous_ic_message = f_message;

  int objection_mod = m_chatmessage[OBJECTION_MOD].toInt();
  QString f_char = m_chatmessage[CHAR_NAME];
  QString f_custom_theme = ao_app->get_char_shouts(f_char);

  //if an objection is used
  if (objection_mod >= 1)
  {
    switch (objection_mod)
    {
    case 1:
      ui_vp_objection->play("holdit", f_char, f_custom_theme);
      objection_player->play("holdit.wav", f_char, f_custom_theme);
      break;
    case 2:
      ui_vp_objection->play("objection", f_char, f_custom_theme);
      objection_player->play("objection.wav", f_char, f_custom_theme);
      break;
    case 3:
      ui_vp_objection->play("takethat", f_char, f_custom_theme);
      objection_player->play("takethat.wav", f_char, f_custom_theme);
      break;
    default:
      if(objection_mod > 39) //checks if there is custom objection as it starts from code 40 and up
      {
          //Incase of multiple custom objections,the flag is set to 41,42 etc so we make it as "custom1","custom2"
          QString objection_mod_custom = QString::number(objection_mod - 40);
          ui_vp_objection->play("custom" + objection_mod_custom, f_char, f_custom_theme);
          objection_player->play("custom" +objection_mod_custom +".wav", f_char, f_custom_theme);
      }
      else
      qDebug() << "W: Logic error in objection switch statement!";
    }

    int emote_mod = m_chatmessage[EMOTE_MOD].toInt();

    if (emote_mod == 0)
      m_chatmessage[EMOTE_MOD] = 1;
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

  if (m_chatmessage[SHOWNAME].isEmpty() || !ui_showname_enable->isChecked())
  {
      QString real_name = char_list.at(m_chatmessage[CHAR_ID].toInt()).name;

      QString f_showname = ao_app->get_showname(real_name);

      ui_vp_showname->setText(f_showname);
  }
  else
  {
      ui_vp_showname->setText(m_chatmessage[SHOWNAME]);
  }

  ui_vp_message->clear();
  ui_vp_chatbox->hide();

  QString chatbox = ao_app->get_chat(m_chatmessage[CHAR_NAME]);

  if (chatbox == "")
    ui_vp_chatbox->set_image("chatmed.png");
  else
  {
    QString chatbox_path = ao_app->get_base_path() + "misc/" + chatbox + "/chatbox.png";
    ui_vp_chatbox->set_image_from_path(chatbox_path);
  }

  ui_vp_showname->setStyleSheet("QLabel { color : " + get_text_color("_showname").name() + "; }");

  set_scene();
  set_text_color();

  // Check if the message needs to be centered.
  QString f_message = m_chatmessage[MESSAGE];
  if (f_message.size() >= 2)
  {
      if (f_message.startsWith("~~"))
      {
          message_is_centered = true;
      }
      else
      {
          message_is_centered = false;
      }
  }
  else
  {
      ui_vp_message->setAlignment(Qt::AlignLeft);
  }


  int emote_mod = m_chatmessage[EMOTE_MOD].toInt();

  if (ao_app->flipping_enabled && m_chatmessage[FLIP].toInt() == 1)
    ui_vp_player_char->set_flipped(true);
  else
    ui_vp_player_char->set_flipped(false);

  QString side = m_chatmessage[SIDE];

  // Making the second character appear.
  if (m_chatmessage[OTHER_CHARID].isEmpty())
  {
    // If there is no second character, hide 'em, and center the first.
    ui_vp_sideplayer_char->hide();
    ui_vp_sideplayer_char->move(0,0);

    ui_vp_player_char->move(0,0);
  }
  else
  {
    bool ok;
    int got_other_charid = m_chatmessage[OTHER_CHARID].toInt(&ok);
    if (ok)
    {
      if (got_other_charid > -1)
      {
        // If there is, show them!
        ui_vp_sideplayer_char->show();

        // Depending on where we are, we offset the characters, and reorder their stacking.
        if (side == "def")
        {
          // We also move the character down depending on how far the are to the right.
          int hor_offset = m_chatmessage[SELF_OFFSET].toInt();
          int vert_offset = 0;
          if (hor_offset > 0)
          {
            vert_offset = hor_offset / 10;
          }
          ui_vp_player_char->move(ui_viewport->width() * hor_offset / 100, ui_viewport->height() * vert_offset / 100);

          // We do the same with the second character.
          int hor2_offset = m_chatmessage[OTHER_OFFSET].toInt();
          int vert2_offset = 0;
          if (hor2_offset > 0)
          {
            vert2_offset = hor2_offset / 10;
          }
          ui_vp_sideplayer_char->move(ui_viewport->width() * hor2_offset / 100, ui_viewport->height() * vert2_offset / 100);

          // Finally, we reorder them based on who is more to the left.
          // The person more to the left is more in the front.
          if (hor2_offset >= hor_offset)
          {
            ui_vp_sideplayer_char->raise();
            ui_vp_player_char->raise();
          }
          else
          {
            ui_vp_player_char->raise();
            ui_vp_sideplayer_char->raise();
          }
          ui_vp_desk->raise();
          ui_vp_legacy_desk->raise();
        }
        else if (side == "pro")
        {
          // Almost the same thing happens here, but in reverse.
          int hor_offset = m_chatmessage[SELF_OFFSET].toInt();
          int vert_offset = 0;
          if (hor_offset < 0)
          {
            // We don't want to RAISE the char off the floor.
            vert_offset = -1 * hor_offset / 10;
          }
          ui_vp_player_char->move(ui_viewport->width() * hor_offset / 100, ui_viewport->height() * vert_offset / 100);

          // We do the same with the second character.
          int hor2_offset = m_chatmessage[OTHER_OFFSET].toInt();
          int vert2_offset = 0;
          if (hor2_offset < 0)
          {
            vert2_offset = -1 * hor2_offset / 10;
          }
          ui_vp_sideplayer_char->move(ui_viewport->width() * hor2_offset / 100, ui_viewport->height() * vert2_offset / 100);

          // Finally, we reorder them based on who is more to the right.
          if (hor2_offset <= hor_offset)
          {
            ui_vp_sideplayer_char->raise();
            ui_vp_player_char->raise();
          }
          else
          {
            ui_vp_player_char->raise();
            ui_vp_sideplayer_char->raise();
          }
          ui_vp_desk->raise();
          ui_vp_legacy_desk->raise();
        }
        else
        {
          // In every other case, the person more to the left is on top.
          // These cases also don't move the characters down.
          int hor_offset = m_chatmessage[SELF_OFFSET].toInt();
          ui_vp_player_char->move(ui_viewport->width() * hor_offset / 100, 0);

          // We do the same with the second character.
          int hor2_offset = m_chatmessage[OTHER_OFFSET].toInt();
          ui_vp_sideplayer_char->move(ui_viewport->width() * hor2_offset / 100, 0);

          // Finally, we reorder them based on who is more to the left.
          // The person more to the left is more in the front.
          if (hor2_offset >= hor_offset)
          {
            ui_vp_sideplayer_char->raise();
            ui_vp_player_char->raise();
          }
          else
          {
            ui_vp_player_char->raise();
            ui_vp_sideplayer_char->raise();
          }
          ui_vp_desk->raise();
          ui_vp_legacy_desk->raise();
        }
        // We should probably also play the other character's idle emote.
        if (ao_app->flipping_enabled && m_chatmessage[OTHER_FLIP].toInt() == 1)
          ui_vp_sideplayer_char->set_flipped(true);
        else
          ui_vp_sideplayer_char->set_flipped(false);
        ui_vp_sideplayer_char->play_idle(m_chatmessage[OTHER_NAME], m_chatmessage[OTHER_EMOTE]);
      }
      else
      {
          // If the server understands other characters, but there
          // really is no second character, hide 'em, and center the first.
          ui_vp_sideplayer_char->hide();
          ui_vp_sideplayer_char->move(0,0);

          ui_vp_player_char->move(0,0);
      }
    }
  }

  switch (emote_mod)
  {
  case 1: case 2: case 6:
    play_preanim(false);
    break;
  case 0: case 5:
    if (m_chatmessage[NONINTERRUPTING_PRE].toInt() == 0)
      handle_chatmessage_3();
    else
      play_preanim(true);
    break;
  default:
    qDebug() << "W: invalid emote mod: " << QString::number(emote_mod);
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
    bool is_left_side = !(f_side == "def" || f_side == "hlp" || f_side == "jud" || f_side == "jur");
    ui_vp_evidence_display->show_evidence(f_image, is_left_side, ui_sfx_slider->value());
  }

  int emote_mod = m_chatmessage[EMOTE_MOD].toInt();

  QString side = m_chatmessage[SIDE];

  if (emote_mod == 5 ||
      emote_mod == 6)
  {
    ui_vp_desk->hide();
    ui_vp_legacy_desk->hide();

    // Since we're zooming, hide the second character, and centre the first.
    ui_vp_sideplayer_char->hide(); 
    ui_vp_player_char->move(0,0);

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
  {
    //talking
    f_anim_state = 2;
    entire_message_is_blue = false;
  }
  else
  {
    //idle
    f_anim_state = 3;
    entire_message_is_blue = true;
  }

  if (f_anim_state <= anim_state)
    return;

  ui_vp_player_char->stop();
  QString f_char = m_chatmessage[CHAR_NAME];
  QString f_emote = m_chatmessage[EMOTE];

  if (f_anim_state == 2) {
    ui_vp_player_char->play_talking(f_char, f_emote);
    anim_state = 2;
  }
  else
  {
    ui_vp_player_char->play_idle(f_char, f_emote);
    anim_state = 3;
  }

  QString f_message = m_chatmessage[MESSAGE];
  QStringList call_words = ao_app->get_call_words();

  for (QString word : call_words)
  {
    if (f_message.contains(word, Qt::CaseInsensitive))
    {
      modcall_player->play(ao_app->get_sfx("word_call"));
      ao_app->alert(this);

      break;
    }
  }

}

QString Courtroom::filter_ic_text(QString p_text)
{
  // Get rid of centering.
  if(p_text.startsWith(": ~~"))
  {
      // Don't forget, the p_text part actually everything after the name!
      // Hence why we check for ': ~~'.

      // Let's remove those two tildes, then.
      // : _ ~ ~
      // 0 1 2 3
      p_text.remove(2,2);
  }

  // Get rid of the inline-colouring.
  // I know, I know, excessive code duplication.
  // Nobody looks in here, I'm fine.
  int trick_check_pos = 0;
  bool ic_next_is_not_special = false;
  QString f_character = p_text.at(trick_check_pos);
  std::stack<INLINE_COLOURS> ic_colour_stack;
  while (trick_check_pos < p_text.size())
  {
      f_character = p_text.at(trick_check_pos);

      // Escape character.
      if (f_character == "\\" and !ic_next_is_not_special)
      {
          ic_next_is_not_special = true;
          p_text.remove(trick_check_pos,1);
      }

      // Text speed modifier.
      else if (f_character == "{" and !ic_next_is_not_special)
      {
          p_text.remove(trick_check_pos,1);
      }
      else if (f_character == "}" and !ic_next_is_not_special)
      {
          p_text.remove(trick_check_pos,1);
      }

      // Orange inline colourisation.
      else if (f_character == "|" and !ic_next_is_not_special)
      {
          if (!ic_colour_stack.empty())
          {
              if (ic_colour_stack.top() == INLINE_ORANGE)
              {
                  ic_colour_stack.pop();
                  p_text.remove(trick_check_pos,1);
              }
              else
              {
                  ic_colour_stack.push(INLINE_ORANGE);
                  p_text.remove(trick_check_pos,1);
              }
          }
          else
          {
              ic_colour_stack.push(INLINE_ORANGE);
              p_text.remove(trick_check_pos,1);
          }
      }

      // Blue inline colourisation.
      else if (f_character == "(" and !ic_next_is_not_special)
      {
          ic_colour_stack.push(INLINE_BLUE);
          trick_check_pos++;
      }
      else if (f_character == ")" and !ic_next_is_not_special
               and !ic_colour_stack.empty())
      {
          if (ic_colour_stack.top() == INLINE_BLUE)
          {
              ic_colour_stack.pop();
              trick_check_pos++;
          }
          else
          {
              ic_next_is_not_special = true;
          }
      }

      // Grey inline colourisation.
      else if (f_character == "[" and !ic_next_is_not_special)
      {
          ic_colour_stack.push(INLINE_GREY);
          trick_check_pos++;
      }
      else if (f_character == "]" and !ic_next_is_not_special
               and !ic_colour_stack.empty())
      {
          if (ic_colour_stack.top() == INLINE_GREY)
          {
              ic_colour_stack.pop();
              trick_check_pos++;
          }
          else
          {
              ic_next_is_not_special = true;
          }
      }

      // Green inline colourisation.
      else if (f_character == "`" and !ic_next_is_not_special)
      {
          if (!ic_colour_stack.empty())
          {
              if (ic_colour_stack.top() == INLINE_GREEN)
              {
                  ic_colour_stack.pop();
                  p_text.remove(trick_check_pos,1);
              }
              else
              {
                  ic_colour_stack.push(INLINE_GREEN);
                  p_text.remove(trick_check_pos,1);
              }
          }
          else
          {
              ic_colour_stack.push(INLINE_GREEN);
              p_text.remove(trick_check_pos,1);
          }
      }
      else
      {
          trick_check_pos++;
          ic_next_is_not_special = false;
      }
  }

  return p_text;
}

void Courtroom::append_ic_text(QString p_text, QString p_name, bool is_songchange)
{
  QTextCharFormat bold;
  QTextCharFormat normal;
  QTextCharFormat italics;
  bold.setFontWeight(QFont::Bold);
  normal.setFontWeight(QFont::Normal);
  italics.setFontItalic(true);
  const QTextCursor old_cursor = ui_ic_chatlog->textCursor();
  const int old_scrollbar_value = ui_ic_chatlog->verticalScrollBar()->value();

  if (!is_songchange)
    p_text = filter_ic_text(p_text);

  if (log_goes_downwards)
  {
      const bool is_scrolled_down = old_scrollbar_value == ui_ic_chatlog->verticalScrollBar()->maximum();

      ui_ic_chatlog->moveCursor(QTextCursor::End);

      if (!first_message_sent)
      {
          ui_ic_chatlog->textCursor().insertText(p_name, bold);
          first_message_sent = true;
      }
      else
      {
          ui_ic_chatlog->textCursor().insertText('\n' + p_name, bold);
      }

      if (is_songchange)
      {
        ui_ic_chatlog->textCursor().insertText(" has played a song: ", normal);
        ui_ic_chatlog->textCursor().insertText(p_text + ".", italics);
      }
      else
      {
        ui_ic_chatlog->textCursor().insertText(p_text, normal);
      }

      // If we got too many blocks in the current log, delete some from the top.
      while (ui_ic_chatlog->document()->blockCount() > log_maximum_blocks && log_maximum_blocks > 0)
      {
          ui_ic_chatlog->moveCursor(QTextCursor::Start);
          ui_ic_chatlog->textCursor().select(QTextCursor::BlockUnderCursor);
          ui_ic_chatlog->textCursor().removeSelectedText();
          ui_ic_chatlog->textCursor().deleteChar();
          //qDebug() << ui_ic_chatlog->document()->blockCount() << " < " << log_maximum_blocks;
      }

      if (old_cursor.hasSelection() || !is_scrolled_down)
      {
          // The user has selected text or scrolled away from the bottom: maintain position.
          ui_ic_chatlog->setTextCursor(old_cursor);
          ui_ic_chatlog->verticalScrollBar()->setValue(old_scrollbar_value);
      }
      else
      {
          // The user hasn't selected any text and the scrollbar is at the bottom: scroll to the bottom.
          ui_ic_chatlog->moveCursor(QTextCursor::End);
          ui_ic_chatlog->verticalScrollBar()->setValue(ui_ic_chatlog->verticalScrollBar()->maximum());
      }
  }
  else
  {
      const bool is_scrolled_up = old_scrollbar_value == ui_ic_chatlog->verticalScrollBar()->minimum();

      ui_ic_chatlog->moveCursor(QTextCursor::Start);

      ui_ic_chatlog->textCursor().insertText(p_name, bold);

      if (is_songchange)
      {
        ui_ic_chatlog->textCursor().insertText(" has played a song: ", normal);
        ui_ic_chatlog->textCursor().insertText(p_text + "." + '\n', italics);
      }
      else
      {
        ui_ic_chatlog->textCursor().insertText(p_text + '\n', normal);
      }

      // If we got too many blocks in the current log, delete some from the bottom.
      while (ui_ic_chatlog->document()->blockCount() > log_maximum_blocks && log_maximum_blocks > 0)
      {
          ui_ic_chatlog->moveCursor(QTextCursor::End);
          ui_ic_chatlog->textCursor().select(QTextCursor::BlockUnderCursor);
          ui_ic_chatlog->textCursor().removeSelectedText();
          ui_ic_chatlog->textCursor().deletePreviousChar();
          //qDebug() << ui_ic_chatlog->document()->blockCount() << " < " << log_maximum_blocks;
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
          ui_ic_chatlog->verticalScrollBar()->setValue(ui_ic_chatlog->verticalScrollBar()->minimum());
      }
  }
}

void Courtroom::play_preanim(bool noninterrupting)
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
  QString anim_to_find = ao_app->get_image_suffix(ao_app->get_character_path(f_char, f_preanim));
  if (!file_exists(anim_to_find) ||
      preanim_duration < 0)
  {
    if (noninterrupting)
      anim_state = 4;
    else
      anim_state = 1;
    preanim_done();
    qDebug() << "could not find " + anim_to_find;
    return;
  }

  ui_vp_player_char->play_pre(f_char, f_preanim, preanim_duration);

  if (noninterrupting)
    anim_state = 4;
  else
    anim_state = 1;

  if (text_delay >= 0)
    text_delay_timer->start(text_delay);

  if (noninterrupting)
    handle_chatmessage_3();
}

void Courtroom::preanim_done()
{
  anim_state = 1;
  handle_chatmessage_3();
}

void Courtroom::realization_done()
{
  ui_vp_realization->hide();
}

void Courtroom::start_chat_ticking()
{
  //we need to ensure that the text isn't already ticking because this function can be called by two logic paths
  if (text_state != 0)
    return;

  if (m_chatmessage[REALIZATION] == "1")
  {
    realization_timer->start(60);
    ui_vp_realization->show();
    sfx_player->play(ao_app->get_custom_realization(m_chatmessage[CHAR_NAME]));
  }

  ui_vp_message->clear();
  set_text_color();
  rainbow_counter = 0;

  if (chatmessage_is_empty)
  {
    //since the message is empty, it's technically done ticking
    text_state = 2;
    return;
  }

  // At this point, we'd do well to clear the inline colour stack.
  // This stops it from flowing into next messages.
  while (!inline_colour_stack.empty())
  {
      inline_colour_stack.pop();
  }

  ui_vp_chatbox->show();

  tick_pos = 0;
  blip_pos = 0;

  // Just in case we somehow got inline blue text left over from a previous message,
  // let's set it to false.
  inline_blue_depth = 0;

  // At the start of every new message, we set the text speed to the default.
  current_display_speed = 3;
  chat_tick_timer->start(message_display_speed[current_display_speed]);

  QString f_gender = ao_app->get_gender(m_chatmessage[CHAR_NAME]);

  blip_player->set_blips(ao_app->get_sfx_suffix("sfx-blip" + f_gender));

  //means text is currently ticking
  text_state = 1;
}

void Courtroom::chat_tick()
{
  //note: this is called fairly often(every 60 ms when char is talking)
  //do not perform heavy operations here

  QString f_message = m_chatmessage[MESSAGE];
  f_message.remove(0, tick_pos);

  // Due to our new text speed system, we always need to stop the timer now.
  chat_tick_timer->stop();

  // Stops blips from playing when we have a formatting option.
  bool formatting_char = false;

  // If previously, we have detected that the message is centered, now
  // is the time to remove those two tildes at the start.
  if (message_is_centered)
  {
    f_message.remove(0,2);
  }

  if (f_message.size() == 0)
  {
    text_state = 2;
    if (anim_state != 4)
    {
      anim_state = 3;
      ui_vp_player_char->play_idle(m_chatmessage[CHAR_NAME], m_chatmessage[EMOTE]);
    }
  }

  else
  {
    QTextBoundaryFinder tbf(QTextBoundaryFinder::Grapheme, f_message);
    QString f_character;
    int f_char_length;

    tbf.toNextBoundary();

    if (tbf.position() == -1)
      f_character = f_message;
    else
      f_character = f_message.left(tbf.position());

    f_char_length = f_character.length();
    f_character = f_character.toHtmlEscaped();


    if (f_character == " ")
      ui_vp_message->insertPlainText(" ");

    // Escape character.
    else if (f_character == "\\" and !next_character_is_not_special)
    {
        next_character_is_not_special = true;
        formatting_char = true;
    }

    // Text speed modifier.
    else if (f_character == "{" and !next_character_is_not_special)
    {
        // ++, because it INCREASES delay!
        current_display_speed++;
        formatting_char = true;
    }
    else if (f_character == "}" and !next_character_is_not_special)
    {
        current_display_speed--;
        formatting_char = true;
    }

    // Orange inline colourisation.
    else if (f_character == "|" and !next_character_is_not_special)
    {
        if (!inline_colour_stack.empty())
        {
            if (inline_colour_stack.top() == INLINE_ORANGE)
            {
                inline_colour_stack.pop();
            }
            else
            {
                inline_colour_stack.push(INLINE_ORANGE);
            }
        }
        else
        {
            inline_colour_stack.push(INLINE_ORANGE);
        }
        formatting_char = true;
    }

    // Blue inline colourisation.
    else if (f_character == "(" and !next_character_is_not_special)
    {
        inline_colour_stack.push(INLINE_BLUE);
        ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(BLUE)).name() +"\">" + f_character + "</font>");

        // Increase how deep we are in inline blues.
        inline_blue_depth++;

        // Here, we check if the entire message is blue.
        // If it isn't, we stop talking.
        if (!entire_message_is_blue and anim_state != 4)
        {
          QString f_char = m_chatmessage[CHAR_NAME];
          QString f_emote = m_chatmessage[EMOTE];
          ui_vp_player_char->play_idle(f_char, f_emote);
        }
    }
    else if (f_character == ")" and !next_character_is_not_special
             and !inline_colour_stack.empty())
    {
        if (inline_colour_stack.top() == INLINE_BLUE)
        {
            inline_colour_stack.pop();
            ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(BLUE)).name() +"\">" + f_character + "</font>");

            // Decrease how deep we are in inline blues.
            // Just in case, we do a check if we're above zero, but we should be.
            if (inline_blue_depth > 0)
            {
              inline_blue_depth--;
              // Here, we check if the entire message is blue.
              // If it isn't, we start talking if we have completely climbed out of inline blues.
              if (!entire_message_is_blue)
              {
                // We should only go back to talking if we're out of inline blues, not during a non. int. pre, and not on the last character.
                if (inline_blue_depth == 0 and anim_state != 4 and !(tick_pos+1 >= f_message.size()))
                {
                  QString f_char = m_chatmessage[CHAR_NAME];
                  QString f_emote = m_chatmessage[EMOTE];
                  ui_vp_player_char->play_talking(f_char, f_emote);
                }
              }
            }
        }
        else
        {
            next_character_is_not_special = true;
            tick_pos -= f_char_length;
        }
    }

    // Grey inline colourisation.
    else if (f_character == "[" and !next_character_is_not_special)
    {
        inline_colour_stack.push(INLINE_GREY);
        ui_vp_message->insertHtml("<font color=\""+ get_text_color("_inline_grey").name() +"\">" + f_character + "</font>");
    }
    else if (f_character == "]" and !next_character_is_not_special
             and !inline_colour_stack.empty())
    {
        if (inline_colour_stack.top() == INLINE_GREY)
        {
            inline_colour_stack.pop();
            ui_vp_message->insertHtml("<font color=\""+ get_text_color("_inline_grey").name() +"\">" + f_character + "</font>");
        }
        else
        {
            next_character_is_not_special = true;
            tick_pos -= f_char_length;
        }
    }

    // Green inline colourisation.
    else if (f_character == "`" and !next_character_is_not_special)
    {
        if (!inline_colour_stack.empty())
        {
            if (inline_colour_stack.top() == INLINE_GREEN)
            {
                inline_colour_stack.pop();
                formatting_char = true;
            }
            else
            {
                inline_colour_stack.push(INLINE_GREEN);
                formatting_char = true;
            }
        }
        else
        {
            inline_colour_stack.push(INLINE_GREEN);
            formatting_char = true;
        }
    }
    else
    {
      next_character_is_not_special = false;
      if (!inline_colour_stack.empty())
      {
          switch (inline_colour_stack.top()) {
          case INLINE_ORANGE:
              ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(ORANGE)).name() +"\">" + f_character + "</font>");
              break;
          case INLINE_BLUE:
              ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(BLUE)).name() +"\">" + f_character + "</font>");
              break;
          case INLINE_GREEN:
              ui_vp_message->insertHtml("<font color=\""+ get_text_color(QString::number(GREEN)).name() +"\">" + f_character + "</font>");
              break;
          case INLINE_GREY:
              ui_vp_message->insertHtml("<font color=\""+ get_text_color("_inline_grey").name() +"\">" + f_character + "</font>");
              break;
          }
      }
      else
      {
        if (m_chatmessage[TEXT_COLOR].toInt() == RAINBOW)
        {
          QString html_color;

          switch (rainbow_counter)
          {
          case 0:
            html_color = get_text_color(QString::number(RED)).name();
            break;
          case 1:
            html_color = get_text_color(QString::number(ORANGE)).name();
            break;
          case 2:
            html_color = get_text_color(QString::number(YELLOW)).name();
            break;
          case 3:
            html_color = get_text_color(QString::number(GREEN)).name();
            break;
          default:
            html_color = get_text_color(QString::number(BLUE)).name();
            rainbow_counter = -1;
          }

          ++rainbow_counter;

          ui_vp_message->insertHtml("<font color=\"" + html_color + "\">" + f_character + "</font>");
        }
        else
          ui_vp_message->insertHtml(f_character);
      }

      if (message_is_centered)
      {
          ui_vp_message->setAlignment(Qt::AlignCenter);
      }
      else
      {
          ui_vp_message->setAlignment(Qt::AlignLeft);
      }
    }

    QScrollBar *scroll = ui_vp_message->verticalScrollBar();
    scroll->setValue(scroll->maximum());

    if(blank_blip)
      qDebug() << "blank_blip found true";

    if (f_character != ' ' || blank_blip)
    {

      if (blip_pos % blip_rate == 0 && !formatting_char)
      {
        blip_pos = 0;
        blip_player->blip_tick();
      }

      ++blip_pos;
    }

    tick_pos += f_char_length;

    // Restart the timer, but according to the newly set speeds, if there were any.
    // Keep the speed at bay.
    if (current_display_speed < 0)
    {
        current_display_speed = 0;
    }

    if (current_display_speed > 6)
    {
        current_display_speed = 6;
    }

    // If we had a formatting char, we shouldn't wait so long again, as it won't appear!
    if (formatting_char)
    {
        chat_tick_timer->start(1);
    }
    else
    {
        chat_tick_timer->start(message_display_speed[current_display_speed]);
    }

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

  sfx_player->play(ao_app->get_sfx_suffix(sfx_name));
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
  else if (f_side == "jur" && (file_exists(ao_app->get_background_path("jurystand.png")) ||
                               file_exists(ao_app->get_background_path("jurystand.gif"))))
  {
    f_background = "jurystand";
    f_desk_image = "jurydesk";
  }
  else if (f_side == "sea" && (file_exists(ao_app->get_background_path("seancestand.png")) ||
                               file_exists(ao_app->get_background_path("seancestand.gif"))))
  {
    f_background = "seancestand";
    f_desk_image = "seancedesk";
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
  QColor textcolor = ao_app->get_chat_color(m_chatmessage[TEXT_COLOR], ao_app->get_chat(m_chatmessage[CHAR_NAME]));

  ui_vp_message->setTextBackgroundColor(QColor(0,0,0,0));
  ui_vp_message->setTextColor(textcolor);

  QString style = "background-color: rgba(0, 0, 0, 0);";
  style.append("color: rgb(");
  style.append(QString::number(textcolor.red()));
  style.append(", ");
  style.append(QString::number(textcolor.green()));
  style.append(", ");
  style.append(QString::number(textcolor.blue()));
  style.append(")");

  ui_vp_message->setStyleSheet(style);
}

QColor Courtroom::get_text_color(QString color)
{
  return ao_app->get_chat_color(color, ao_app->get_chat(m_chatmessage[CHAR_NAME]));
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
  QString f_song_clear = f_song;
  f_song_clear = f_song_clear.left(f_song_clear.lastIndexOf("."));
  int n_char = f_contents.at(1).toInt();

  if (n_char < 0 || n_char >= char_list.size())
  {
    music_player->play(f_song);
  }
  else
  {
    QString str_char = char_list.at(n_char).name;
    QString str_show = char_list.at(n_char).name;

    if (p_contents->length() > 2)
    {
        str_show = p_contents->at(2);
    }

    if (!mute_map.value(n_char))
    {
      chatlogpiece* temp = new chatlogpiece(str_char, str_show, f_song, true);
      ic_chatlog_history.append(*temp);

      while(ic_chatlog_history.size() > log_maximum_blocks && log_maximum_blocks > 0)
      {
        ic_chatlog_history.removeFirst();
      }

      append_ic_text(f_song_clear, str_show, true);
      music_player->play(f_song);
    }
  }
}

void Courtroom::handle_wtce(QString p_wtce, int variant)
{
  QString sfx_file = "courtroom_sounds.ini";

  //witness testimony
  if (p_wtce == "testimony1")
  {
    sfx_player->play(ao_app->get_sfx("witness_testimony"));
    ui_vp_wtce->play("witnesstestimony");
    testimony_in_progress = true;
    show_testimony();
  }
  //cross examination
  else if (p_wtce == "testimony2")
  {
    sfx_player->play(ao_app->get_sfx("cross_examination"));
    ui_vp_wtce->play("crossexamination");
    testimony_in_progress = false;
  }
  else if (p_wtce == "judgeruling")
  {
    if (variant == 0)
    {
        sfx_player->play(ao_app->get_sfx("not_guilty"));
        ui_vp_wtce->play("notguilty");
        testimony_in_progress = false;
    }
    else if (variant == 1) {
        sfx_player->play(ao_app->get_sfx("guilty"));
        ui_vp_wtce->play("guilty");
        testimony_in_progress = false;
    }
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

void Courtroom::toggle_judge_buttons(bool is_on)
{
  if (is_on)
  {
    ui_witness_testimony->show();
    ui_cross_examination->show();
    ui_guilty->show();
    ui_not_guilty->show();
    ui_defense_minus->show();
    ui_defense_plus->show();
    ui_prosecution_minus->show();
    ui_prosecution_plus->show();
  }
  else
  {
    ui_witness_testimony->hide();
    ui_cross_examination->hide();
    ui_guilty->hide();
    ui_not_guilty->hide();
    ui_defense_minus->hide();
    ui_defense_plus->hide();
    ui_prosecution_minus->hide();
    ui_prosecution_plus->hide();
  }
}

void Courtroom::mod_called(QString p_ip)
{
  ui_server_chatlog->append(p_ip);
  if (ui_guard->isChecked())
  {
    modcall_player->play(ao_app->get_sfx("mod_call"));
    ao_app->alert(this);
  }
}

void Courtroom::case_called(QString msg, bool def, bool pro, bool jud, bool jur, bool steno)
{
  if (ui_casing->isChecked())
  {
    ui_server_chatlog->append(msg);
    if ((ao_app->get_casing_defence_enabled() && def) ||
        (ao_app->get_casing_prosecution_enabled() && pro) ||
        (ao_app->get_casing_judge_enabled() && jud) ||
        (ao_app->get_casing_juror_enabled() && jur) ||
        (ao_app->get_casing_steno_enabled() && steno))
    {
        modcall_player->play(ao_app->get_sfx("case_call"));
        ao_app->alert(this);
    }
  }
}

void Courtroom::on_ooc_return_pressed()
{
  QString ooc_message = ui_ooc_chat_message->text();

  if (ooc_message == "" || ui_ooc_chat_name->text() == "")
    return;

  if (ooc_message.startsWith("/pos"))
  {
    if (ooc_message == "/pos jud")
    {
      toggle_judge_buttons(true);
    }
    else
    {
      toggle_judge_buttons(false);
    }
  }
  else if (ooc_message.startsWith("/login"))
  {
    ui_guard->show();
    append_server_chatmessage("CLIENT", tr("You were granted the Guard button."), "1");
  }
  else if (ooc_message.startsWith("/rainbow") && ao_app->yellow_text_enabled && !rainbow_appended)
  {
    //ui_text_color->addItem("Rainbow");
    ui_ooc_chat_message->clear();
    //rainbow_appended = true;
    append_server_chatmessage("CLIENT", tr("This does nohing, but there you go."), "1");
    return;
  }
  else if (ooc_message.startsWith("/settings"))
  {
    ui_ooc_chat_message->clear();
    ao_app->call_settings_menu();
    append_server_chatmessage("CLIENT", tr("You opened the settings menu."), "1");
    return;
  }
  else if (ooc_message.startsWith("/pair"))
  {
    ui_ooc_chat_message->clear();
    ooc_message.remove(0,6);
    
    bool ok;
    int whom = ooc_message.toInt(&ok);
    if (ok)
    {
      if (whom > -1)
      {
        other_charid = whom;
        QString msg = tr("You will now pair up with ");
        msg.append(char_list.at(whom).name);
        msg.append(tr(" if they also choose your character in return."));
        append_server_chatmessage("CLIENT", msg, "1");
      }
      else
      {
        other_charid = -1;
        append_server_chatmessage("CLIENT", tr("You are no longer paired with anyone."), "1");
      }
    }
    else
    {
      append_server_chatmessage("CLIENT", tr("Are you sure you typed that well? The char ID could not be recognised."), "1");
    }
    return;
  }
  else if (ooc_message.startsWith("/offset"))
  {
    ui_ooc_chat_message->clear();
    ooc_message.remove(0,8);
    
    bool ok;
    int off = ooc_message.toInt(&ok);
    if (ok)
    {
      if (off >= -100 && off <= 100)
      {
        offset_with_pair = off;
        QString msg = tr("You have set your offset to ");
        msg.append(QString::number(off));
        msg.append("%.");
        append_server_chatmessage("CLIENT", msg, "1");
      }
      else
      {
        append_server_chatmessage("CLIENT", tr("Your offset must be between -100% and 100%!"), "1");
      }
    }
    else
    {
      append_server_chatmessage("CLIENT", tr("That offset does not look like one."), "1");
    }
    return;
  }
  else if (ooc_message.startsWith("/switch_am"))
  {
      append_server_chatmessage("CLIENT", tr("You switched your music and area list."), "1");
      on_switch_area_music_clicked();
      ui_ooc_chat_message->clear();
      return;
  }
  else if (ooc_message.startsWith("/enable_blocks"))
  {
    append_server_chatmessage("CLIENT", tr("You have forcefully enabled features that the server may not support. You may not be able to talk IC, or worse, because of this."), "1");
    ao_app->cccc_ic_support_enabled = true;
    ao_app->arup_enabled = true;
    ao_app->modcall_reason_enabled = true;
    on_reload_theme_clicked();
    ui_ooc_chat_message->clear();
    return;
  }
  else if (ooc_message.startsWith("/non_int_pre"))
  {
    if (ui_pre_non_interrupt->isChecked())
      append_server_chatmessage("CLIENT", tr("Your pre-animations interrupt again."), "1");
    else
      append_server_chatmessage("CLIENT", tr("Your pre-animations will not interrupt text."), "1");
    ui_pre_non_interrupt->setChecked(!ui_pre_non_interrupt->isChecked());
    ui_ooc_chat_message->clear();
    return;
  }
  else if (ooc_message.startsWith("/save_chatlog"))
  {
    QFile file("chatlog.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      append_server_chatmessage("CLIENT", tr("Couldn't open chatlog.txt to write into."), "1");
      ui_ooc_chat_message->clear();
      return;
    }

    QTextStream out(&file);

    foreach (chatlogpiece item, ic_chatlog_history) {
        out << item.get_full() << '\n';
      }

    file.close();

    append_server_chatmessage("CLIENT", tr("The IC chatlog has been saved."), "1");
    ui_ooc_chat_message->clear();
    return;
  }
  else if (ooc_message.startsWith("/load_case"))
  {
    QStringList command = ooc_message.split(" ", QString::SkipEmptyParts);

    QDir casefolder("base/cases");
    if (!casefolder.exists())
    {
        QDir::current().mkdir("base/" + casefolder.dirName());
        append_server_chatmessage("CLIENT", tr("You don't have a `base/cases/` folder! It was just made for you, but seeing as it WAS just made for you, it's likely the case file you're looking for can't be found in there."), "1");
        ui_ooc_chat_message->clear();
        return;
    }
    QStringList caseslist = casefolder.entryList();
    caseslist.removeOne(".");
    caseslist.removeOne("..");
    caseslist.replaceInStrings(".ini","");

    if (command.size() < 2)
    {
      append_server_chatmessage("CLIENT", tr("You need to give a filename to load (extension not needed)! Make sure that it is in the `base/cases/` folder, and that it is a correctly formatted ini.\nCases you can load: ") + caseslist.join(", "), "1");
      ui_ooc_chat_message->clear();
      return;
    }


    if (command.size() > 2)
    {
      append_server_chatmessage("CLIENT", tr("Too many arguments to load a case! You only need one filename, without extension."), "1");
      ui_ooc_chat_message->clear();
      return;
    }

    QSettings casefile("base/cases/" + command[1] + ".ini", QSettings::IniFormat);

    QString caseauth = casefile.value("author", "").value<QString>();
    QString casedoc = casefile.value("doc", "").value<QString>();
    QString cmdoc = casefile.value("cmdoc", "").value<QString>();
    QString casestatus = casefile.value("status", "").value<QString>();

    if (!caseauth.isEmpty())
      append_server_chatmessage("CLIENT", tr("Case made by ") + caseauth + ".", "1");
    if (!casedoc.isEmpty())
      ao_app->send_server_packet(new AOPacket("CT#" + ui_ooc_chat_name->text() + "#/doc " + casedoc + "#%"));
    if (!casestatus.isEmpty())
      ao_app->send_server_packet(new AOPacket("CT#" + ui_ooc_chat_name->text() + "#/status " + casestatus + "#%"));
    if (!cmdoc.isEmpty())
      append_server_chatmessage("CLIENT", tr("Navigate to ") + cmdoc + tr(" for the CM doc."), "1");

    for (int i = local_evidence_list.size() - 1; i >= 0; i--) {
        ao_app->send_server_packet(new AOPacket("DE#" + QString::number(i) + "#%"));
    }

    foreach (QString evi, casefile.childGroups()) {
        if (evi == "General")
          continue;

        QStringList f_contents;

        f_contents.append(casefile.value(evi + "/name", "UNKNOWN").value<QString>());
        f_contents.append(casefile.value(evi + "/description", "UNKNOWN").value<QString>());
        f_contents.append(casefile.value(evi + "/image", "UNKNOWN.png").value<QString>());

        ao_app->send_server_packet(new AOPacket("PE", f_contents));
      }

    append_server_chatmessage("CLIENT", tr("Your case \"") + command[1] + tr("\" was loaded!"), "1");
    ui_ooc_chat_message->clear();
    return;
  }
  else if(ooc_message.startsWith("/save_case"))
  {
      QStringList command = ooc_message.split(" ", QString::SkipEmptyParts);

      QDir casefolder("base/cases");
      if (!casefolder.exists())
      {
          QDir::current().mkdir("base/" + casefolder.dirName());
          append_server_chatmessage("CLIENT", tr("You don't have a `base/cases/` folder! It was just made for you, but seeing as it WAS just made for you, it's likely that you somehow deleted it."), "1");
          ui_ooc_chat_message->clear();
          return;
      }
      QStringList caseslist = casefolder.entryList();
      caseslist.removeOne(".");
      caseslist.removeOne("..");
      caseslist.replaceInStrings(".ini","");

      if (command.size() < 3)
      {
        append_server_chatmessage("CLIENT", tr("You need to give a filename to save (extension not needed) and the courtroom status!"), "1");
        ui_ooc_chat_message->clear();
        return;
      }


      if (command.size() > 3)
      {
        append_server_chatmessage("CLIENT", tr("Too many arguments to save a case! You only need a filename without extension and the courtroom status!"), "1");
        ui_ooc_chat_message->clear();
        return;
      }
      QSettings casefile("base/cases/" + command[1] + ".ini", QSettings::IniFormat);
      casefile.setValue("author",ui_ooc_chat_name->text());
      casefile.setValue("cmdoc","");
      casefile.setValue("doc", "");
      casefile.setValue("status",command[2]);
      casefile.sync();
      for(int i = local_evidence_list.size() - 1; i >= 0; i--)
      {
           QString clean_evidence_dsc =  local_evidence_list[i].description.replace(QRegularExpression("<owner = ...>..."), "");
           clean_evidence_dsc = clean_evidence_dsc.replace(clean_evidence_dsc.lastIndexOf(">"), 1, "");
           casefile.beginGroup(QString::number(i));
           casefile.sync();
           casefile.setValue("name",local_evidence_list[i].name);
           casefile.setValue("description",local_evidence_list[i].description);
           casefile.setValue("image",local_evidence_list[i].image);
           casefile.endGroup();
      }
      casefile.sync();
      append_server_chatmessage("CLIENT", tr("Succesfully saved, edit doc and cmdoc link on the ini!"), "1");
      ui_ooc_chat_message->clear();
      return;

  }
  else if(ooc_message.startsWith("/clear"))
  {
      ui_server_chatlog->clear();
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
    ui_ooc_toggle->setText(tr("Master"));

    server_ooc = false;
  }
  else
  {
    ui_ms_chatlog->hide();
    ui_server_chatlog->show();
    ui_ooc_toggle->setText(tr("Server"));

    server_ooc = true;
  }
}

void Courtroom::on_music_search_edited(QString p_text)
{
  //preventing compiler warnings
  p_text += "a";
  list_music();
  list_areas();
}

void Courtroom::on_pos_dropdown_changed(int p_index)
{
  ui_ic_chat_message->setFocus();

  if (p_index < 0 || p_index > 5)
    return;

  toggle_judge_buttons(false);

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
    toggle_judge_buttons(true);
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
}

void Courtroom::on_pair_list_clicked(QModelIndex p_index)
{
  QListWidgetItem *f_item = ui_pair_list->item(p_index.row());
  QString f_char = f_item->text();
  QString real_char;
  int f_cid = -1;

  if (f_char.endsWith(" [x]"))
  {
    real_char = f_char.left(f_char.size() - 4);
    f_item->setText(real_char);
  }
  else
  {
   real_char = f_char;
   for (int n_char = 0 ; n_char < char_list.size() ; n_char++)
   {
    if (char_list.at(n_char).name == real_char)
      f_cid = n_char;
   }
  }




  if (f_cid < -2 || f_cid >= char_list.size())
  {
    qDebug() << "W: " << real_char << " not present in char_list";
    return;
  }

  other_charid = f_cid;

  // Redo the character list.
  QStringList sorted_pair_list;

  for (char_type i_char : char_list)
    sorted_pair_list.append(i_char.name);

  sorted_pair_list.sort();

  for (int i = 0; i < ui_pair_list->count(); i++) {
    ui_pair_list->item(i)->setText(sorted_pair_list.at(i));
  }
  if(other_charid != -1)
  {
   f_item->setText(real_char + " [x]");
  }
}

void Courtroom::on_music_list_double_clicked(QModelIndex p_model)
{
  if (is_muted)
    return;

  QString p_song = music_list.at(music_row_to_number.at(p_model.row()));

  if (!ui_ic_chat_name->text().isEmpty() && ao_app->cccc_ic_support_enabled)
  {
    ao_app->send_server_packet(new AOPacket("MC#" + p_song + "#" + QString::number(m_cid) + "#" + ui_ic_chat_name->text() + "#%"), false);
  }
  else
  {
    ao_app->send_server_packet(new AOPacket("MC#" + p_song + "#" + QString::number(m_cid) + "#%"), false);
  }
}

void Courtroom::on_area_list_double_clicked(QModelIndex p_model)
{
    QString p_area = area_list.at(area_row_to_number.at(p_model.row()));
    ao_app->send_server_packet(new AOPacket("MC#" + p_area + "#" + QString::number(m_cid) + "#%"), false);
}

void Courtroom::on_hold_it_clicked()
{
  if (objection_state == 1)
  {
    ui_hold_it->set_image("holdit.png");
    objection_state = 0;
  }
  else
  {
    ui_objection->set_image("objection.png");
    ui_take_that->set_image("takethat.png");
    ui_custom_objection->set_image("custom.png");

    ui_hold_it->set_image("holdit_selected.png");
    objection_state = 1;
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_objection_clicked()
{
  if (objection_state == 2)
  {
    ui_objection->set_image("objection.png");
    objection_state = 0;
  }
  else
  {
    ui_hold_it->set_image("holdit.png");
    ui_take_that->set_image("takethat.png");
    ui_custom_objection->set_image("custom.png");

    ui_objection->set_image("objection_selected.png");
    objection_state = 2;
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_take_that_clicked()
{
  if (objection_state == 3)
  {
    ui_take_that->set_image("takethat.png");
    objection_state = 0;
  }
  else
  {
    ui_objection->set_image("objection.png");
    ui_hold_it->set_image("holdit.png");
    ui_custom_objection->set_image("custom.png");

    ui_take_that->set_image("takethat_selected.png");
    objection_state = 3;
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_custom_objection_clicked()
{
  if (objection_state > 39)
  {
    ui_custom_objection->set_image("custom.png");
    objection_state = 0;
  }
  else
  {
    ui_objection->set_image("objection.png");
    ui_take_that->set_image("takethat.png");
    ui_hold_it->set_image("holdit.png");

    ui_custom_objection->set_image("custom_selected.png");
    objection_state = 41; //It selects by default first custom objection.
  }

  ui_ic_chat_message->setFocus();
}
void Courtroom::ShowContextMenu(const QPoint& pos)
{
    QPoint globalPos = ui_custom_objection->mapToGlobal(pos);
    QMenu Custom_obj_menu;
    int x = 1;
    while(ao_app->custom_objection_enabled &&
          (file_exists(ao_app->get_character_path(current_char, "custom" + QString::number(x) + ".gif")) ||
          file_exists(ao_app->get_character_path(current_char, "custom" + QString::number(x) + ".apng"))) &&
          file_exists(ao_app->get_character_path(current_char, "custom" + QString::number(x) + ".wav")))
    {        
        Custom_obj_menu.addAction("custom" + QString::number(x));
        x++;
    }
    //tomorrow do the auto adds
    QAction* selecteditem = Custom_obj_menu.exec(globalPos);
    if(selecteditem)
    {
        ui_objection->set_image("objection.png");
        ui_take_that->set_image("takethat.png");
        ui_hold_it->set_image("holdit.png");
        ui_custom_objection->set_image("custom_selected.png");
        objection_state = selecteditem->text().split('m')[1].toInt() + 40; //here we make the "custom1" to 41 in order to make it a flag
    }
    else
    {
        //nothing was chosen
    }
}

void Courtroom::on_realization_clicked()
{
  if (realization_state == 0)
  {
    realization_state = 1;
    ui_realization->set_image("realization_pressed.png");
  }
  else
  {
    realization_state = 0;
    ui_realization->set_image("realization.png");
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_mute_clicked()
{
  if (ui_mute_list->isHidden())
  {
    ui_mute_list->show();
    ui_pair_list->hide();
    ui_pair_offset_spinbox->hide();
    ui_pair_button->set_image("pair_button.png");
    ui_mute->set_image("mute_pressed.png");
  }
  else
  {
    ui_mute_list->hide();
    ui_mute->set_image("mute.png");
  }
}

void Courtroom::on_pair_clicked()
{
  if (ui_pair_list->isHidden())
  {
    ui_pair_list->show();
    ui_pair_offset_spinbox->show();
    ui_mute_list->hide();
    ui_mute->set_image("mute.png");
    ui_pair_button->set_image("pair_button_pressed.png");
  }
  else
  {
    ui_pair_list->hide();
    ui_pair_offset_spinbox->hide();
    ui_pair_button->set_image("pair_button.png");
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
  text_color = p_color;
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_music_slider_moved(int p_value)
{
  music_player->set_volume(p_value);
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_sfx_slider_moved(int p_value)
{
  sfx_player->set_volume(p_value);
  objection_player->set_volume(p_value);
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_blip_slider_moved(int p_value)
{
  blip_player->set_volume(p_value);
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_log_limit_changed(int value)
{
  log_maximum_blocks = value;
}

void Courtroom::on_pair_offset_changed(int value)
{
  offset_with_pair = value;
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

void Courtroom::on_not_guilty_clicked()
{
  if (is_muted)
    return;

  ao_app->send_server_packet(new AOPacket("RT#judgeruling#0#%"));

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_guilty_clicked()
{
  if (is_muted)
    return;

  ao_app->send_server_packet(new AOPacket("RT#judgeruling#1#%"));

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_change_character_clicked()
{
  music_player->set_volume(0);
  sfx_player->set_volume(0);
  sfx_player->set_volume(0);
  blip_player->set_volume(0);

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
  enter_courtroom(-1);

  ui_emotes->hide();

  ui_char_select_background->hide();
}

void Courtroom::on_call_mod_clicked()
{
  if (ao_app->modcall_reason_enabled) {
    QMessageBox errorBox;
    QInputDialog input;

    input.setWindowFlags(Qt::WindowSystemMenuHint);
    input.setLabelText(tr("Reason:"));
    input.setWindowTitle(tr("Call Moderator"));
    auto code = input.exec();

    if (code != QDialog::Accepted)
      return;

    QString text = input.textValue();
    if (text.isEmpty()) {
      errorBox.critical(nullptr, tr("Error"), tr("You must provide a reason."));
      return;
    } else if (text.length() > 256) {
      errorBox.critical(nullptr, tr("Error"), tr("The message is too long."));
      return;
    }

    QStringList mod_reason;
    mod_reason.append(text);

    ao_app->send_server_packet(new AOPacket("ZZ", mod_reason));
  } else {
    ao_app->send_server_packet(new AOPacket("ZZ#%"));
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_settings_clicked()
{
    ao_app->call_settings_menu();
}

void Courtroom::on_announce_casing_clicked()
{
    ao_app->call_announce_menu(this);
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

void Courtroom::on_showname_enable_clicked()
{
  ui_ic_chatlog->clear();
  first_message_sent = false;

  foreach (chatlogpiece item, ic_chatlog_history) {
      if (ui_showname_enable->isChecked())
      {
         if (item.get_is_song())
           append_ic_text(item.get_message(), item.get_showname(), true);
         else
           append_ic_text(item.get_message(), item.get_showname());
      }
      else
      {
          if (item.get_is_song())
            append_ic_text(item.get_message(), item.get_name(), true);
          else
            append_ic_text(item.get_message(), item.get_name());
      }
    }

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

void Courtroom::on_switch_area_music_clicked()
{
    if (ui_area_list->isHidden())
    {
        ui_area_list->show();
        ui_music_list->hide();
    }
    else
    {
        ui_area_list->hide();
        ui_music_list->show();
    }
}

void Courtroom::ping_server()
{
  ao_app->send_server_packet(new AOPacket("CH#" + QString::number(m_cid) + "#%"));
}

void Courtroom::on_casing_clicked()
{
  if (ao_app->casing_alerts_enabled)
  {
    if (ui_casing->isChecked())
    {
      QStringList f_packet;

      f_packet.append(ao_app->get_casing_can_host_cases());
      f_packet.append(QString::number(ao_app->get_casing_cm_enabled()));
      f_packet.append(QString::number(ao_app->get_casing_defence_enabled()));
      f_packet.append(QString::number(ao_app->get_casing_prosecution_enabled()));
      f_packet.append(QString::number(ao_app->get_casing_judge_enabled()));
      f_packet.append(QString::number(ao_app->get_casing_juror_enabled()));
      f_packet.append(QString::number(ao_app->get_casing_steno_enabled()));

      ao_app->send_server_packet(new AOPacket("SETCASE", f_packet));
    }
    else
      ao_app->send_server_packet(new AOPacket("SETCASE#\"\"#0#0#0#0#0#0#%"));
  }
}

void Courtroom::announce_case(QString title, bool def, bool pro, bool jud, bool jur, bool steno)
{
  if (ao_app->casing_alerts_enabled)
  {
    QStringList f_packet;

    f_packet.append(title);
    f_packet.append(QString::number(def));
    f_packet.append(QString::number(pro));
    f_packet.append(QString::number(jud));
    f_packet.append(QString::number(jur));
    f_packet.append(QString::number(steno));

    ao_app->send_server_packet(new AOPacket("CASEA", f_packet));
    }
}

Courtroom::~Courtroom()
{
  delete music_player;
  delete sfx_player;
  delete objection_player;
  delete blip_player;
}


#if (defined (_WIN32) || defined (_WIN64))
void Courtroom::load_bass_opus_plugin()
{
  #ifdef BASSAUDIO
  BASS_PluginLoad("bassopus.dll", 0);
  #endif
}
#elif (defined (LINUX) || defined (__linux__))
void Courtroom::load_bass_opus_plugin()
{
  #ifdef BASSAUDIO
  BASS_PluginLoad("libbassopus.so", 0);
  #endif
}
#elif defined __APPLE__
void Courtroom::load_bass_opus_plugin()
{
  QString libpath = ao_app->get_base_path() + "../../Frameworks/libbassopus.dylib";
  QByteArray ba = libpath.toLocal8Bit();
  #ifdef BASSAUDIO
  BASS_PluginLoad(ba.data(), 0);
  #endif
}
#else
#error This operating system is unsupported for bass plugins.
#endif
