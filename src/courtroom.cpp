#include "courtroom.h"

Courtroom::Courtroom(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

  this->setWindowFlags( (this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  ao_app->initBASS();

  qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch() / 1000));

  keepalive_timer = new QTimer(this);
  keepalive_timer->start(60000);

  chat_tick_timer = new QTimer(this);

  text_delay_timer = new QTimer(this);
  text_delay_timer->setSingleShot(true);

  sfx_delay_timer = new QTimer(this);
  sfx_delay_timer->setSingleShot(true);

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

  ui_vp_evidence_display = new AOEvidenceDisplay(ui_viewport, ao_app);

  ui_vp_chatbox = new AOImage(this, ao_app);
  ui_vp_showname = new QLabel(ui_vp_chatbox);
  ui_vp_showname->setAlignment(Qt::AlignLeft);
  ui_vp_chat_arrow = new AOMovie(ui_vp_chatbox, ao_app);
  ui_vp_chat_arrow->set_play_once(false);

  ui_vp_message = new QTextEdit(this);
  ui_vp_message->setFrameStyle(QFrame::NoFrame);
  ui_vp_message->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_vp_message->setReadOnly(true);

  ui_vp_testimony = new AOMovie(this, ao_app);
  ui_vp_testimony->set_play_once(false);
  ui_vp_testimony->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_effect = new AOMovie(this, ao_app);
  ui_vp_effect->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_wtce = new AOMovie(this, ao_app);
  ui_vp_wtce->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_vp_objection = new AOMovie(this, ao_app);
  ui_vp_objection->setAttribute(Qt::WA_TransparentForMouseEvents);

  ui_ic_chatlog = new QTextEdit(this);
  ui_ic_chatlog->setReadOnly(true);

  log_maximum_blocks = ao_app->get_max_log_size();
  log_goes_downwards = ao_app->get_log_goes_downwards();
  log_colors = ao_app->is_colorlog_enabled();
  log_newline = ao_app->get_log_newline();
  log_margin = ao_app->get_log_margin();
  log_timestamp = ao_app->get_log_timestamp();

  ui_ms_chatlog = new AOTextArea(this);
  ui_ms_chatlog->setReadOnly(true);
  ui_ms_chatlog->setOpenExternalLinks(true);
  ui_ms_chatlog->hide();

  ui_server_chatlog = new AOTextArea(this);
  ui_server_chatlog->setReadOnly(true);
  ui_server_chatlog->setOpenExternalLinks(true);

  ui_area_list = new QTreeWidget(this);
  ui_area_list->setColumnCount(2);
  ui_area_list->hideColumn(0);
  ui_area_list->setHeaderHidden(true);
  ui_area_list->header()->setStretchLastSection(false);
  ui_area_list->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui_area_list->hide();

  ui_music_list = new QTreeWidget(this);
  ui_music_list->setColumnCount(2);
  ui_music_list->hideColumn(1);
  ui_music_list->setHeaderHidden(true);
  ui_music_list->header()->setStretchLastSection(false);
  ui_music_list->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui_music_list->setContextMenuPolicy(Qt::CustomContextMenu);

  ui_music_display = new AOMovie(this, ao_app);
  ui_music_display->set_play_once(false);
  ui_music_display->setAttribute(Qt::WA_TransparentForMouseEvents);

  ui_music_name = new ScrollText(ui_music_display);
  ui_music_name->setText(tr("None"));
  ui_music_name->setAttribute(Qt::WA_TransparentForMouseEvents);

  ui_ic_chat_name = new QLineEdit(this);
  ui_ic_chat_name->setFrame(false);
  ui_ic_chat_name->setPlaceholderText(tr("Showname"));

  ui_ic_chat_message = new AOLineEdit(this);
  ui_ic_chat_message->setFrame(false);
  ui_ic_chat_message->setPlaceholderText(tr("Message"));
  ui_ic_chat_message->preserve_selection(true);
  //  ui_ic_chat_message->setValidator(new QRegExpValidator(QRegExp("^\\S+(?:
  //  \\S+)*$"), ui_ic_chat_message));
  // todo: filter out \n from showing up as that commonly breaks the chatlog and
  // can be spammed to hell

  ui_muted = new AOImage(ui_ic_chat_message, ao_app);
  ui_muted->hide();

  ui_ooc_chat_message = new QLineEdit(this);
  ui_ooc_chat_message->setFrame(false);

  ui_ooc_chat_name = new QLineEdit(this);
  ui_ooc_chat_name->setFrame(false);
  ui_ooc_chat_name->setPlaceholderText(tr("Name"));
  ui_ooc_chat_name->setMaxLength(30);
  ui_ooc_chat_name->setText(p_ao_app->get_default_username());

  // ui_area_password = new QLineEdit(this);
  // ui_area_password->setFrame(false);
  ui_music_search = new QLineEdit(this);
  ui_music_search->setFrame(false);
  ui_music_search->setPlaceholderText(tr("Search"));

  initialize_emotes();

  ui_pos_dropdown = new QComboBox(this);

  ui_iniswap_dropdown = new QComboBox(this);
  ui_iniswap_dropdown->setContextMenuPolicy(Qt::CustomContextMenu);
  ui_iniswap_remove = new AOButton(this, ao_app);

  ui_sfx_dropdown = new QComboBox(this);
  ui_sfx_dropdown->setContextMenuPolicy(Qt::CustomContextMenu);
  ui_sfx_remove = new AOButton(this, ao_app);

  ui_effects_dropdown = new QComboBox(this);
  ui_effects_dropdown->setContextMenuPolicy(Qt::CustomContextMenu);

  ui_defense_bar = new AOImage(this, ao_app);
  ui_prosecution_bar = new AOImage(this, ao_app);

  ui_music_label = new QLabel(this);
  ui_sfx_label = new QLabel(this);
  ui_blip_label = new QLabel(this);

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

  ui_additive = new QCheckBox(this);
  ui_additive->setText(tr("Additive"));
  ui_additive->hide();

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
  custom_obj_menu = new QMenu(this);
  ui_realization = new AOButton(this, ao_app);
  ui_screenshake = new AOButton(this, ao_app);
  ui_mute = new AOButton(this, ao_app);

  ui_defense_plus = new AOButton(this, ao_app);
  ui_defense_minus = new AOButton(this, ao_app);

  ui_prosecution_plus = new AOButton(this, ao_app);
  ui_prosecution_minus = new AOButton(this, ao_app);

  ui_text_color = new QComboBox(this);

  ui_music_slider = new QSlider(Qt::Horizontal, this);
  ui_music_slider->setRange(0, 100);
  ui_music_slider->setValue(ao_app->get_default_music());

  ui_sfx_slider = new QSlider(Qt::Horizontal, this);
  ui_sfx_slider->setRange(0, 100);
  ui_sfx_slider->setValue(ao_app->get_default_sfx());

  ui_blip_slider = new QSlider(Qt::Horizontal, this);
  ui_blip_slider->setRange(0, 100);
  ui_blip_slider->setValue(ao_app->get_default_blip());

  ui_mute_list = new QListWidget(this);

  ui_pair_list = new QListWidget(this);
  ui_pair_offset_spinbox = new QSpinBox(this);
  ui_pair_offset_spinbox->setRange(-100, 100);
  ui_pair_offset_spinbox->setSuffix(tr("% x offset"));
  ui_pair_vert_offset_spinbox = new QSpinBox(this);
  ui_pair_vert_offset_spinbox->setRange(-100, 100);
  ui_pair_vert_offset_spinbox->setSuffix(tr("% y offset"));

  ui_pair_order_dropdown = new QComboBox(this);
  ui_pair_order_dropdown->addItem(tr("To front"));
  ui_pair_order_dropdown->addItem(tr("To behind"));

  ui_pair_button = new AOButton(this, ao_app);

  ui_evidence_button = new AOButton(this, ao_app);

  initialize_evidence();

  construct_char_select();

  connect(keepalive_timer, SIGNAL(timeout()), this, SLOT(ping_server()));

  connect(ui_vp_objection, SIGNAL(done()), this, SLOT(objection_done()));
  connect(ui_vp_player_char, SIGNAL(done()), this, SLOT(preanim_done()));
  connect(ui_vp_player_char, SIGNAL(shake()), this, SLOT(do_screenshake()));
  connect(ui_vp_player_char, SIGNAL(flash()), this, SLOT(do_flash()));
  connect(ui_vp_player_char, SIGNAL(play_sfx(QString)), this,
          SLOT(play_char_sfx(QString)));

  connect(text_delay_timer, SIGNAL(timeout()), this,
          SLOT(start_chat_ticking()));
  connect(sfx_delay_timer, SIGNAL(timeout()), this, SLOT(play_sfx()));

  connect(chat_tick_timer, SIGNAL(timeout()), this, SLOT(chat_tick()));

  connect(ui_pos_dropdown, SIGNAL(currentIndexChanged(int)), this,
          SLOT(on_pos_dropdown_changed(int)));

  connect(ui_iniswap_dropdown, SIGNAL(currentIndexChanged(int)), this,
          SLOT(on_iniswap_dropdown_changed(int)));
  connect(ui_iniswap_dropdown, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(on_iniswap_context_menu_requested(QPoint)));
  connect(ui_iniswap_remove, SIGNAL(clicked()), this,
          SLOT(on_iniswap_remove_clicked()));

  connect(ui_sfx_dropdown, SIGNAL(currentIndexChanged(int)), this,
          SLOT(on_sfx_dropdown_changed(int)));
  connect(ui_sfx_dropdown, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(on_sfx_context_menu_requested(QPoint)));
  connect(ui_sfx_remove, SIGNAL(clicked()), this,
          SLOT(on_sfx_remove_clicked()));

  connect(ui_effects_dropdown, SIGNAL(currentIndexChanged(int)), this,
          SLOT(on_effects_dropdown_changed(int)));
  connect(ui_effects_dropdown, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(on_effects_context_menu_requested(QPoint)));

  connect(ui_music_search, SIGNAL(returnPressed()), this,
          SLOT(on_music_search_return_pressed()));
  connect(ui_mute_list, SIGNAL(clicked(QModelIndex)), this,
          SLOT(on_mute_list_clicked(QModelIndex)));

  connect(ui_ic_chat_message, SIGNAL(returnPressed()), this,
          SLOT(on_chat_return_pressed()));

  connect(ui_ooc_chat_message, SIGNAL(returnPressed()), this,
          SLOT(on_ooc_return_pressed()));

  connect(ui_music_list, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
          this, SLOT(on_music_list_double_clicked(QTreeWidgetItem *, int)));
  connect(ui_music_list, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(on_music_list_context_menu_requested(QPoint)));

  connect(ui_area_list, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this,
          SLOT(on_area_list_double_clicked(QTreeWidgetItem *, int)));

  connect(ui_hold_it, SIGNAL(clicked()), this, SLOT(on_hold_it_clicked()));
  connect(ui_objection, SIGNAL(clicked()), this, SLOT(on_objection_clicked()));
  connect(ui_take_that, SIGNAL(clicked()), this, SLOT(on_take_that_clicked()));
  connect(ui_custom_objection, SIGNAL(clicked()), this,
          SLOT(on_custom_objection_clicked()));
  connect(ui_custom_objection,
          SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(show_custom_objection_menu(const QPoint &)));

  connect(ui_realization, SIGNAL(clicked()), this,
          SLOT(on_realization_clicked()));
  connect(ui_screenshake, SIGNAL(clicked()), this,
          SLOT(on_screenshake_clicked()));

  connect(ui_mute, SIGNAL(clicked()), this, SLOT(on_mute_clicked()));

  connect(ui_defense_minus, SIGNAL(clicked()), this,
          SLOT(on_defense_minus_clicked()));
  connect(ui_defense_plus, SIGNAL(clicked()), this,
          SLOT(on_defense_plus_clicked()));
  connect(ui_prosecution_minus, SIGNAL(clicked()), this,
          SLOT(on_prosecution_minus_clicked()));
  connect(ui_prosecution_plus, SIGNAL(clicked()), this,
          SLOT(on_prosecution_plus_clicked()));

  connect(ui_text_color, SIGNAL(currentIndexChanged(int)), this,
          SLOT(on_text_color_changed(int)));

  connect(ui_music_slider, SIGNAL(valueChanged(int)), this,
          SLOT(on_music_slider_moved(int)));
  connect(ui_sfx_slider, SIGNAL(valueChanged(int)), this,
          SLOT(on_sfx_slider_moved(int)));
  connect(ui_blip_slider, SIGNAL(valueChanged(int)), this,
          SLOT(on_blip_slider_moved(int)));

  connect(ui_ooc_toggle, SIGNAL(clicked()), this,
          SLOT(on_ooc_toggle_clicked()));

  connect(ui_music_search, SIGNAL(textChanged(QString)), this,
          SLOT(on_music_search_edited(QString)));

  connect(ui_witness_testimony, SIGNAL(clicked()), this,
          SLOT(on_witness_testimony_clicked()));
  connect(ui_cross_examination, SIGNAL(clicked()), this,
          SLOT(on_cross_examination_clicked()));
  connect(ui_guilty, SIGNAL(clicked()), this, SLOT(on_guilty_clicked()));
  connect(ui_not_guilty, SIGNAL(clicked()), this,
          SLOT(on_not_guilty_clicked()));

  connect(ui_change_character, SIGNAL(clicked()), this,
          SLOT(on_change_character_clicked()));
  connect(ui_reload_theme, SIGNAL(clicked()), this,
          SLOT(on_reload_theme_clicked()));
  connect(ui_call_mod, SIGNAL(clicked()), this, SLOT(on_call_mod_clicked()));
  connect(ui_settings, SIGNAL(clicked()), this, SLOT(on_settings_clicked()));
  connect(ui_announce_casing, SIGNAL(clicked()), this,
          SLOT(on_announce_casing_clicked()));
  connect(ui_switch_area_music, SIGNAL(clicked()), this,
          SLOT(on_switch_area_music_clicked()));

  connect(ui_pre, SIGNAL(clicked()), this, SLOT(on_pre_clicked()));
  connect(ui_flip, SIGNAL(clicked()), this, SLOT(on_flip_clicked()));
  connect(ui_additive, SIGNAL(clicked()), this, SLOT(on_additive_clicked()));
  connect(ui_guard, SIGNAL(clicked()), this, SLOT(on_guard_clicked()));
  connect(ui_casing, SIGNAL(clicked()), this, SLOT(on_casing_clicked()));

  connect(ui_showname_enable, SIGNAL(clicked()), this,
          SLOT(on_showname_enable_clicked()));

  connect(ui_pair_button, SIGNAL(clicked()), this, SLOT(on_pair_clicked()));
  connect(ui_pair_list, SIGNAL(clicked(QModelIndex)), this,
          SLOT(on_pair_list_clicked(QModelIndex)));
  connect(ui_pair_offset_spinbox, SIGNAL(valueChanged(int)), this,
          SLOT(on_pair_offset_changed(int)));
  connect(ui_pair_vert_offset_spinbox, SIGNAL(valueChanged(int)), this,
          SLOT(on_pair_vert_offset_changed(int)));
  connect(ui_pair_order_dropdown, SIGNAL(currentIndexChanged(int)), this,
          SLOT(on_pair_order_dropdown_changed(int)));

  connect(ui_evidence_button, SIGNAL(clicked()), this,
          SLOT(on_evidence_button_clicked()));

  set_widgets();

  set_char_select();
}

void Courtroom::set_mute_list()
{
  mute_map.clear();

  // maps which characters are muted based on cid, none are muted by default
  for (int n_cid = 0; n_cid < char_list.size(); n_cid++) {
    mute_map.insert(n_cid, false);
  }

  QStringList sorted_mute_list;

  for (char_type i_char : char_list)
    sorted_mute_list.append(i_char.name);

  sorted_mute_list.sort();

  for (QString i_name : sorted_mute_list) {
    // mute_map.insert(i_name, false);
    ui_mute_list->addItem(i_name);
  }
}

void Courtroom::set_pair_list()
{
  QStringList sorted_pair_list;

  for (char_type i_char : char_list)
    sorted_pair_list.append(i_char.name);

  sorted_pair_list.sort();

  for (QString i_name : sorted_pair_list) {
    ui_pair_list->addItem(i_name);
  }
}

void Courtroom::set_widgets()
{
  blip_rate = ao_app->read_blip_rate();
  blank_blip = ao_app->get_blank_blip();

  QString filename = "courtroom_design.ini";

  pos_size_type f_courtroom =
      ao_app->get_element_dimensions("courtroom", filename);

  if (f_courtroom.width < 0 || f_courtroom.height < 0) {
    qDebug() << "W: did not find courtroom width or height in " << filename;

    this->setFixedSize(714, 668);
  }
  else {
    m_courtroom_width = f_courtroom.width;
    m_courtroom_height = f_courtroom.height;

    this->setFixedSize(f_courtroom.width, f_courtroom.height);
  }

  set_fonts();

  ui_background->move(0, 0);
  ui_background->resize(m_courtroom_width, m_courtroom_height);
  ui_background->set_image("courtroombackground");

  set_size_and_pos(ui_viewport, "viewport");

  // If there is a point to it, show all CCCC features.
  // We also do this this soon so that set_size_and_pos can hide them all later,
  // if needed.
  if (ao_app->cccc_ic_support_enabled) {
    ui_pair_button->show();
    ui_pre_non_interrupt->show();
    ui_showname_enable->show();
    ui_ic_chat_name->show();
    ui_ic_chat_name->setEnabled(true);
  }
  else {
    ui_pair_button->hide();
    ui_pre_non_interrupt->hide();
    ui_showname_enable->hide();
    ui_ic_chat_name->hide();
    ui_ic_chat_name->setEnabled(false);
  }

  if (ao_app->casing_alerts_enabled) {
    ui_announce_casing->show();
    ui_casing->show();
  }
  else {
    ui_announce_casing->hide();
    ui_casing->hide();
  }

  // We also show the non-server-dependent client additions.
  // Once again, if the theme can't display it, set_move_and_pos will catch
  // them.
  ui_settings->show();

  ui_vp_background->move(0, 0);
  ui_vp_background->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_speedlines->move(0, 0);
  ui_vp_speedlines->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_player_char->move(0, 0);
  ui_vp_player_char->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_sideplayer_char->move(0, 0);
  ui_vp_sideplayer_char->combo_resize(ui_viewport->width(),
                                      ui_viewport->height());

  // the AO2 desk element
  ui_vp_desk->move(0, 0);
  ui_vp_desk->combo_resize(ui_viewport->width(), ui_viewport->height());

  // the size of the ui_vp_legacy_desk element relies on various factors and is
  // set in set_scene()

  double y_modifier = 147.0 / 192.0;
  int final_y = static_cast<int>(y_modifier * ui_viewport->height());
  ui_vp_legacy_desk->move(0, final_y);
  ui_vp_legacy_desk->hide();

  ui_vp_evidence_display->move(0, 0);
  ui_vp_evidence_display->combo_resize(ui_viewport->width(),
                                       ui_viewport->height());

  ui_vp_chat_arrow->move(0, 0);
  pos_size_type design_ini_result =
      ao_app->get_element_dimensions("chat_arrow", "courtroom_design.ini");

  if (design_ini_result.width < 0 || design_ini_result.height < 0) {
    qDebug() << "W: could not find \"chat_arrow\" in courtroom_design.ini";
    ui_vp_chat_arrow->hide();
  }
  else {
    ui_vp_chat_arrow->move(design_ini_result.x, design_ini_result.y);
    ui_vp_chat_arrow->combo_resize(design_ini_result.width,
                                   design_ini_result.height);
  }

  ui_vp_testimony->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_testimony->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_effect->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_effect->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_wtce->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_wtce->combo_resize(ui_viewport->width(), ui_viewport->height());

  ui_vp_objection->move(ui_viewport->x(), ui_viewport->y());
  ui_vp_objection->combo_resize(ui_viewport->width(), ui_viewport->height());

  log_maximum_blocks = ao_app->get_max_log_size();

  bool regenerate = log_goes_downwards != ao_app->get_log_goes_downwards() ||
                    log_colors != ao_app->is_colorlog_enabled() ||
                    log_newline != ao_app->get_log_newline() ||
                    log_margin != ao_app->get_log_margin() ||
                    log_timestamp != ao_app->get_log_timestamp();
  log_goes_downwards = ao_app->get_log_goes_downwards();
  log_colors = ao_app->is_colorlog_enabled();
  log_newline = ao_app->get_log_newline();
  log_margin = ao_app->get_log_margin();
  log_timestamp = ao_app->get_log_timestamp();
  if (regenerate)
    regenerate_ic_chatlog();

  set_size_and_pos(ui_ic_chatlog, "ic_chatlog");
  ui_ic_chatlog->setFrameShape(QFrame::NoFrame);
  ui_ic_chatlog->setPlaceholderText(log_goes_downwards ? "▼ Log goes down ▼"
                                                       : "▲ Log goes up ▲");

  set_size_and_pos(ui_ms_chatlog, "ms_chatlog");
  ui_ms_chatlog->setFrameShape(QFrame::NoFrame);

  set_size_and_pos(ui_server_chatlog, "server_chatlog");
  ui_server_chatlog->setFrameShape(QFrame::NoFrame);

  set_size_and_pos(ui_mute_list, "mute_list");
  ui_mute_list->hide();

  set_size_and_pos(ui_pair_list, "pair_list");
  ui_pair_list->hide();
  ui_pair_list->setToolTip(tr("Select a character you wish to pair with."));

  set_size_and_pos(ui_pair_offset_spinbox, "pair_offset_spinbox");
  ui_pair_offset_spinbox->hide();
  ui_pair_offset_spinbox->setToolTip(
      tr("Change the horizontal percentage offset of your character's position from the "
         "center of the screen."));
  
  set_size_and_pos(ui_pair_vert_offset_spinbox, "pair_vert_offset_spinbox");
  ui_pair_vert_offset_spinbox->hide();
  ui_pair_vert_offset_spinbox->setToolTip(
      tr("Change the vertical percentage offset of your character's position from the "
         "center of the screen."));

  ui_pair_order_dropdown->hide();
  set_size_and_pos(ui_pair_order_dropdown, "pair_order_dropdown");
  ui_pair_order_dropdown->setToolTip(
      tr("Change the order of appearance for your character."));

  set_size_and_pos(ui_pair_button, "pair_button");
  ui_pair_button->set_image("pair_button");
  ui_pair_button->setToolTip(
      tr("Display the list of characters to pair with."));

  set_size_and_pos(ui_area_list, "music_list");
  ui_area_list->header()->setMinimumSectionSize(ui_area_list->width());

  set_size_and_pos(ui_music_list, "music_list");
  ui_music_list->header()->setMinimumSectionSize(ui_music_list->width());

  set_size_and_pos(ui_music_name, "music_name");

  ui_music_display->move(0, 0);
  design_ini_result =
      ao_app->get_element_dimensions("music_display", "courtroom_design.ini");

  if (design_ini_result.width < 0 || design_ini_result.height < 0) {
    qDebug() << "W: could not find \"music_name\" in courtroom_design.ini";
    ui_music_display->hide();
  }
  else {
    ui_music_display->move(design_ini_result.x, design_ini_result.y);
    ui_music_display->combo_resize(design_ini_result.width,
                                   design_ini_result.height);
  }

  ui_music_display->play("music_display");
  ui_music_display->set_play_once(false);

  if (is_ao2_bg) {
    set_size_and_pos(ui_ic_chat_message, "ao2_ic_chat_message");
    set_size_and_pos(ui_vp_chatbox, "ao2_chatbox");
    set_size_and_pos(ui_ic_chat_name, "ao2_ic_chat_name");
  }
  else {
    set_size_and_pos(ui_ic_chat_message, "ic_chat_message");
    set_size_and_pos(ui_vp_chatbox, "chatbox");
    set_size_and_pos(ui_ic_chat_name, "ic_chat_name");
  }

  ui_ic_chat_message->setStyleSheet(
      "QLineEdit{background-color: rgba(100, 100, 100, 255);}");
  ui_ic_chat_name->setStyleSheet(
      "QLineEdit{background-color: rgba(180, 180, 180, 255);}");

  ui_vp_chatbox->set_image("chatblank");
  ui_vp_chatbox->hide();

  set_size_and_pos(ui_vp_showname, "showname");

  set_size_and_pos(ui_vp_message, "message");
  ui_vp_message->hide();

  // We detached the text as parent from the chatbox so it doesn't get affected
  // by the screenshake.
  ui_vp_message->move(ui_vp_message->x() + ui_vp_chatbox->x(),
                      ui_vp_message->y() + ui_vp_chatbox->y());
  ui_vp_message->setTextInteractionFlags(Qt::NoTextInteraction);

  ui_muted->resize(ui_ic_chat_message->width(), ui_ic_chat_message->height());
  ui_muted->set_image("muted");
  ui_muted->setToolTip(tr("Oops, you're muted!"));

  set_size_and_pos(ui_ooc_chat_message, "ooc_chat_message");
  ui_ooc_chat_message->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_ooc_chat_name, "ooc_chat_name");
  ui_ooc_chat_name->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  // set_size_and_pos(ui_area_password, "area_password");
  set_size_and_pos(ui_music_search, "music_search");
  ui_music_search->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_emote_dropdown, "emote_dropdown");
  ui_emote_dropdown->setToolTip(
      tr("Set your character's emote to play on your next message."));

  set_size_and_pos(ui_pos_dropdown, "pos_dropdown");
  ui_pos_dropdown->setToolTip(
      tr("Set your character's supplementary background."));

  set_size_and_pos(ui_iniswap_dropdown, "iniswap_dropdown");
  ui_iniswap_dropdown->setEditable(true);
  ui_iniswap_dropdown->setInsertPolicy(QComboBox::InsertAtBottom);
  ui_iniswap_dropdown->setToolTip(
      tr("Set an 'iniswap', or an alternative character folder to refer to "
         "from your current character.\n"
         "Edit by typing and pressing Enter, [X] to remove. This saves to your "
         "base/characters/<charname>/iniswaps.ini"));

  set_size_and_pos(ui_iniswap_remove, "iniswap_remove");
  ui_iniswap_remove->setText("X");
  ui_iniswap_remove->set_image("evidencex");
  ui_iniswap_remove->setToolTip(
      tr("Remove the currently selected iniswap from the list and return to "
         "the original character folder."));
  ui_iniswap_remove->hide();

  set_size_and_pos(ui_sfx_dropdown, "sfx_dropdown");
  ui_sfx_dropdown->setEditable(true);
  ui_sfx_dropdown->setInsertPolicy(QComboBox::InsertAtBottom);
  ui_sfx_dropdown->setToolTip(
      tr("Set a sound effect to play on your next 'Preanim'. Leaving it on "
         "Default will use the emote-defined sound (if any).\n"
         "Edit by typing and pressing Enter, [X] to remove. This saves to your "
         "base/characters/<charname>/soundlist.ini"));

  set_size_and_pos(ui_sfx_remove, "sfx_remove");
  ui_sfx_remove->setText("X");
  ui_sfx_remove->set_image("evidencex");
  ui_sfx_remove->setToolTip(
      tr("Remove the currently selected sound effect."));
  ui_sfx_remove->hide();

  set_iniswap_dropdown();

  set_size_and_pos(ui_effects_dropdown, "effects_dropdown");
  ui_effects_dropdown->setInsertPolicy(QComboBox::InsertAtBottom);
  ui_effects_dropdown->setToolTip(
      tr("Choose an effect to play on your next spoken message.\n"
         "The effects are defined in your theme/effects/effects.ini. Your "
         "character can define custom effects by\n"
         "char.ini [Options] category, effects = 'miscname' where it referes "
         "to misc/<miscname>/effects.ini to read the effects."));
  // Todo: recode this entire fucking system with these dumbass goddamn ini's
  // why is everything so specifically coded for all these purposes is ABSTRACT
  // CODING not a thing now huh what the FUCK why do I gotta do this pleASE FOR
  // THE LOVE OF GOD SPARE ME FROM THIS FRESH HELL btw i still love coding.
  QPoint p_point = ao_app->get_button_spacing("effects_icon_size", filename);
  ui_effects_dropdown->setIconSize(QSize(p_point.x(), p_point.y()));

  set_size_and_pos(ui_defense_bar, "defense_bar");
  ui_defense_bar->set_image("defensebar" + QString::number(defense_bar_state));

  set_size_and_pos(ui_prosecution_bar, "prosecution_bar");
  ui_prosecution_bar->set_image("prosecutionbar" +
                                QString::number(prosecution_bar_state));

  set_size_and_pos(ui_music_label, "music_label");
  ui_music_label->setText(tr("Music"));
  set_size_and_pos(ui_sfx_label, "sfx_label");
  ui_sfx_label->setText(tr("Sfx"));
  set_size_and_pos(ui_blip_label, "blip_label");
  ui_blip_label->setText(tr("Blips"));

  set_size_and_pos(ui_hold_it, "hold_it");
  ui_hold_it->setText(tr("Hold It!"));
  ui_hold_it->setToolTip(tr("When this is turned on, your next in-character "
                            "message will be a shout!"));
  ui_hold_it->set_image("holdit");

  set_size_and_pos(ui_objection, "objection");
  ui_objection->setText(tr("Objection!"));
  ui_objection->setToolTip(tr("When this is turned on, your next in-character "
                              "message will be a shout!"));
  ui_objection->set_image("objection");

  set_size_and_pos(ui_take_that, "take_that");
  ui_take_that->setText(tr("Take That!"));
  ui_take_that->setToolTip(tr("When this is turned on, your next in-character "
                              "message will be a shout!"));
  ui_take_that->set_image("takethat");

  set_size_and_pos(ui_ooc_toggle, "ooc_toggle");
  ui_ooc_toggle->setText(tr("Server"));
  ui_ooc_toggle->setToolTip(
      tr("Toggle between server chat and global AO2 chat."));

  set_size_and_pos(ui_witness_testimony, "witness_testimony");
  ui_witness_testimony->set_image("witnesstestimony");
  ui_witness_testimony->setToolTip(tr("This will display the animation in the "
                                      "viewport as soon as it is pressed."));
  set_size_and_pos(ui_cross_examination, "cross_examination");
  ui_cross_examination->set_image("crossexamination");
  ui_cross_examination->setToolTip(tr("This will display the animation in the "
                                      "viewport as soon as it is pressed."));

  set_size_and_pos(ui_guilty, "guilty");
  ui_guilty->setText(tr("Guilty!"));
  ui_guilty->set_image("guilty");
  ui_guilty->setToolTip(tr("This will display the animation in the viewport as "
                           "soon as it is pressed."));
  set_size_and_pos(ui_not_guilty, "not_guilty");
  ui_not_guilty->set_image("notguilty");
  ui_not_guilty->setToolTip(tr("This will display the animation in the "
                               "viewport as soon as it is pressed."));

  set_size_and_pos(ui_change_character, "change_character");
  ui_change_character->setText(tr("Change character"));
  ui_change_character->set_image("change_character");
  ui_change_character->setToolTip(
      tr("Bring up the Character Select Screen and change your character."));

  set_size_and_pos(ui_reload_theme, "reload_theme");
  ui_reload_theme->setText(tr("Reload theme"));
  ui_reload_theme->set_image("reload_theme");
  ui_reload_theme->setToolTip(
      tr("Refresh the theme and update all of the ui elements to match."));

  set_size_and_pos(ui_call_mod, "call_mod");
  ui_call_mod->setText(tr("Call mod"));
  ui_call_mod->set_image("call_mod");
  ui_call_mod->setToolTip(
      tr("Request the attention of the current server's moderator."));

  set_size_and_pos(ui_settings, "settings");
  ui_settings->setText(tr("Settings"));
  ui_settings->set_image("settings");
  ui_settings->setToolTip(
      tr("Allows you to change various aspects of the client."));

  set_size_and_pos(ui_announce_casing, "casing_button");
  ui_announce_casing->setText(tr("Casing"));
  ui_announce_casing->set_image("casing_button");
  ui_announce_casing->setToolTip(
      tr("An interface to help you announce a case (you have to be a CM first "
         "to be able to announce cases)"));

  set_size_and_pos(ui_switch_area_music, "switch_area_music");
  ui_switch_area_music->setText(tr("A/M"));
  ui_switch_area_music->set_image("switch_area_music");
  ui_switch_area_music->setToolTip(tr("Switch between Areas and Music lists"));

  set_size_and_pos(ui_pre, "pre");
  ui_pre->setText(tr("Preanim"));
  ui_pre->setToolTip(
      tr("Play a single-shot animation as defined by the emote when checked."));

  set_size_and_pos(ui_pre_non_interrupt, "pre_no_interrupt");
  ui_pre_non_interrupt->setToolTip(
      tr("If preanim is checked, display the input text immediately as the "
         "animation plays concurrently."));

  set_size_and_pos(ui_flip, "flip");
  ui_flip->setToolTip(tr("Mirror your character's emotes when checked."));

  set_size_and_pos(ui_additive, "additive");
  ui_additive->setToolTip(
      tr("Add text to your last spoken message when checked."));

  set_size_and_pos(ui_guard, "guard");
  ui_guard->setToolTip(
      tr("Do not listen to mod calls when checked, preventing them from "
         "playing sounds or focusing attention on the window."));

  set_size_and_pos(ui_casing, "casing");
  ui_casing->setToolTip(tr("Lets you receive case alerts when enabled.\n"
                           "(You can set your preferences in the Settings!)"));

  set_size_and_pos(ui_showname_enable, "showname_enable");
  ui_showname_enable->setToolTip(
      tr("Display customized shownames for all users when checked."));

  set_size_and_pos(ui_custom_objection, "custom_objection");
  ui_custom_objection->setText(tr("Custom Shout!"));
  ui_custom_objection->set_image("custom");
  ui_custom_objection->setToolTip(
      tr("This will display the custom character-defined animation in the "
         "viewport as soon as it is pressed.\n"
         "To make one, your character's folder must contain "
         "custom.[webp/apng/gif/png] and custom.[wav/ogg/opus] sound effect"));

  set_size_and_pos(ui_realization, "realization");
  ui_realization->set_image("realization");
  ui_realization->setToolTip(
      tr("Play realization sound and animation in the viewport on the next "
         "spoken message when checked."));

  set_size_and_pos(ui_screenshake, "screenshake");
  ui_screenshake->set_image("screenshake");
  ui_screenshake->setToolTip(
      tr("Shake the screen on next spoken message when checked."));

  set_size_and_pos(ui_mute, "mute_button");
  ui_mute->setText("Mute");
  ui_mute->set_image("mute");
  ui_mute->setToolTip(
      tr("Display the list of character folders you wish to mute."));

  set_size_and_pos(ui_defense_plus, "defense_plus");
  ui_defense_plus->set_image("defplus");
  ui_defense_plus->setToolTip(tr("Increase the health bar."));

  set_size_and_pos(ui_defense_minus, "defense_minus");
  ui_defense_minus->set_image("defminus");
  ui_defense_minus->setToolTip(tr("Decrease the health bar."));

  set_size_and_pos(ui_prosecution_plus, "prosecution_plus");
  ui_prosecution_plus->set_image("proplus");
  ui_prosecution_plus->setToolTip(tr("Increase the health bar."));

  set_size_and_pos(ui_prosecution_minus, "prosecution_minus");
  ui_prosecution_minus->set_image("prominus");
  ui_prosecution_minus->setToolTip(tr("Decrease the health bar."));

  set_size_and_pos(ui_text_color, "text_color");
  ui_text_color->setToolTip(
      tr("Change the text color of the spoken message.\n"
         "You can also select a part of your currently typed message and use "
         "the dropdown to change its color!"));
  set_text_color_dropdown();

  set_size_and_pos(ui_music_slider, "music_slider");
  set_size_and_pos(ui_sfx_slider, "sfx_slider");
  set_size_and_pos(ui_blip_slider, "blip_slider");

  ui_selector->set_image("char_selector");
  ui_selector->hide();

  set_size_and_pos(ui_back_to_lobby, "back_to_lobby");
  ui_back_to_lobby->setText(tr("Back to Lobby"));
  ui_back_to_lobby->setToolTip(tr("Return back to the server list."));

  set_size_and_pos(ui_char_password, "char_password");

  set_size_and_pos(ui_char_buttons, "char_buttons");

  set_size_and_pos(ui_char_select_left, "char_select_left");
  ui_char_select_left->set_image("arrow_left");

  set_size_and_pos(ui_char_select_right, "char_select_right");
  ui_char_select_right->set_image("arrow_right");

  set_size_and_pos(ui_spectator, "spectator");
  ui_spectator->setToolTip(tr("Become a spectator. You won't be able to "
                              "interact with the in-character screen."));

  free_brush =
      QBrush(ao_app->get_color("area_free_color", "courtroom_design.ini"));
  lfp_brush =
      QBrush(ao_app->get_color("area_lfp_color", "courtroom_design.ini"));
  casing_brush =
      QBrush(ao_app->get_color("area_casing_color", "courtroom_design.ini"));
  recess_brush =
      QBrush(ao_app->get_color("area_recess_color", "courtroom_design.ini"));
  rp_brush = QBrush(ao_app->get_color("area_rp_color", "courtroom_design.ini"));
  gaming_brush =
      QBrush(ao_app->get_color("area_gaming_color", "courtroom_design.ini"));
  locked_brush =
      QBrush(ao_app->get_color("area_locked_color", "courtroom_design.ini"));

  refresh_evidence();
}

void Courtroom::set_fonts(QString p_char)
{
  set_font(ui_vp_showname, "", "showname", p_char);
  set_font(ui_vp_message, "", "message", p_char);
  set_font(ui_ic_chatlog, "", "ic_chatlog", p_char);
  set_font(ui_ms_chatlog, "", "ms_chatlog", p_char);
  set_font(ui_server_chatlog, "", "server_chatlog", p_char);
  set_font(ui_music_list, "", "music_list", p_char);
  set_font(ui_area_list, "", "area_list", p_char);
  set_font(ui_music_name, "", "music_name", p_char);

  set_dropdowns();
}

void Courtroom::set_font(QWidget *widget, QString class_name,
                         QString p_identifier, QString p_char,
                         QString font_name, int f_pointsize)
{
  QString design_file = "courtroom_fonts.ini";
  if (f_pointsize <= 0)
    f_pointsize =
        ao_app->get_design_element(p_identifier, design_file, p_char).toInt();
  if (font_name == "")
    font_name =
        ao_app->get_design_element(p_identifier + "_font", design_file, p_char);
  QString f_color_result =
      ao_app->get_design_element(p_identifier + "_color", design_file, p_char);
  QColor f_color(0, 0, 0);
  if (f_color_result != "") {
    QStringList color_list = f_color_result.split(",");

    if (color_list.size() >= 3) {
      f_color.setRed(color_list.at(0).toInt());
      f_color.setGreen(color_list.at(1).toInt());
      f_color.setBlue(color_list.at(2).toInt());
    }
  }
  bool bold =
      ao_app->get_design_element(p_identifier + "_bold", design_file, p_char) ==
      "1"; // is the font bold or not?
  bool antialias = ao_app->get_design_element(p_identifier + "_sharp",
                                              design_file, p_char) !=
                   "1"; // is the font anti-aliased or not?

  this->set_qfont(widget, class_name,
                  get_qfont(font_name, f_pointsize, antialias), f_color, bold);
}

QFont Courtroom::get_qfont(QString font_name, int f_pointsize, bool antialias)
{
  QFont font;
  if (font_name.isEmpty())
    font_name = "Arial";

  QFont::StyleStrategy style_strategy = QFont::PreferDefault;
  if (!antialias)
    style_strategy = QFont::NoAntialias;

  font = QFont(font_name, f_pointsize);
  font.setStyleHint(QFont::SansSerif, style_strategy);
  return font;
}

void Courtroom::set_qfont(QWidget *widget, QString class_name, QFont font,
                          QColor f_color, bool bold)
{
  if (class_name.isEmpty())
    class_name = widget->metaObject()->className();

  font.setBold(bold);
  widget->setFont(font);

  QString style_sheet_string =
      class_name + " { background-color: rgba(0, 0, 0, 0);\n" + "color: rgba(" +
      QString::number(f_color.red()) + ", " + QString::number(f_color.green()) +
      ", " + QString::number(f_color.blue()) + ", 255);}";
  widget->setStyleSheet(style_sheet_string);
}

void Courtroom::set_dropdown(QWidget *widget)
{
  QString f_file = "courtroom_stylesheets.css";
  QString style_sheet_string = ao_app->get_stylesheet(f_file);
  if (style_sheet_string != "")
    widget->setStyleSheet(style_sheet_string);
}

void Courtroom::set_dropdowns()
{
  set_dropdown(
      this); // EXPERIMENTAL - Read the style-sheet as-is for maximum memeage
  //  set_dropdown(ui_text_color, "[TEXT COLOR]");
  //  set_dropdown(ui_pos_dropdown, "[POS DROPDOWN]");
  //  set_dropdown(ui_emote_dropdown, "[EMOTE DROPDOWN]");
  //  set_dropdown(ui_mute_list, "[MUTE LIST]");
}

void Courtroom::set_window_title(QString p_title)
{
  this->setWindowTitle(p_title);
}

void Courtroom::set_size_and_pos(QWidget *p_widget, QString p_identifier)
{
  QString filename = "courtroom_design.ini";

  pos_size_type design_ini_result =
      ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0) {
    qDebug() << "W: could not find \"" << p_identifier << "\" in " << filename;
    p_widget->hide();
  }
  else {
    p_widget->move(design_ini_result.x, design_ini_result.y);
    p_widget->resize(design_ini_result.width, design_ini_result.height);
  }
}

void Courtroom::set_taken(int n_char, bool p_taken)
{
  if (n_char >= char_list.size()) {
    qDebug()
        << "W: set_taken attempted to set an index bigger than char_list size";
    return;
  }

  char_type f_char;
  f_char.name = char_list.at(n_char).name;
  f_char.description = char_list.at(n_char).description;
  f_char.taken = p_taken;
  f_char.evidence_string = char_list.at(n_char).evidence_string;

  char_list.replace(n_char, f_char);
}

QPoint Courtroom::get_theme_pos(QString p_identifier)
{
  QString filename = "courtroom_design.ini";

  pos_size_type design_ini_result =
      ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0) {
    qDebug() << "W: could not find \"" << p_identifier << "\" in " << filename;
    return QPoint(0, 0);
  }
  else {
    return QPoint(design_ini_result.x, design_ini_result.y);
  }
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

void Courtroom::set_background(QString p_background, bool display)
{
  ui_vp_testimony->stop();
  current_background = p_background;

  // welcome to hardcode central may I take your order of regularly scheduled
  // CBT
  QMap<QString, QString> default_pos;
  default_pos["defenseempty"] = "def";
  default_pos["helperstand"] = "hld";
  default_pos["prosecutorempty"] = "pro";
  default_pos["prohelperstand"] = "hlp";
  default_pos["witnessempty"] = "wit";
  default_pos["judgestand"] = "jud";
  default_pos["jurystand"] = "jur";
  default_pos["seancestand"] = "sea";

  // Populate the dropdown list with all pos that exist on this bg
  QStringList pos_list = {};
  for (QString key : default_pos.keys()) {
    if (file_exists(
            ao_app->get_image_suffix(ao_app->get_background_path(key)))) {
      pos_list.append(default_pos[key]);
    }
  }

  // TODO: search through extra/custom pos and add them to the pos dropdown as
  // well

  set_pos_dropdown(pos_list);

  is_ao2_bg = true;

  if (is_ao2_bg) {
    set_size_and_pos(ui_vp_chatbox, "ao2_chatbox");
    set_size_and_pos(ui_ic_chat_message, "ao2_ic_chat_message");
  }
  else {
    set_size_and_pos(ui_vp_chatbox, "chatbox");
    set_size_and_pos(ui_ic_chat_message, "ic_chat_message");
  }

  if (display) {
    ui_vp_speedlines->stop();
    ui_vp_player_char->stop();

    ui_vp_sideplayer_char->stop();
    ui_vp_effect->stop();
    ui_vp_message->hide();
    ui_vp_chatbox->hide();

    // Stop the chat arrow from animating
    ui_vp_chat_arrow->stop();

    text_state = 2;
    anim_state = 3;
    ui_vp_objection->stop();
    chat_tick_timer->stop();
    ui_vp_evidence_display->reset();
    set_scene(
        QString::number(ao_app->get_desk_mod(current_char, current_emote)),
        current_side);
  }
}

void Courtroom::set_side(QString p_side)
{
  if (p_side == "")
    current_side = ao_app->get_char_side(current_char);
  else
    current_side = p_side;

  for (int i = 0; i < ui_pos_dropdown->count(); ++i) {
    QString pos = ui_pos_dropdown->itemText(i);
    if (pos == current_side) {
      // Block the signals to prevent setCurrentIndex from triggering a pos
      // change
      ui_pos_dropdown->blockSignals(true);

      // Set the index on dropdown ui element to let you know what pos you're on
      // right now
      ui_pos_dropdown->setCurrentIndex(i);

      // Unblock the signals so the element can be used for setting pos again
      ui_pos_dropdown->blockSignals(false);

      // alright we dun, jobs done here boyos
      break;
    }
  }
}

void Courtroom::set_pos_dropdown(QStringList pos_dropdowns)
{
  // Block the signals to prevent setCurrentIndex from triggering a pos change
  ui_pos_dropdown->blockSignals(true);
  pos_dropdown_list = pos_dropdowns;
  ui_pos_dropdown->clear();
  ui_pos_dropdown->addItems(pos_dropdown_list);
  // Unblock the signals so the element can be used for setting pos again
  ui_pos_dropdown->blockSignals(false);

  qDebug() << pos_dropdown_list;
}

void Courtroom::update_character(int p_cid)
{
  bool newchar = m_cid != p_cid;

  m_cid = p_cid;

  QString f_char;

  if (m_cid == -1) {
    if (ao_app->is_discord_enabled())
      ao_app->discord->state_spectate();
    f_char = "";
  }
  else {
    f_char = ao_app->get_char_name(char_list.at(m_cid).name);

    if (ao_app->is_discord_enabled())
      ao_app->discord->state_character(f_char.toStdString());
  }

  current_char = f_char;
  current_side = ao_app->get_char_side(current_char);

  set_text_color_dropdown();

  current_emote_page = 0;
  current_emote = 0;

  if (m_cid == -1)
    ui_emotes->hide();
  else
    ui_emotes->show();

  refresh_emotes();
  set_emote_page();
  set_emote_dropdown();

  set_sfx_dropdown();
  set_effects_dropdown();

  qDebug() << "update_character called";
  if (newchar) // Avoid infinite loop of death and suffering
    set_iniswap_dropdown();

  if (current_side == "jud") {
    ui_witness_testimony->show();
    ui_cross_examination->show();
    ui_not_guilty->show();
    ui_guilty->show();
    ui_defense_minus->show();
    ui_defense_plus->show();
    ui_prosecution_minus->show();
    ui_prosecution_plus->show();
  }
  else {
    ui_witness_testimony->hide();
    ui_cross_examination->hide();
    ui_guilty->hide();
    ui_not_guilty->hide();
    ui_defense_minus->hide();
    ui_defense_plus->hide();
    ui_prosecution_minus->hide();
    ui_prosecution_plus->hide();
  }

  ui_custom_objection->hide();
  if (ao_app->custom_objection_enabled) // if setting is enabled
  {
    custom_obj_menu->clear();
    if (file_exists(ao_app->get_image_suffix(
            ao_app->get_character_path(current_char, "custom")))) {
      ui_custom_objection->show();
      QAction *action = custom_obj_menu->addAction("Default");
      custom_obj_menu->setDefaultAction(action);
      objection_custom = "";
    }
    if (dir_exists(
            ao_app->get_character_path(current_char, "custom_objections"))) {
      ui_custom_objection->show();
      QDir directory(
          ao_app->get_character_path(current_char, "custom_objections"));
      QStringList custom_obj = directory.entryList(QStringList() << "*.png"
                                                                 << "*.gif"
                                                                 << "*.apng"
                                                                 << "*.webp",
                                                   QDir::Files);
      for (const QString &filename : custom_obj) {
        QAction *action = custom_obj_menu->addAction(filename);
        if (custom_obj_menu->defaultAction() == nullptr) {
          custom_obj_menu->setDefaultAction(action);
          objection_custom = action->text();
        }
      }
    }
  }

  if (m_cid != -1) // there is no name at char_list -1, and we crash if we try
                   // to find one
    ui_ic_chat_name->setPlaceholderText(char_list.at(m_cid).name);
  ui_char_select_background->hide();
  ui_ic_chat_message->setEnabled(m_cid != -1);
  ui_ic_chat_message->setFocus();
}

void Courtroom::enter_courtroom()
{
  set_widgets();

  current_evidence_page = 0;
  current_evidence = 0;

  set_evidence_page();

  if (ao_app->flipping_enabled)
    ui_flip->show();
  else
    ui_flip->hide();

  if (ao_app->additive_enabled)
    ui_additive->show();
  else
    ui_additive->hide();

  if (ao_app->casing_alerts_enabled)
    ui_casing->show();
  else
    ui_casing->hide();

  list_music();
  list_areas();

  switch (
      objection_state) // no need to reset these as it was done in set_widgets()
  {
  case 1:
    ui_hold_it->set_image("holdit_selected");
    break;
  case 2:
    ui_objection->set_image("objection_selected");
    break;
  case 3:
    ui_take_that->set_image("takethat_selected");
    break;
  case 4:
    ui_custom_objection->set_image("custom_selected");
    break;
  default:
    break;
  }

  music_player->set_volume(ui_music_slider->value(), 0); // set music
  // Set the ambience and other misc. music layers
  for (int i = 1; i < music_player->m_channelmax; ++i) {
    music_player->set_volume(ui_sfx_slider->value(), i);
  }
  sfx_player->set_volume(ui_sfx_slider->value());
  objection_player->set_volume(ui_sfx_slider->value());
  blip_player->set_volume(ui_blip_slider->value());

  ui_vp_testimony->stop();
  // ui_server_chatlog->setHtml(ui_server_chatlog->toHtml());
}

// Todo: multithread this due to some servers having large as hell music list
void Courtroom::list_music()
{
  ui_music_list->clear();
  //  ui_music_search->setText("");

  QString f_file = "courtroom_design.ini";

  QBrush found_brush(ao_app->get_color("found_song_color", f_file));
  QBrush missing_brush(ao_app->get_color("missing_song_color", f_file));

  int n_listed_songs = 0;

  QTreeWidgetItem *parent = nullptr;
  for (int n_song = 0; n_song < music_list.size(); ++n_song) {
    QString i_song = music_list.at(n_song);
    QString i_song_listname = i_song.left(i_song.lastIndexOf("."));
    i_song_listname = i_song_listname.right(
        i_song_listname.length() - (i_song_listname.lastIndexOf("/") + 1));

    QTreeWidgetItem *treeItem;
    if (i_song_listname != i_song &&
        parent != nullptr) // not a category, parent exists
      treeItem = new QTreeWidgetItem(parent);
    else
      treeItem = new QTreeWidgetItem(ui_music_list);
    treeItem->setText(0, i_song_listname);
    treeItem->setText(1, i_song);

    QString song_path = ao_app->get_music_path(i_song);

    if (file_exists(song_path))
      treeItem->setBackground(0, found_brush);
    else
      treeItem->setBackground(0, missing_brush);

    if (i_song_listname ==
        i_song) // Not supposed to be a song to begin with - a category?
      parent = treeItem;
    ++n_listed_songs;
  }

  ui_music_list->expandAll(); // Needs to somehow remember which categories were
                              // expanded/collapsed if the music list didn't
                              // change since last time
  if (ui_music_search->text() != "") {
    on_music_search_edited(ui_music_search->text());
  }
}

// Todo: multithread this due to some servers having large as hell area list
void Courtroom::list_areas()
{
  ui_area_list->clear();
  //  ui_music_search->setText("");

  int n_listed_areas = 0;

  for (int n_area = 0; n_area < area_list.size(); ++n_area) {
    QString i_area = "";
    i_area.append(area_list.at(n_area));

    if (ao_app->arup_enabled) {
      i_area.prepend("[" + QString::number(n_area) + "] "); // Give it the index

      i_area.append("\n  ");

      i_area.append(arup_statuses.at(n_area));
      i_area.append(" | CM: ");
      i_area.append(arup_cms.at(n_area));

      i_area.append("\n  ");

      i_area.append(QString::number(arup_players.at(n_area)));
      i_area.append(" users | ");

      i_area.append(arup_locks.at(n_area));
    }

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui_area_list);
    treeItem->setText(0, area_list.at(n_area));
    treeItem->setText(1, i_area);

    if (ao_app->arup_enabled) {
      // Coloring logic here.
      treeItem->setBackground(1, free_brush);
      if (arup_locks.at(n_area) == "LOCKED") {
        treeItem->setBackground(1, locked_brush);
      }
      else {
        if (arup_statuses.at(n_area) == "LOOKING-FOR-PLAYERS")
          treeItem->setBackground(1, lfp_brush);
        else if (arup_statuses.at(n_area) == "CASING")
          treeItem->setBackground(1, casing_brush);
        else if (arup_statuses.at(n_area) == "RECESS")
          treeItem->setBackground(1, recess_brush);
        else if (arup_statuses.at(n_area) == "RP")
          treeItem->setBackground(1, rp_brush);
        else if (arup_statuses.at(n_area) == "GAMING")
          treeItem->setBackground(1, gaming_brush);
      }
    }
    else {
      treeItem->setBackground(1, free_brush);
    }

    ++n_listed_areas;
  }

  if (ui_music_search->text() != "") {
    on_music_search_edited(ui_music_search->text());
  }
}

void Courtroom::append_ms_chatmessage(QString f_name, QString f_message)
{
  ui_ms_chatlog->append_chatmessage(
      f_name, f_message,
      ao_app->get_color("ms_chatlog_sender_color", "courtroom_fonts.ini")
          .name());
}

void Courtroom::append_server_chatmessage(QString p_name, QString p_message,
                                          QString p_color)
{
  QString color = "#000000";

  if (p_color == "0")
    color = ao_app->get_color("ms_chatlog_sender_color", "courtroom_fonts.ini")
                .name();
  if (p_color == "1")
    color =
        ao_app->get_color("server_chatlog_sender_color", "courtroom_fonts.ini")
            .name();
  if (p_message == "Logged in as a moderator.") {
    ui_guard->show();
    append_server_chatmessage(
        tr("CLIENT"), tr("You were granted the Disable Modcalls button."), "1");
  }

  ui_server_chatlog->append_chatmessage(p_name, p_message, color);
}

void Courtroom::on_chat_return_pressed()
{
  if (is_muted)
    return;

  if ((anim_state < 3 || text_state < 2) && objection_state == 0)
    return;

  ui_ic_chat_message->blockSignals(true);
  QTimer::singleShot(600, this,
                     [=] { ui_ic_chat_message->blockSignals(false); });
  // MS#
  // deskmod#
  // pre-emote#
  // character#
  // emote#
  // message#
  // side#
  // sfx-name#
  // emote_modifier#
  // char_id#
  // sfx_delay#
  // objection_modifier#
  // evidence#
  // placeholder#
  // realization#
  // text_color#%

  // Additionally, in our case:

  // showname#
  // other_charid#
  // self_offset#
  // noninterrupting_preanim#%

  QStringList packet_contents;

  if (current_side == "")
    current_side = ao_app->get_char_side(current_char);

  QString f_desk_mod = "chat";

  if (ao_app->desk_mod_enabled) {
    f_desk_mod =
        QString::number(ao_app->get_desk_mod(current_char, current_emote));
    if (f_desk_mod == "-1")
      f_desk_mod = "chat";
  }

  packet_contents.append(f_desk_mod);

  packet_contents.append(ao_app->get_pre_emote(current_char, current_emote));

  packet_contents.append(current_char);

  packet_contents.append(ao_app->get_emote(current_char, current_emote));

  packet_contents.append(ui_ic_chat_message->text());

  packet_contents.append(current_side);

  packet_contents.append(get_char_sfx());
  if (ui_pre->isChecked() && !ao_app->is_stickysounds_enabled()) {
    ui_sfx_dropdown->blockSignals(true);
    ui_sfx_dropdown->setCurrentIndex(0);
    ui_sfx_dropdown->blockSignals(false);
    ui_sfx_remove->hide();
  }

  int f_emote_mod = ao_app->get_emote_mod(current_char, current_emote);

  // needed or else legacy won't understand what we're saying
  if (objection_state > 0 && ui_pre->isChecked()) {
    if (f_emote_mod == 4 || f_emote_mod == 5)
      f_emote_mod = 6;
    else
      f_emote_mod = 2;
  }
  else if (ui_pre->isChecked() && !ui_pre_non_interrupt->isChecked()) {
    if (f_emote_mod == 0)
      f_emote_mod = 1;
    else if (f_emote_mod == 5 && ao_app->prezoom_enabled)
      f_emote_mod = 6;
  }
  else {
    if (f_emote_mod == 1)
      f_emote_mod = 0;
    else if (f_emote_mod == 4)
      f_emote_mod = 5;
  }

  packet_contents.append(QString::number(f_emote_mod));
  packet_contents.append(QString::number(m_cid));

  packet_contents.append(QString::number(get_char_sfx_delay()));

  QString f_obj_state;

  if ((objection_state == 4 && !ao_app->custom_objection_enabled) ||
      (objection_state < 0))
    f_obj_state = "0";
  else if (objection_custom != "" && objection_state == 4) {
    f_obj_state = QString::number(objection_state) + "&" +
                  objection_custom; // we add the name of the objection so the
                                    // packet is like: 4&(name of custom obj)
  }
  else
    f_obj_state = QString::number(objection_state);

  packet_contents.append(f_obj_state);

  if (is_presenting_evidence)
    // the evidence index is shifted by 1 because 0 is no evidence per legacy
    // standards besides, older clients crash if we pass -1
    packet_contents.append(QString::number(current_evidence + 1));
  else
    packet_contents.append("0");

  QString f_flip;

  if (ao_app->flipping_enabled) {
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
  else if (text_color >= max_colors)
    f_text_color = "0";
  else
    f_text_color = QString::number(text_color);

  packet_contents.append(f_text_color);

  // If the server we're on supports CCCC stuff, we should use it!
  if (ao_app->cccc_ic_support_enabled) {
    // If there is a showname entered, use that -- else, just send an empty
    // packet-part.
    if (!ui_ic_chat_name->text().isEmpty()) {
      packet_contents.append(ui_ic_chat_name->text());
    }
    else {
      packet_contents.append("");
    }

    // Similarly, we send over whom we're paired with, unless we have chosen
    // ourselves. Or a charid of -1 or lower, through some means.
    if (other_charid > -1 && other_charid != m_cid) {
      QString packet = QString::number(other_charid);
      if (ao_app->effects_enabled) // Only servers with effects enabled will
                                   // support pair reordering
        packet += "^" + QString::number(pair_order);
      packet_contents.append(packet);
    }
    else {
      packet_contents.append("-1");
    }
    // Send the offset as it's gonna be used regardless
    packet_contents.append(QString::number(char_offset) + "&" + QString::number(char_vert_offset));

    // Finally, we send over if we want our pres to not interrupt.
    if (ui_pre_non_interrupt->isChecked() && ui_pre->isChecked()) {
      packet_contents.append("1");
    }
    else {
      packet_contents.append("0");
    }
  }

  // If the server we're on supports Looping SFX and Screenshake, use it if the
  // emote uses it.
  if (ao_app->looping_sfx_support_enabled) {
    packet_contents.append(
        ao_app->get_sfx_looping(current_char, current_emote));
    packet_contents.append(QString::number(screenshake_state));

    QString pre_emote = ao_app->get_pre_emote(current_char, current_emote);
    QString emote = ao_app->get_emote(current_char, current_emote);
    QStringList emotes_to_check = {pre_emote, "(b)" + emote, "(a)" + emote};
    QStringList effects_to_check = {"_FrameScreenshake", "_FrameRealization",
                                    "_FrameSFX"};

    foreach (QString f_effect, effects_to_check) {
      QString packet;
      foreach (QString f_emote, emotes_to_check) {
        packet += f_emote;
        if (ao_app->is_frame_network_enabled()) {
          QString sfx_frames =
              ao_app
                  ->read_ini_tags(
                      ao_app->get_character_path(current_char, "char.ini"),
                      f_emote.append(f_effect))
                  .join("|");
          if (sfx_frames != "")
            packet += "|" + sfx_frames;
        }
        packet += "^";
      }
      packet_contents.append(packet);
    }
  }

  if (ao_app->additive_enabled) {
    packet_contents.append(ui_additive->isChecked() ? "1" : "0");
  }
  if (ao_app->effects_enabled) {
    QString fx_sound = ao_app->get_effect_sound(effect, current_char);
    QString p_effect =
        ao_app->read_char_ini(current_char, "effects", "Options");
    packet_contents.append(effect + "|" + p_effect + "|" + fx_sound);
    if (!ao_app->is_stickyeffects_enabled()) {
      ui_effects_dropdown->blockSignals(true);
      ui_effects_dropdown->setCurrentIndex(0);
      ui_effects_dropdown->blockSignals(false);
      effect = "";
    }
  }

  ao_app->send_server_packet(new AOPacket("MS", packet_contents));
}

void Courtroom::reset_ic()
{
  ui_vp_chat_arrow->stop();
  text_state = 0;
  anim_state = 0;
  evidence_presented = false;
  ui_vp_objection->stop();
  chat_tick_timer->stop();
  ui_vp_evidence_display->reset();
}

void Courtroom::reset_ui()
{
  ui_ic_chat_message->clear();
  if (ui_additive->isChecked())
    ui_ic_chat_message->insert(" ");
  objection_state = 0;
  realization_state = 0;
  screenshake_state = 0;
  is_presenting_evidence = false;
  if (!ao_app->is_stickypres_enabled())
    ui_pre->setChecked(false);
  ui_hold_it->set_image("holdit");
  ui_objection->set_image("objection");
  ui_take_that->set_image("takethat");
  ui_custom_objection->set_image("custom");
  ui_realization->set_image("realization");
  ui_screenshake->set_image("screenshake");
  ui_evidence_present->set_image("present");
}

void Courtroom::handle_chatmessage(QStringList *p_contents)
{
  // Instead of checking for whether a message has at least chatmessage_size
  // amount of packages, we'll check if it has at least 15.
  // That was the original chatmessage_size.
  if (p_contents->size() < MS_MINIMUM)
    return;

  for (int n_string = 0; n_string < MS_MAXIMUM; ++n_string) {
    // Note that we have added stuff that vanilla clients and servers simply
    // won't send. So now, we have to check if the thing we want even exists
    // amongst the packet's content. We also have to check if the server even
    // supports CCCC's IC features, or if it's just japing us. Also, don't
    // forget! A size 15 message will have indices from 0 to 14.
    if (n_string < p_contents->size() &&
        (n_string < MS_MINIMUM || ao_app->cccc_ic_support_enabled)) {
      m_chatmessage[n_string] = p_contents->at(n_string);
    }
    else {
      m_chatmessage[n_string] = "";
    }
  }

  int f_char_id = m_chatmessage[CHAR_ID].toInt();
  const bool is_spectator = (f_char_id == -1);

  if (f_char_id < -1 || f_char_id >= char_list.size())
    return;
  if (mute_map.value(m_chatmessage[CHAR_ID].toInt()))
    return;

  QString f_displayname;
  if (!is_spectator &&
      (m_chatmessage[SHOWNAME].isEmpty() || !ui_showname_enable->isChecked())) {
    // If the users is not a spectator and showname is disabled, use the
    // character's name
    f_displayname = ao_app->get_showname(char_list.at(f_char_id).name);
  }
  else {
    // Otherwise, use the showname
    f_displayname = m_chatmessage[SHOWNAME];
  }

  // If chatblank is enabled, use the character's name for logs
  if (f_displayname.trimmed().isEmpty())
    f_displayname = ao_app->get_showname(char_list.at(f_char_id).name);

  // Check if a custom objection is in use
  int objection_mod = 0;
  QString custom_objection = "";
  if (m_chatmessage[OBJECTION_MOD].contains("4&")) {
    objection_mod = 4;
    custom_objection = m_chatmessage[OBJECTION_MOD].split(
        "4&")[1]; // takes the name of custom objection.
  }
  else {
    objection_mod = m_chatmessage[OBJECTION_MOD].toInt();
  }

  // Reset IC display
  reset_ic();

  // Reset UI elements after client message gets sent
  if (m_chatmessage[CHAR_ID].toInt() == m_cid) {
    reset_ui();
  }

  QString f_charname = "";
  if (f_char_id >= 0)
    f_charname = ao_app->get_showname(char_list.at(f_char_id).name);

  if (m_chatmessage[MESSAGE].trimmed().isEmpty()) // User-created blankpost
  {
    m_chatmessage[MESSAGE] = ""; // Turn it into true blankpost
  }

  if (!m_chatmessage[MESSAGE].isEmpty() || ic_chatlog_history.isEmpty() ||
      ic_chatlog_history.last().get_message() != "") {
    log_ic_text(f_charname, f_displayname, m_chatmessage[MESSAGE], "",
                m_chatmessage[TEXT_COLOR].toInt());
    append_ic_text(m_chatmessage[MESSAGE], f_displayname, "",
                   m_chatmessage[TEXT_COLOR].toInt());
  }

  QString f_char = m_chatmessage[CHAR_NAME];
  QString f_custom_theme = ao_app->get_char_shouts(f_char);

  // if an objection is used
  if (objection_mod <= 4 && objection_mod >= 1) {
    switch (objection_mod) {
    case 1:
      ui_vp_objection->play("holdit_bubble", f_char, f_custom_theme, 724);
      objection_player->play("holdit", f_char, f_custom_theme, 0);
      break;
    case 2:
      ui_vp_objection->play("objection_bubble", f_char, f_custom_theme, 724);
      objection_player->play("objection", f_char, f_custom_theme, 0);
      if (ao_app->objection_stop_music())
        music_player->stop();
      break;
    case 3:
      ui_vp_objection->play("takethat_bubble", f_char, f_custom_theme, 724);
      objection_player->play("takethat", f_char, f_custom_theme, 0);
      break;
    // case 4 is AO2 only
    case 4:
      if (custom_objection != "") {
        ui_vp_objection->play("custom_objections/" + custom_objection, f_char,
                              f_custom_theme, shout_stay_time);
        objection_player->play("custom_objections/" +
                                   custom_objection.split('.')[0],
                               f_char, f_custom_theme, 0);
      }
      else {
        ui_vp_objection->play("custom", f_char, f_custom_theme,
                              shout_stay_time);
        objection_player->play("custom", f_char, f_custom_theme, 0);
      }
      m_chatmessage[EMOTE_MOD] = 1;
      break;
    }
    sfx_player->clear(); // Objection played! Cut all sfx.
  }
  else
    handle_chatmessage_2();
}

void Courtroom::objection_done() { handle_chatmessage_2(); }

void Courtroom::handle_chatmessage_2()
{
  ui_vp_speedlines->stop();
  ui_vp_player_char->stop();
  ui_vp_effect->stop();
  // Clear all looping sfx to prevent obnoxiousness
  sfx_player->loop_clear();

  if (!m_chatmessage[FRAME_SFX].isEmpty() &&
      ao_app->is_frame_network_enabled()) {
    // ORDER IS IMPORTANT!!
    QStringList netstrings = {m_chatmessage[FRAME_SCREENSHAKE],
                              m_chatmessage[FRAME_REALIZATION],
                              m_chatmessage[FRAME_SFX]};
    ui_vp_player_char->network_strings = netstrings;
  }
  else
    ui_vp_player_char->network_strings.clear();

  int f_charid = m_chatmessage[CHAR_ID].toInt();
  if (f_charid >= 0 &&
      (m_chatmessage[SHOWNAME].isEmpty() || !ui_showname_enable->isChecked())) {
    QString real_name = char_list.at(f_charid).name;

    QString f_showname = ao_app->get_showname(real_name);

    ui_vp_showname->setText(f_showname);
  }
  else {
    ui_vp_showname->setText(m_chatmessage[SHOWNAME]);
  }

  QString customchar;
  if (ao_app->is_customchat_enabled())
    customchar = m_chatmessage[CHAR_NAME];
  if (ui_vp_showname->text().trimmed().isEmpty()) // Whitespace showname
  {
    ui_vp_chatbox->set_image("chatblank");
  }
  else // Aw yeah dude do some showname magic
  {
    if (!ui_vp_chatbox->set_image("chat"))
      ui_vp_chatbox->set_image("chatbox");

    QFontMetrics fm(ui_vp_showname->font());
// Gotta support the slow paced ubuntu 18 STUCK IN 5.9.5!!
#if QT_VERSION > QT_VERSION_CHECK(5, 11, 0)
    int fm_width = fm.horizontalAdvance(ui_vp_showname->text());
#else
    int fm_width = fm.boundingRect((ui_vp_showname->text())).width();
#endif
    QString chatbox_path = ao_app->get_theme_path("chat");
    QString chatbox = ao_app->get_chat(customchar);

    if (chatbox != "" && ao_app->is_customchat_enabled()) {
      chatbox_path = ao_app->get_base_path() + "misc/" + chatbox + "/chat";
      if (!ui_vp_chatbox->set_chatbox(chatbox_path))
        ui_vp_chatbox->set_chatbox(chatbox_path + "box");
    }

    // This should probably be called only if any change from the last chat
    // arrow was actually detected.
    pos_size_type design_ini_result = ao_app->get_element_dimensions(
        "chat_arrow", "courtroom_design.ini", customchar);
    if (design_ini_result.width < 0 || design_ini_result.height < 0) {
      qDebug() << "W: could not find \"chat_arrow\" in courtroom_design.ini";
      ui_vp_chat_arrow->hide();
    }
    else {
      ui_vp_chat_arrow->move(design_ini_result.x, design_ini_result.y);
      ui_vp_chat_arrow->combo_resize(design_ini_result.width,
                                     design_ini_result.height);
    }

    pos_size_type default_width = ao_app->get_element_dimensions(
        "showname", "courtroom_design.ini", customchar);
    int extra_width =
        ao_app
            ->get_design_element("showname_extra_width", "courtroom_design.ini",
                                 customchar)
            .toInt();
    QString align = ao_app
                        ->get_design_element("showname_align",
                                             "courtroom_design.ini", customchar)
                        .toLower();
    if (align == "right")
      ui_vp_showname->setAlignment(Qt::AlignRight);
    else if (align == "center")
      ui_vp_showname->setAlignment(Qt::AlignHCenter);
    else if (align == "justify")
      ui_vp_showname->setAlignment(Qt::AlignHCenter);
    else
      ui_vp_showname->setAlignment(Qt::AlignLeft);

    if (extra_width > 0) {
      if (fm_width > default_width.width &&
          ui_vp_chatbox->set_chatbox(
              chatbox_path +
              "med")) // This text be big. Let's do some shenanigans.
      {
        ui_vp_showname->resize(default_width.width + extra_width,
                               ui_vp_showname->height());
        if (fm_width > ui_vp_showname->width() &&
            ui_vp_chatbox->set_chatbox(chatbox_path +
                                       "big")) // Biggest possible size for us.
        {
          ui_vp_showname->resize(
              static_cast<int>(default_width.width + (extra_width * 2)),
              ui_vp_showname->height());
        }
      }
      else
        ui_vp_showname->resize(default_width.width, ui_vp_showname->height());

      set_font(ui_vp_showname, "", "showname", customchar);
    }
    else {
      ui_vp_showname->resize(default_width.width, ui_vp_showname->height());
    }
  }

  ui_vp_message->hide();
  ui_vp_chatbox->hide();

  QString font_name;
  QString chatfont = ao_app->get_chat_font(m_chatmessage[CHAR_NAME]);
  if (chatfont != "")
    font_name = chatfont;

  int f_pointsize = 0;
  int chatsize = ao_app->get_chat_size(m_chatmessage[CHAR_NAME]);
  if (chatsize > 0)
    f_pointsize = chatsize;
  set_font(ui_vp_message, "", "message", customchar, font_name, f_pointsize);

  set_scene(m_chatmessage[DESK_MOD], m_chatmessage[SIDE]);

  int emote_mod = m_chatmessage[EMOTE_MOD].toInt();
  // Deal with invalid emote modifiers
  if (emote_mod != 0 && emote_mod != 1 && emote_mod != 2 && emote_mod != 5 &&
      emote_mod != 6) {
    if (emote_mod == 4)
      emote_mod = 6; // Addresses issue with an old bug that sent the wrong
                     // emote modifier for zoompre
    else
      emote_mod = 0;
  }

  if (ao_app->flipping_enabled && m_chatmessage[FLIP].toInt() == 1)
    ui_vp_player_char->set_flipped(true);
  else
    ui_vp_player_char->set_flipped(false);

  QString side = m_chatmessage[SIDE];

  // Making the second character appear.
  if (m_chatmessage[OTHER_CHARID].isEmpty()) {
    // If there is no second character, hide 'em
    ui_vp_sideplayer_char->stop();
    ui_vp_sideplayer_char->move(0, 0);
  }
  else {
    bool ok;
    int got_other_charid = m_chatmessage[OTHER_CHARID].split("^")[0].toInt(&ok);
    if (ok) {
      if (got_other_charid > -1) {
        // If there is, show them!
        ui_vp_sideplayer_char->show();
        QStringList other_offsets = m_chatmessage[OTHER_OFFSET].split("&");
        int other_offset;
        int other_offset_v;
        if (other_offsets.length() <= 1) {
          other_offset = m_chatmessage[OTHER_OFFSET].toInt();
          other_offset_v = 0;
        }
        else {
          other_offset = other_offsets[0].toInt();
          other_offset_v = other_offsets[1].toInt();
        }
        ui_vp_sideplayer_char->move(ui_viewport->width() * other_offset / 100,
                                    ui_viewport->height() * other_offset_v / 100);

        QStringList args = m_chatmessage[OTHER_CHARID].split("^");
        if (args.size() >
            1) // This ugly workaround is so we don't make an extra packet just
               // for this purpose. Rewrite pairing when?
        {
          // Change the order of appearance based on the pair order variable
          int order = args.at(1).toInt();
          switch (order) {
          case 0:
            ui_vp_sideplayer_char->stackUnder(ui_vp_player_char);
            break;
          case 1:
            ui_vp_player_char->stackUnder(ui_vp_sideplayer_char);
            break;
          default:
            break;
          }
        }

        // We should probably also play the other character's idle emote.
        if (ao_app->flipping_enabled && m_chatmessage[OTHER_FLIP].toInt() == 1)
          ui_vp_sideplayer_char->set_flipped(true);
        else
          ui_vp_sideplayer_char->set_flipped(false);
        ui_vp_sideplayer_char->play_idle(m_chatmessage[OTHER_NAME],
                                         m_chatmessage[OTHER_EMOTE]);
      }
      else {
        // If the server understands other characters, but there
        // really is no second character, hide 'em, and center the first.
        ui_vp_sideplayer_char->hide();
        ui_vp_sideplayer_char->move(0, 0);
      }
    }
  }
  // Set ourselves according to SELF_OFFSET

  QStringList self_offsets = m_chatmessage[SELF_OFFSET].split("&");
  int self_offset = self_offsets[0].toInt();
  int self_offset_v;
  if (self_offsets.length() <= 1)
    self_offset_v = 0;
  else 
    self_offset_v = self_offsets[1].toInt();
  ui_vp_player_char->move(ui_viewport->width() * self_offset / 100, ui_viewport->height() * self_offset_v / 100);

  switch (emote_mod) {
  case 1:
  case 2:
  case 6:
    play_preanim(false);
    break;
  case 0:
  case 5:
    if (m_chatmessage[NONINTERRUPTING_PRE].toInt() == 0)
      handle_chatmessage_3();
    else
      play_preanim(true);
    break;
  default:
    qDebug() << "W: invalid emote mod: " << QString::number(emote_mod);
  }
}

void Courtroom::do_screenshake()
{
  if (!ao_app->is_shake_enabled())
    return;

  // This way, the animation is reset in such a way that last played screenshake
  // would return to its "final frame" properly. This properly resets all UI
  // elements without having to bother keeping track of "origin" positions.
  // Works great wit the chat text being detached from the chat box!
  screenshake_animation_group->setCurrentTime(
      screenshake_animation_group->duration());
  screenshake_animation_group->clear();

  QList<QWidget *> affected_list = {ui_vp_background, ui_vp_player_char,
                                    ui_vp_sideplayer_char, ui_vp_chatbox};

  // I would prefer if this was its own "shake" function to be honest.
  foreach (QWidget *ui_element, affected_list) {
    QPropertyAnimation *screenshake_animation =
        new QPropertyAnimation(ui_element, "pos", this);
    QPoint pos_default = QPoint(ui_element->x(), ui_element->y());

    int duration = 300; // How long does the screenshake last
    int frequency = 20; // How often in ms is there a "jolt" frame
    int maxframes = duration / frequency;
    int max_x = 7; // Max deviation from origin on x axis
    int max_y = 7; // Max deviation from origin on y axis
    screenshake_animation->setDuration(duration);
    for (int frame = 0; frame < maxframes; frame++) {
      double fraction = double(frame * frequency) / duration;
      int rng = qrand(); // QRandomGenerator::global()->generate();
      int rand_x = max_x - (int(rng) % (max_x * 2));
      int rand_y = max_y - (int(rng + 100) % (max_y * 2));
      screenshake_animation->setKeyValueAt(
          fraction, QPoint(pos_default.x() + rand_x, pos_default.y() + rand_y));
    }
    screenshake_animation->setEndValue(pos_default);
    screenshake_animation->setEasingCurve(QEasingCurve::Linear);
    screenshake_animation_group->addAnimation(screenshake_animation);
  }

  screenshake_animation_group->start();
}

void Courtroom::do_flash()
{
  if (!ao_app->is_effects_enabled())
    return;

  QString f_char = m_chatmessage[CHAR_NAME];
  QString f_custom_theme = ao_app->get_char_shouts(f_char);
  ui_vp_effect->play("realizationflash", f_char, f_custom_theme, 60);
}

void Courtroom::do_effect(QString fx_name, QString fx_sound, QString p_char,
                          QString p_folder)
{

  QString effect = ao_app->get_effect(fx_name, p_char, p_folder);
  if (effect == "")
    return;

  if (fx_sound != "")
    sfx_player->play(fx_sound, "","",0);

  // Only check if effects are disabled after playing the sound if it exists
  if (!ao_app->is_effects_enabled())
    return;

  ui_vp_effect->set_play_once(
      false); // The effects themselves dictate whether or not they're looping.
              // Static effects will linger.
  ui_vp_effect->play(effect); // It will set_play_once to true if the filepath
                              // provided is not designed to loop more than once
}

void Courtroom::play_char_sfx(QString sfx_name)
{
  sfx_player->play(sfx_name, "","",0);
  //  sfx_player->set_looping(false);
  //  if (ao_app->get_looping_sfx())
  //    sfx_player->set_looping(
  //        ao_app->get_sfx_looping(current_char, current_emote) == "1");
}

void Courtroom::handle_chatmessage_3()
{
  int f_evi_id = m_chatmessage[EVIDENCE_ID].toInt();
  QString f_side = m_chatmessage[SIDE];

  QString f_showname;
  int f_char_id = m_chatmessage[CHAR_ID].toInt();
  if (f_char_id > -1 &&
      (m_chatmessage[SHOWNAME].isEmpty() || !ui_showname_enable->isChecked())) {
    f_showname = ao_app->get_showname(char_list.at(f_char_id).name);
  }
  else {
    f_showname = m_chatmessage[SHOWNAME];
  }
  if (f_showname.trimmed()
          .isEmpty()) // Pure whitespace showname, get outta here.
    f_showname = m_chatmessage[CHAR_NAME];

  if (f_evi_id > 0 && f_evi_id <= local_evidence_list.size() &&
      !evidence_presented) {
    // shifted by 1 because 0 is no evidence per legacy standards
    QString f_image = local_evidence_list.at(f_evi_id - 1).image;
    QString f_evi_name = local_evidence_list.at(f_evi_id - 1).name;
    // def jud and hlp should display the evidence icon on the RIGHT side
    bool is_left_side = !(f_side == "def" || f_side == "hlp" ||
                          f_side == "jud" || f_side == "jur");
    ui_vp_evidence_display->show_evidence(f_image, is_left_side,
                                          ui_sfx_slider->value());

    log_ic_text(m_chatmessage[CHAR_NAME], m_chatmessage[SHOWNAME], f_evi_name,
                tr("has presented evidence"),
                m_chatmessage[TEXT_COLOR].toInt());
    append_ic_text(f_evi_name, f_showname, tr("has presented evidence"));
    evidence_presented = true; // we're done presenting evidence, and we
                               // don't want to do it twice
  }

  int emote_mod = m_chatmessage[EMOTE_MOD].toInt();

  QString side = m_chatmessage[SIDE];

  if (emote_mod == 5 || emote_mod == 6) {
    ui_vp_desk->hide();
    ui_vp_legacy_desk->hide();

    // Since we're zooming, hide the second character, and centre the first.
    ui_vp_sideplayer_char->hide();
    ui_vp_player_char->move(0, 0);

    QString f_char = m_chatmessage[CHAR_NAME];
    QString f_custom_theme = ao_app->get_char_shouts(f_char);
    if (side == "pro" || side == "hlp" || side == "wit")
      ui_vp_speedlines->play("prosecution_speedlines", f_char, f_custom_theme);
    else
      ui_vp_speedlines->play("defense_speedlines", f_char, f_custom_theme);
  }

  // If this color is talking
  color_is_talking =
      color_markdown_talking_list.at(m_chatmessage[TEXT_COLOR].toInt());

  if (color_is_talking && text_state == 1 &&
      anim_state < 2) // Set it to talking as we're not on that already
  {
    ui_vp_player_char->stop();
    ui_vp_player_char->play_talking(m_chatmessage[CHAR_NAME],
                                    m_chatmessage[EMOTE]);
    anim_state = 2;
  }
  else if (anim_state < 3) // Set it to idle as we're not on that already
  {
    ui_vp_player_char->stop();
    ui_vp_player_char->play_idle(m_chatmessage[CHAR_NAME],
                                 m_chatmessage[EMOTE]);
    anim_state = 3;
  }

  QString f_message = m_chatmessage[MESSAGE];
  QStringList call_words = ao_app->get_call_words();

  for (QString word : call_words) {
    if (f_message.contains(word, Qt::CaseInsensitive)) {
      modcall_player->play(ao_app->get_sfx("word_call"));
      ao_app->alert(this);

      break;
    }
  }

  start_chat_ticking();
}

QString Courtroom::filter_ic_text(QString p_text, bool html, int target_pos,
                                  int default_color)
{
  QString p_text_escaped;

  int check_pos = 0;
  int check_pos_escaped = 0;
  bool parse_escape_seq = false;
  std::stack<int> ic_color_stack;

  // Text alignment shenanigans. Could make a dropdown for this later, too!
  QString align;
  if (p_text.trimmed().startsWith("~~")) {
    p_text.remove(p_text.indexOf("~~"), 2);
    if (target_pos != -1) {
      target_pos = qMax(0, target_pos - 2);
    }
    align = "center";
  }
  else if (p_text.trimmed().startsWith("~>")) {
    p_text.remove(p_text.indexOf("~>"), 2);
    if (target_pos != -1) {
      target_pos = qMax(0, target_pos - 2);
    }
    align = "right";
  }
  else if (p_text.trimmed().startsWith("<>")) {
    p_text.remove(p_text.indexOf("<>"), 2);
    if (target_pos != -1) {
      target_pos = qMax(0, target_pos - 2);
    }
    align = "justify";
  }

  // If html is enabled, prepare this text to be all ready for it.
  if (html) {
    ic_color_stack.push(default_color);
    QString appendage = "<font color=\"" +
                        color_rgb_list.at(default_color).name(QColor::HexRgb) +
                        "\">";

    if (!align.isEmpty())
      appendage.prepend("<div align=" + align + ">");

    p_text_escaped.insert(check_pos_escaped, appendage);
    check_pos_escaped += appendage.size();
  }

  // Current issue: does not properly escape html stuff.
  // Solution: probably parse p_text and export into a different string
  // separately, perform some mumbo jumbo to properly adjust string indexes.
  while (check_pos < p_text.size()) {
    QString f_rest = p_text.right(p_text.size() - check_pos);
    QTextBoundaryFinder tbf(QTextBoundaryFinder::Grapheme, f_rest);
    QString f_character;
    int f_char_length;
    int f_char_bytes;

    tbf.toNextBoundary();

    if (tbf.position() == -1)
      f_character = f_rest;
    else
      f_character = f_rest.left(tbf.position());

    //    if (f_character == "&") //oh shit it's probably an escaped html
    //    {
    //      //Skip escaped chars like you would graphemes
    //      QRegularExpression re("&([a-z0-9]+|#[0-9]{1,6}|#x[0-9a-f]{1,6});",
    //      QRegularExpression::CaseInsensitiveOption); QRegularExpressionMatch
    //      match = re.match(f_rest); if (match.hasMatch()) //OH SHIT IT IS,
    //      PANIC, PANIC
    //      {
    //        f_character = match.captured(0); //Phew, we solved the big problem
    //        here.
    //      }
    //    }

    f_char_bytes = f_char_length = f_character.length();

    if (html) {
      f_character = f_character.toHtmlEscaped();
      f_char_length = f_character.length();
    }

    bool color_update = false;
    bool is_end = false;
    bool skip = false;

    if (!parse_escape_seq) {
      if (f_character == "\\") {
        parse_escape_seq = true;
        skip = true;
      }
      // Nothing related to colors here
      else if (f_character == "{" ||
               f_character ==
                   "}") //|| f_character == "@" || f_character == "$")
      {
        skip = true;
      }
      // Parse markdown colors
      else {
        for (int c = 0; c < max_colors; ++c) {
          // Clear the stored optimization information
          QString markdown_start = color_markdown_start_list.at(c);
          QString markdown_end = color_markdown_end_list.at(c);
          if (html) {
            markdown_start = markdown_start.toHtmlEscaped();
            markdown_end = markdown_end.toHtmlEscaped();
          }
          bool markdown_remove = color_markdown_remove_list.at(c);
          if (markdown_start.isEmpty()) // Not defined
            continue;

          if (markdown_end.isEmpty() ||
              markdown_end == markdown_start) //"toggle switch" type
          {
            if (f_character == markdown_start) {
              if (html) {
                if (!ic_color_stack.empty() && ic_color_stack.top() == c &&
                    default_color != c) {
                  ic_color_stack.pop(); // Cease our coloring
                  is_end = true;
                }
                else {
                  ic_color_stack.push(c); // Begin our coloring
                }
                color_update = true;
              }
              skip = markdown_remove;
              break; // Prevent it from looping forward for whatever reason
            }
          }
          else if (f_character == markdown_start ||
                   (f_character == markdown_end && !ic_color_stack.empty() &&
                    ic_color_stack.top() == c)) {
            if (html) {
              if (f_character == markdown_end) {
                ic_color_stack.pop(); // Cease our coloring
                is_end = true;
              }
              else if (f_character == markdown_start) {
                ic_color_stack.push(c); // Begin our coloring
              }
              color_update = true;
            }
            skip = markdown_remove;
            break; // Prevent it from looping forward for whatever reason
          }
        }
        // Parse the newest color stack
        if (color_update && (target_pos <= -1 || check_pos < target_pos)) {
          if (!parse_escape_seq) {
            QString appendage = "</font>";

            if (!ic_color_stack.empty())
              appendage +=
                  "<font color=\"" +
                  color_rgb_list.at(ic_color_stack.top()).name(QColor::HexRgb) +
                  "\">";

            if (is_end && !skip) {
              p_text_escaped.insert(check_pos_escaped,
                                    f_character); // Add that char right now
              check_pos_escaped +=
                  f_char_length; // So the closing char is captured too
              skip = true;
            }
            p_text_escaped.insert(check_pos_escaped, appendage);
            check_pos_escaped += appendage.size();
          }
        }
      }
    }
    else {
      if (f_character == "n") // \n, that's a line break son
      {
        QString appendage = "<br/>";
        if (!html) {
          // actual newline commented out
          //          appendage = "\n";
          //          size = 1; //yeah guess what \n is a "single character"
          //          apparently
          appendage = "\\n "; // visual representation of a newline
        }
        p_text_escaped.insert(check_pos_escaped, appendage);
        check_pos_escaped += appendage.size();
        skip = true;
      }
      if (f_character == "s" || f_character == "f") // screenshake/flash
        skip = true;

      parse_escape_seq = false;
    }

    // Make all chars we're not supposed to see invisible
    if (target_pos > -1 && check_pos == target_pos) {
      QString appendage = "";
      if (!ic_color_stack.empty()) {
        if (!is_end) // Was our last coloring char ending the color stack or nah
        {
          // God forgive me for my transgressions but I have refactored this
          // whole thing about 25 times and having to refactor it again to more
          // elegantly support this will finally make me go insane.
          color_is_talking =
              color_markdown_talking_list.at(ic_color_stack.top());
        }

        // Clean it up, we're done here
        while (!ic_color_stack.empty())
          ic_color_stack.pop();

        appendage += "</font>";
      }
      ic_color_stack.push(
          -1); // Dummy colorstack push for maximum </font> appendage
      appendage += "<font color=\"#00000000\">";
      p_text_escaped.insert(check_pos_escaped, appendage);
      check_pos_escaped += appendage.size();
    }
    if (!skip) {
      p_text_escaped.insert(check_pos_escaped, f_character);
      check_pos_escaped += f_char_length;
    }
    check_pos += f_char_bytes;
  }

  if (!ic_color_stack.empty() && html) {
    p_text_escaped.append("</font>");
  }

  if (html) {
    // Example: https://regex101.com/r/oL4nM9/37 - this replaces
    // excessive/trailing/etc. whitespace with non-breaking space. I WOULD use
    // white-space: pre; stylesheet tag, but for whataver reason it doesn't work
    // no matter where I try it. If somoene else can get that piece of HTML
    // memery to work, please do.
    p_text_escaped.replace(QRegularExpression("^\\s|(?<=\\s)\\s"), "&nbsp;");
    if (!align.isEmpty())
      p_text_escaped.append("</div>");
  }

  return p_text_escaped;
}

void Courtroom::log_ic_text(QString p_name, QString p_showname,
                            QString p_message, QString p_action, int p_color)
{
  chatlogpiece log_entry(p_name, p_showname, p_message, p_action, p_color);
  ic_chatlog_history.append(log_entry);
  if (ao_app->get_auto_logging_enabled())
    ao_app->append_to_file(log_entry.get_full(), ao_app->log_filename, true);

  while (ic_chatlog_history.size() > log_maximum_blocks &&
         log_maximum_blocks > 0) {
    ic_chatlog_history.removeFirst();
  }
}

void Courtroom::append_ic_text(QString p_text, QString p_name, QString p_action,
                               int color)
{
  QTextCharFormat bold;
  QTextCharFormat normal;
  QTextCharFormat italics;
  QTextBlockFormat format;
  bold.setFontWeight(QFont::Bold);
  normal.setFontWeight(QFont::Normal);
  italics.setFontItalic(true);
  format.setTopMargin(log_margin);
  const QTextCursor old_cursor = ui_ic_chatlog->textCursor();
  const int old_scrollbar_value = ui_ic_chatlog->verticalScrollBar()->value();
  const bool need_newline = !ui_ic_chatlog->document()->isEmpty();
  const int scrollbar_target_value =
      log_goes_downwards ? ui_ic_chatlog->verticalScrollBar()->maximum()
                         : ui_ic_chatlog->verticalScrollBar()->minimum();

  ui_ic_chatlog->moveCursor(log_goes_downwards ? QTextCursor::End
                                               : QTextCursor::Start);

  // Only prepend with newline if log goes downwards
  if (log_goes_downwards && need_newline) {
    ui_ic_chatlog->textCursor().insertBlock(format);
  }

  // Timestamp if we're doing that meme
  if (log_timestamp)
    ui_ic_chatlog->textCursor().insertText(
        "[" + QDateTime::currentDateTime().toString("h:mm:ss AP") + "] ",
        normal);

  // Format the name of the actor
  ui_ic_chatlog->textCursor().insertText(p_name, bold);
  // Special case for stopping the music
  if (p_action == tr("has stopped the music")) {
    ui_ic_chatlog->textCursor().insertText(" " + p_action + ".", normal);
  }
  // If action not blank:
  else if (p_action != "") {
    // Format the action in normal
    ui_ic_chatlog->textCursor().insertText(" " + p_action, normal);
    if (log_newline)
      // For some reason, we're forced to use <br> instead of the more sensible
      // \n. Why? Because \n is treated as a new Block instead of a soft newline
      // within a paragraph!
      ui_ic_chatlog->textCursor().insertHtml("<br>");
    else
      ui_ic_chatlog->textCursor().insertText(": ", normal);
    // Format the result in italics
    ui_ic_chatlog->textCursor().insertText(p_text + ".", italics);
  }
  else {
    if (log_newline)
      // For some reason, we're forced to use <br> instead of the more sensible
      // \n. Why? Because \n is treated as a new Block instead of a soft newline
      // within a paragraph!
      ui_ic_chatlog->textCursor().insertHtml("<br>");
    else
      ui_ic_chatlog->textCursor().insertText(": ", normal);
    // Format the result according to html
    if (log_colors)
      ui_ic_chatlog->textCursor().insertHtml(
          filter_ic_text(p_text, true, -1, color));
    else
      ui_ic_chatlog->textCursor().insertText(filter_ic_text(p_text, false),
                                             normal);
  }

  // Only append with newline if log goes upwards
  if (!log_goes_downwards && need_newline) {
    ui_ic_chatlog->textCursor().insertBlock(format);
  }

  // If we got too many blocks in the current log, delete some.
  while (ui_ic_chatlog->document()->blockCount() > log_maximum_blocks &&
         log_maximum_blocks > 0) {
    ui_ic_chatlog->moveCursor(log_goes_downwards ? QTextCursor::Start
                                                 : QTextCursor::End);
    ui_ic_chatlog->textCursor().select(QTextCursor::BlockUnderCursor);
    ui_ic_chatlog->textCursor().removeSelectedText();
    if (log_goes_downwards)
      ui_ic_chatlog->textCursor().deleteChar();
    else
      ui_ic_chatlog->textCursor().deletePreviousChar();
  }

  // Finally, scroll the scrollbar to the correct position.
  if (old_cursor.hasSelection() ||
      old_scrollbar_value != scrollbar_target_value) {
    // The user has selected text or scrolled away from the bottom: maintain
    // position.
    ui_ic_chatlog->setTextCursor(old_cursor);
    ui_ic_chatlog->verticalScrollBar()->setValue(old_scrollbar_value);
  }
  else {
    // The user hasn't selected any text and the scrollbar is at the bottom:
    // scroll to the bottom.
    ui_ic_chatlog->moveCursor(log_goes_downwards ? QTextCursor::End
                                                 : QTextCursor::Start);
    ui_ic_chatlog->verticalScrollBar()->setValue(
        log_goes_downwards ? ui_ic_chatlog->verticalScrollBar()->maximum() : 0);
  }
}

void Courtroom::play_preanim(bool noninterrupting)
{
  QString f_char = m_chatmessage[CHAR_NAME];
  QString f_preanim = m_chatmessage[PRE_EMOTE];

  // all time values in char.inis are multiplied by a constant(time_mod) to get
  // the actual time
  int ao2_duration = ao_app->get_ao2_preanim_duration(f_char, f_preanim);
  int text_delay = ao_app->get_text_delay(f_char, f_preanim) * time_mod;
  int sfx_delay = m_chatmessage[SFX_DELAY].toInt() * time_mod;

  int preanim_duration;

  if (ao2_duration < 0)
    preanim_duration = ao_app->get_preanim_duration(f_char, f_preanim);
  else
    preanim_duration = ao2_duration;

  sfx_delay_timer->start(sfx_delay);
  QString anim_to_find =
      ao_app->get_image_suffix(ao_app->get_character_path(f_char, f_preanim));
  if (!file_exists(anim_to_find)) {
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

void Courtroom::start_chat_ticking()
{
  // we need to ensure that the text isn't already ticking because this function
  // can be called by two logic paths
  if (text_state != 0)
    return;

  if (m_chatmessage[EFFECTS] != "") {
    QStringList fx_list = m_chatmessage[EFFECTS].split("|");
    QString fx = fx_list[0];
    QString fx_sound;
    QString fx_folder;

    if (fx_list.length() > 1)
      fx_sound = fx_list[1];

    if (fx_list.length() > 2) {
      fx_folder = fx_list[1];
      fx_sound = fx_list[2];
    }

    this->do_effect(fx, fx_sound, m_chatmessage[CHAR_NAME], fx_folder);
  }
  else if (m_chatmessage[REALIZATION] == "1") {
    this->do_flash();
    sfx_player->play(ao_app->get_custom_realization(m_chatmessage[CHAR_NAME]), "","",0);
  }
  int emote_mod = m_chatmessage[EMOTE_MOD].toInt(); // text meme bonanza
  if ((emote_mod == 0 || emote_mod == 5) && m_chatmessage[SCREENSHAKE] == "1") {
    this->do_screenshake();
  }
  if (m_chatmessage[MESSAGE].isEmpty()) {
    // since the message is empty, it's technically done ticking
    text_state = 2;
    if (m_chatmessage[ADDITIVE] == "1") {
      // Cool behavior
      ui_vp_chatbox->show();
      ui_vp_message->show();
    }
    return;
  }

  ui_vp_chatbox->show();
  ui_vp_message->show();

  if (m_chatmessage[ADDITIVE] != "1") {
    ui_vp_message->clear();
    real_tick_pos = 0;
    additive_previous = "";
  }

  tick_pos = 0;
  blip_ticker = 0;

  // At the start of every new message, we set the text speed to the default.
  current_display_speed = 3;
  chat_tick_timer->start(0); // Display the first char right away

  QString f_gender = ao_app->get_gender(m_chatmessage[CHAR_NAME]);

  blip_player->set_blips(f_gender);

  // means text is currently ticking
  text_state = 1;
}

void Courtroom::chat_tick()
{
  // note: this is called fairly often
  // do not perform heavy operations here

  QString f_message = m_chatmessage[MESSAGE];

  // Due to our new text speed system, we always need to stop the timer now.
  chat_tick_timer->stop();

  if (tick_pos >= f_message.size()) {
    text_state = 2;
    if (anim_state < 3) {
      anim_state = 3;
      ui_vp_player_char->play_idle(m_chatmessage[CHAR_NAME],
                                   m_chatmessage[EMOTE]);
    }
    QString f_char;
    QString f_custom_theme;
    if (ao_app->is_customchat_enabled()) {
      f_char = m_chatmessage[CHAR_NAME];
      f_custom_theme = ao_app->get_chat(f_char);
    }
    ui_vp_chat_arrow->play(
        "chat_arrow", f_char,
        f_custom_theme); // Chat stopped being processed, indicate that.
    additive_previous =
        additive_previous +
        filter_ic_text(f_message, true, -1, m_chatmessage[TEXT_COLOR].toInt());
    real_tick_pos = ui_vp_message->toPlainText().size();
    return;
  }

  // Stops blips from playing when we have a formatting option.
  bool formatting_char = false;

  QString f_rest = f_message;

  // Alignment characters
  if (tick_pos < 2) {
    if (f_rest.startsWith("~~")) {
      tick_pos = f_rest.indexOf("~~");
      f_rest.remove(tick_pos, 2);
      tick_pos += 2;
    }
    else if (f_rest.startsWith("~>")) {
      tick_pos = f_rest.indexOf("~>");
      f_rest.remove(tick_pos, 2);
      tick_pos += 2;
    }
    else if (f_rest.startsWith("<>")) {
      tick_pos = f_rest.indexOf("<>");
      f_rest.remove(tick_pos, 2);
      tick_pos += 2;
    }
  }
  f_rest.remove(0, tick_pos);
  QTextBoundaryFinder tbf(QTextBoundaryFinder::Grapheme, f_rest);
  QString f_character;
  int f_char_length;

  tbf.toNextBoundary();

  if (tbf.position() == -1)
    f_character = f_rest;
  else
    f_character = f_rest.left(tbf.position());

  f_char_length = f_character.length();
  tick_pos += f_char_length;

  // Escape character.
  if (!next_character_is_not_special) {
    if (f_character == "\\") {
      next_character_is_not_special = true;
      formatting_char = true;
    }

    // Text speed modifier.
    else if (f_character == "{") {
      // ++, because it INCREASES delay!
      current_display_speed++;
      formatting_char = true;
    }
    else if (f_character == "}") {
      current_display_speed--;
      formatting_char = true;
    }

    else {
      // Parse markdown colors
      for (int c = 0; c < max_colors; ++c) {
        QString markdown_start = color_markdown_start_list.at(c);
        QString markdown_end = color_markdown_end_list.at(c);
        bool markdown_remove = color_markdown_remove_list.at(c);
        if (markdown_start.isEmpty())
          continue;

        if (f_character == markdown_start || f_character == markdown_end) {
          if (markdown_remove)
            formatting_char = true;
          break;
        }
      }
    }
  }
  else {
    if (f_character == "n")
      formatting_char = true; // it's a newline
    if (f_character == "s")   // Screenshake.
    {
      this->do_screenshake();
      formatting_char = true;
    }
    if (f_character == "f") // Flash.
    {
      this->do_flash();
      formatting_char = true;
    }
    next_character_is_not_special = false;
  }

  // Keep the speed at bay.
  if (current_display_speed < 0)
    current_display_speed = 0;
  else if (current_display_speed > 6)
    current_display_speed = 6;

  if ((message_display_speed[current_display_speed] <= 0 &&
       tick_pos < f_message.size() - 1) ||
      formatting_char) {
    chat_tick_timer->start(0); // Don't bother rendering anything out as we're
                               // doing the SPEED. (there's latency otherwise)
    if (!formatting_char || f_character == "n" || f_character == "f" ||
        f_character == "s")
      real_tick_pos += f_char_length; // Adjust the tick position for the
                                      // scrollbar convenience
  }
  else {
    int msg_delay = message_display_speed[current_display_speed];
    // Do the colors, gradual showing, etc. in here
    ui_vp_message->setHtml(additive_previous +
                           filter_ic_text(f_message, true, tick_pos,
                                          m_chatmessage[TEXT_COLOR].toInt()));

    // This should always be done AFTER setHtml. Scroll the chat window with the
    // text.

    // Make the cursor follow the message
    QTextCursor cursor = ui_vp_message->textCursor();
    cursor.setPosition(real_tick_pos);
    ui_vp_message->setTextCursor(cursor);
    real_tick_pos += f_char_length;

    ui_vp_message->ensureCursorVisible();

    // We blip every "blip rate" letters.
    // Here's an example with blank_blip being false and blip_rate being 2:
    // I am you
    // ! !  ! !
    // where ! is the blip sound
    int b_rate = blip_rate;
    // Earrape prevention without using timers, this method is more consistent.
    if (msg_delay != 0 && msg_delay <= 25) {
      // The default blip speed is 40ms, and if current msg_delay is 25ms,
      // the formula will result in the blip rate of:
      // 40/25 = 1.6 = 2
      // And if it's faster than that:
      // 40/10 = 4
      b_rate = qMax(b_rate, qRound(static_cast<float>(message_display_speed[3])/msg_delay));
    }
    if (blip_ticker % b_rate == 0) {
      // ignoring white space unless blank_blip is enabled.
      if (!formatting_char && (f_character != ' ' || blank_blip)) {
        blip_player->blip_tick();
        ++blip_ticker;
      }
    }
    else
    {
      // Don't fully ignore whitespace still, keep ticking until
      // we reached the need to play a blip sound - we also just
      // need to wait for a letter to play it on.
      ++blip_ticker;
    }

    // Punctuation delayer, only kicks in on speed ticks less than }}
    if (current_display_speed > 1 && punctuation_chars.contains(f_character)) {
      // Making the user have to wait any longer than 150ms per letter is downright unreasonable
      msg_delay = qMin(150, msg_delay * punctuation_modifier);
    }

    // If this color is talking
    if (color_is_talking && anim_state != 2 &&
        anim_state <
            4) // Set it to talking as we're not on that already (though we have
               // to avoid interrupting a non-interrupted preanim)
    {
      ui_vp_player_char->stop();
      ui_vp_player_char->play_talking(m_chatmessage[CHAR_NAME],
                                      m_chatmessage[EMOTE]);
      anim_state = 2;
    }
    else if (!color_is_talking && anim_state < 3 &&
             anim_state != 3) // Set it to idle as we're not on that already
    {
      ui_vp_player_char->stop();
      ui_vp_player_char->play_idle(m_chatmessage[CHAR_NAME],
                                   m_chatmessage[EMOTE]);
      anim_state = 3;
    }
    // Continue ticking
    chat_tick_timer->start(msg_delay);
  }
}

void Courtroom::play_sfx()
{
  QString sfx_name = m_chatmessage[SFX_NAME];
  if (m_chatmessage[SCREENSHAKE] ==
      "1") // Screenshake dependant on preanim sfx delay meme
  {
    this->do_screenshake();
  }
  if (sfx_name == "1")
    return;

  sfx_player->play(sfx_name, "","",0);
  if (ao_app->get_looping_sfx())
    sfx_player->set_looping(
        ao_app->get_sfx_looping(current_char, current_emote) == "1");
}

void Courtroom::set_scene(QString f_desk_mod, QString f_side)
{
  // witness is default if pos is invalid
  QString f_background = "witnessempty";
  QString f_desk_image = "stand";

  if (f_side == "def" && file_exists(ao_app->get_image_suffix(
                             ao_app->get_background_path("defenseempty")))) {
    f_background = "defenseempty";
    f_desk_image = "defensedesk";
  }
  else if (f_side == "pro" &&
           file_exists(ao_app->get_image_suffix(
               ao_app->get_background_path("prosecutorempty")))) {
    f_background = "prosecutorempty";
    f_desk_image = "prosecutiondesk";
  }
  else if (f_side == "jud" && file_exists(ao_app->get_image_suffix(
                                  ao_app->get_background_path("judgestand")))) {
    f_background = "judgestand";
    f_desk_image = "judgedesk";
  }
  else if (f_side == "hld" &&
           file_exists(ao_app->get_image_suffix(
               ao_app->get_background_path("helperstand")))) {
    f_background = "helperstand";
    f_desk_image = "helperdesk";
  }
  else if (f_side == "hlp" &&
           file_exists(ao_app->get_image_suffix(
               ao_app->get_background_path("prohelperstand")))) {
    f_background = "prohelperstand";
    f_desk_image = "prohelperdesk";
  }
  else if (f_side == "jur" && file_exists(ao_app->get_image_suffix(
                                  ao_app->get_background_path("jurystand")))) {
    f_background = "jurystand";
    f_desk_image = "jurydesk";
  }
  else if (f_side == "sea" &&
           file_exists(ao_app->get_image_suffix(
               ao_app->get_background_path("seancestand")))) {
    f_background = "seancestand";
    f_desk_image = "seancedesk";
  }

  if (file_exists(ao_app->get_image_suffix(
          ao_app->get_background_path(f_side)))) // Unique pos path
  {
    f_background = f_side;
    f_desk_image = f_side + "_overlay";
  }

  ui_vp_background->set_image(f_background);
  ui_vp_desk->set_image(f_desk_image);
  ui_vp_legacy_desk->set_legacy_desk(f_desk_image);

  if (f_desk_mod == "0" ||
      (f_desk_mod != "1" &&
       (f_side == "jud" || f_side == "hld" || f_side == "hlp"))) {
    ui_vp_desk->hide();
    ui_vp_legacy_desk->hide();
  }
  else {
    ui_vp_legacy_desk->hide();
    ui_vp_desk->show();
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
  else {
    ui_muted->hide();
    ui_ic_chat_message->setFocus();
  }

  ui_muted->resize(ui_ic_chat_message->width(), ui_ic_chat_message->height());
  ui_muted->set_image("muted");

  is_muted = p_muted;
  ui_ic_chat_message->setEnabled(!p_muted);
}

void Courtroom::set_ban(int p_cid)
{
  if (p_cid != m_cid && p_cid != -1)
    return;

  call_notice(tr("You have been banned."));

  ao_app->construct_lobby();
  ao_app->destruct_courtroom();
}

void Courtroom::handle_song(QStringList *p_contents)
{
  QStringList f_contents = *p_contents;

  if (f_contents.size() < 2)
    return;

  QString f_song = f_contents.at(0);
  QString f_song_clear = f_song.left(f_song.lastIndexOf("."));
  f_song_clear = f_song_clear.right(f_song_clear.length() -
                                    (f_song_clear.lastIndexOf("/") + 1));
  int n_char = f_contents.at(1).toInt();

  // Assume the song doesn't loop unless told otherwise (due to most outdated
  // servers handling looping through serverside)
  bool looping = false;
  // Channel 0 is the 'master music', other channels would commonly be used for
  // ambience
  int channel = 0;
  // No effects assumed by default - vanilla functionality
  int effect_flags = 0;

  if (n_char < 0 || n_char >= char_list.size()) {
    int channel = 0;
    if (p_contents->length() > 3 && p_contents->at(3) == "1")
      looping = true;

    if (p_contents->length() >
        4) // eyyy we want to change this song's CHANNEL huh
      channel = p_contents->at(4).toInt(); // let the music player handle it if
                                           // it's bigger than the channel list

    if (p_contents->length() > 5) // Flags provided to us by server such as Fade
                                  // In, Fade Out, Sync Pos etc.
    {
      effect_flags = p_contents->at(5).toInt();
    }
    music_player->play(f_song, channel, looping, effect_flags);
    if (f_song == "~stop.mp3")
      ui_music_name->setText(tr("None"));
    else if (channel == 0) {
      if (file_exists(ao_app->get_sfx_suffix(ao_app->get_music_path(f_song))))
        ui_music_name->setText(f_song_clear);
      else
        ui_music_name->setText(tr("[MISSING] %1").arg(f_song_clear));
    }
  }
  else {
    QString str_char = char_list.at(n_char).name;
    QString str_show = ao_app->get_showname(str_char);

    if (p_contents->length() > 2) {
      if (p_contents->at(2) != "") {
        str_show = p_contents->at(2);
      }
    }
    if (p_contents->length() > 3 && p_contents->at(3) == "1") {
      looping = true;
    }
    if (p_contents->length() >
        4) // eyyy we want to change this song's CHANNEL huh
      channel = p_contents->at(4).toInt(); // let the music player handle it if
                                           // it's bigger than the channel list

    if (p_contents->length() > 5) // Flags provided to us by server such as Fade
                                  // In, Fade Out, Sync Pos etc.
    {
      effect_flags = p_contents->at(5).toInt();
    }

    if (!mute_map.value(n_char)) {
      if (f_song == "~stop.mp3") {
        log_ic_text(str_char, str_show, "", tr("has stopped the music"),
                    m_chatmessage[TEXT_COLOR].toInt());
        append_ic_text("", str_show, tr("has stopped the music"));
      }
      else {
        log_ic_text(str_char, str_show, f_song, tr("has played a song"),
                    m_chatmessage[TEXT_COLOR].toInt());
        append_ic_text(f_song_clear, str_show, tr("has played a song"));
      }
      music_player->play(f_song, channel, looping, effect_flags);
      if (f_song == "~stop.mp3")
        ui_music_name->setText(tr("None"));
      else if (channel == 0) {
        if (file_exists(ao_app->get_sfx_suffix(ao_app->get_music_path(f_song))))
          ui_music_name->setText(f_song_clear);
        else
          ui_music_name->setText(tr("[MISSING] %1").arg(f_song_clear));
      }
    }
  }
}

void Courtroom::handle_wtce(QString p_wtce, int variant)
{
  QString sfx_file = "courtroom_sounds.ini";

  // witness testimony
  if (p_wtce == "testimony1") {
    sfx_player->play(ao_app->get_sfx("witness_testimony"));
    ui_vp_wtce->play("witnesstestimony", "", "", 1500);
    ui_vp_testimony->play("testimony");
  }
  // cross examination
  else if (p_wtce == "testimony2") {
    sfx_player->play(ao_app->get_sfx("cross_examination"));
    ui_vp_wtce->play("crossexamination", "", "", 1500);
    ui_vp_testimony->stop();
  }
  else if (p_wtce == "judgeruling") {
    if (variant == 0) {
      sfx_player->play(ao_app->get_sfx("not_guilty"));
      ui_vp_wtce->play("notguilty", "", "", 3000);
      ui_vp_testimony->stop();
    }
    else if (variant == 1) {
      sfx_player->play(ao_app->get_sfx("guilty"));
      ui_vp_wtce->play("guilty", "", "", 3000);
      ui_vp_testimony->stop();
    }
  }
}

void Courtroom::set_hp_bar(int p_bar, int p_state)
{
  if (p_state < 0 || p_state > 10)
    return;

  if (p_bar == 1) {
    ui_defense_bar->set_image("defensebar" + QString::number(p_state));
    defense_bar_state = p_state;
  }
  else if (p_bar == 2) {
    ui_prosecution_bar->set_image("prosecutionbar" + QString::number(p_state));
    prosecution_bar_state = p_state;
  }
}

void Courtroom::toggle_judge_buttons(bool is_on)
{
  if (is_on) {
    ui_witness_testimony->show();
    ui_cross_examination->show();
    ui_guilty->show();
    ui_not_guilty->show();
    ui_defense_minus->show();
    ui_defense_plus->show();
    ui_prosecution_minus->show();
    ui_prosecution_plus->show();
  }
  else {
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
  if (!ui_guard->isChecked()) {
    modcall_player->play(ao_app->get_sfx("mod_call"));
    ao_app->alert(this);
  }
}

void Courtroom::case_called(QString msg, bool def, bool pro, bool jud, bool jur,
                            bool steno)
{
  if (ui_casing->isChecked()) {
    ui_server_chatlog->append(msg);
    if ((ao_app->get_casing_defence_enabled() && def) ||
        (ao_app->get_casing_prosecution_enabled() && pro) ||
        (ao_app->get_casing_judge_enabled() && jud) ||
        (ao_app->get_casing_juror_enabled() && jur) ||
        (ao_app->get_casing_steno_enabled() && steno)) {
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

  if (ooc_message.startsWith("/pos")) {
    if (ooc_message == "/pos jud") {
      toggle_judge_buttons(true);
    }
    else {
      toggle_judge_buttons(false);
    }
  }
  else if (ooc_message.startsWith("/settings")) {
    ui_ooc_chat_message->clear();
    ao_app->call_settings_menu();
    append_server_chatmessage("CLIENT", tr("You opened the settings menu."),
                              "1");
    return;
  }
  else if (ooc_message.startsWith("/pair")) {
    ui_ooc_chat_message->clear();
    ooc_message.remove(0, 6);

    bool ok;
    int whom = ooc_message.toInt(&ok);
    if (ok) {
      if (whom > -1) {
        other_charid = whom;
        QString msg = tr("You will now pair up with %1 if they also choose "
                         "your character in return.")
                          .arg(char_list.at(whom).name);
        append_server_chatmessage("CLIENT", msg, "1");
      }
      else {
        other_charid = -1;
        append_server_chatmessage(
            "CLIENT", tr("You are no longer paired with anyone."), "1");
      }
    }
    else {
      append_server_chatmessage("CLIENT",
                                tr("Are you sure you typed that well? The char "
                                   "ID could not be recognised."),
                                "1");
    }
    return;
  }
  else if (ooc_message.startsWith("/offset")) {
    ui_ooc_chat_message->clear();
    ooc_message.remove(0, 8);

    bool ok;
    int off = ooc_message.toInt(&ok);
    if (ok) {
      if (off >= -100 && off <= 100) {
        char_offset = off;
        QString msg = tr("You have set your offset to %1%%.")
          .arg(QString::number(off));
        append_server_chatmessage("CLIENT", msg, "1");
      }
      else {
        append_server_chatmessage(
            "CLIENT", tr("Your offset must be between -100% and 100%!"), "1");
      }
    }
    else {
      append_server_chatmessage("CLIENT",
                                tr("That offset does not look like one."), "1");
    }
    return;
  }
  else if (ooc_message.startsWith("/voffset")) {
    ui_ooc_chat_message->clear();
    ooc_message.remove(0, 8);

    bool ok;
    int off = ooc_message.toInt(&ok);
    if (ok) {
      if (off >= -100 && off <= 100) {
        char_vert_offset = off;
        QString msg = tr("You have set your vertical offset to %1%%.")
          .arg(QString::number(off));
        append_server_chatmessage("CLIENT", msg, "1");
      }
      else {
        append_server_chatmessage(
            "CLIENT", tr("Your vertical offset must be between -100% and 100%!"), "1");
      }
    }
    else {
      append_server_chatmessage("CLIENT",
                                tr("That vertical offset does not look like one."), "1");
    }
    return;
  }
  else if (ooc_message.startsWith("/switch_am")) {
    append_server_chatmessage(
        "CLIENT", tr("You switched your music and area list."), "1");
    on_switch_area_music_clicked();
    ui_ooc_chat_message->clear();
    return;
  }
  else if (ooc_message.startsWith("/enable_blocks")) {
    append_server_chatmessage("CLIENT",
                              tr("You have forcefully enabled features that "
                                 "the server may not support. You may not be "
                                 "able to talk IC, or worse, because of this."),
                              "1");
    ao_app->cccc_ic_support_enabled = true;
    ao_app->arup_enabled = true;
    ao_app->modcall_reason_enabled = true;
    on_reload_theme_clicked();
    ui_ooc_chat_message->clear();
    return;
  }
  else if (ooc_message.startsWith("/non_int_pre")) {
    if (ui_pre_non_interrupt->isChecked())
      append_server_chatmessage(
          "CLIENT", tr("Your pre-animations interrupt again."), "1");
    else
      append_server_chatmessage(
          "CLIENT", tr("Your pre-animations will not interrupt text."), "1");
    ui_pre_non_interrupt->setChecked(!ui_pre_non_interrupt->isChecked());
    ui_ooc_chat_message->clear();
    return;
  }
  else if (ooc_message.startsWith("/save_chatlog")) {
    QFile file("chatlog.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text |
                   QIODevice::Truncate)) {
      append_server_chatmessage(
          "CLIENT", tr("Couldn't open chatlog.txt to write into."), "1");
      ui_ooc_chat_message->clear();
      return;
    }

    QTextStream out(&file);

    foreach (chatlogpiece item, ic_chatlog_history) {
      out << item.get_full() << '\n';
    }

    file.close();

    append_server_chatmessage("CLIENT", tr("The IC chatlog has been saved."),
                              "1");
    ui_ooc_chat_message->clear();
    return;
  }
  else if (ooc_message.startsWith("/load_case")) {
    QStringList command = ooc_message.split(" ", QString::SkipEmptyParts);

    QDir casefolder("base/cases");
    if (!casefolder.exists()) {
      QDir::current().mkdir("base/" + casefolder.dirName());
      append_server_chatmessage(
          "CLIENT",
          tr("You don't have a `base/cases/` folder! It was just made for you, "
             "but seeing as it WAS just made for you, it's likely the case "
             "file you're looking for can't be found in there."),
          "1");
      ui_ooc_chat_message->clear();
      return;
    }
    QStringList caseslist = casefolder.entryList();
    caseslist.removeOne(".");
    caseslist.removeOne("..");
    caseslist.replaceInStrings(".ini", "");

    if (command.size() < 2) {
      append_server_chatmessage(
          "CLIENT",
          tr("You need to give a filename to load (extension not needed)! Make "
             "sure that it is in the `base/cases/` folder, and that it is a "
             "correctly formatted ini.\nCases you can load: %1")
              .arg(caseslist.join(", ")),
          "1");
      ui_ooc_chat_message->clear();
      return;
    }

    if (command.size() > 2) {
      append_server_chatmessage(
          "CLIENT",
          tr("Too many arguments to load a case! You only need one filename, "
             "without extension."),
          "1");
      ui_ooc_chat_message->clear();
      return;
    }

    QSettings casefile("base/cases/" + command[1] + ".ini",
                       QSettings::IniFormat);

    QString caseauth = casefile.value("author", "").value<QString>();
    QString casedoc = casefile.value("doc", "").value<QString>();
    QString cmdoc = casefile.value("cmdoc", "").value<QString>();
    QString casestatus = casefile.value("status", "").value<QString>();

    if (!caseauth.isEmpty())
      append_server_chatmessage(tr("CLIENT"),
                                tr("Case made by %1.").arg(caseauth), "1");
    if (!casedoc.isEmpty())
      ao_app->send_server_packet(new AOPacket("CT#" + ui_ooc_chat_name->text() +
                                              "#/doc " + casedoc + "#%"));
    if (!casestatus.isEmpty())
      ao_app->send_server_packet(new AOPacket("CT#" + ui_ooc_chat_name->text() +
                                              "#/status " + casestatus + "#%"));
    if (!cmdoc.isEmpty())
      append_server_chatmessage(
          "CLIENT", tr("Navigate to %1 for the CM doc.").arg(cmdoc), "1");

    for (int i = local_evidence_list.size() - 1; i >= 0; i--) {
      ao_app->send_server_packet(
          new AOPacket("DE#" + QString::number(i) + "#%"));
    }

    foreach (QString evi, casefile.childGroups()) {
      if (evi == "General")
        continue;

      QStringList f_contents;

      f_contents.append(
          casefile.value(evi + "/name", tr("UNKNOWN")).value<QString>());
      f_contents.append(
          casefile.value(evi + "/description", tr("UNKNOWN")).value<QString>());
      f_contents.append(
          casefile.value(evi + "/image", "UNKNOWN.png").value<QString>());

      ao_app->send_server_packet(new AOPacket("PE", f_contents));
    }

    append_server_chatmessage(
        "CLIENT", tr("Your case \"%1\" was loaded!").arg(command[1]), "1");
    ui_ooc_chat_message->clear();
    return;
  }
  else if (ooc_message.startsWith("/save_case")) {
    QStringList command = ooc_message.split(" ", QString::SkipEmptyParts);

    QDir casefolder("base/cases");
    if (!casefolder.exists()) {
      QDir::current().mkdir("base/" + casefolder.dirName());
      append_server_chatmessage(
          "CLIENT",
          tr("You don't have a `base/cases/` folder! It was just made for you, "
             "but seeing as it WAS just made for you, it's likely that you "
             "somehow deleted it."),
          "1");
      ui_ooc_chat_message->clear();
      return;
    }
    QStringList caseslist = casefolder.entryList();
    caseslist.removeOne(".");
    caseslist.removeOne("..");
    caseslist.replaceInStrings(".ini", "");

    if (command.size() < 3) {
      append_server_chatmessage(
          "CLIENT",
          tr("You need to give a filename to save (extension not needed) and "
             "the courtroom status!"),
          "1");
      ui_ooc_chat_message->clear();
      return;
    }

    if (command.size() > 3) {
      append_server_chatmessage(
          "CLIENT",
          tr("Too many arguments to save a case! You only need a filename "
             "without extension and the courtroom status!"),
          "1");
      ui_ooc_chat_message->clear();
      return;
    }
    QSettings casefile("base/cases/" + command[1] + ".ini",
                       QSettings::IniFormat);
    casefile.setValue("author", ui_ooc_chat_name->text());
    casefile.setValue("cmdoc", "");
    casefile.setValue("doc", "");
    casefile.setValue("status", command[2]);
    casefile.sync();
    for (int i = 0; i < local_evidence_list.size(); i++) {
      QString clean_evidence_dsc = local_evidence_list[i].description.replace(
          QRegularExpression("<owner = ...>..."), "");
      clean_evidence_dsc = clean_evidence_dsc.replace(
          clean_evidence_dsc.lastIndexOf(">"), 1, "");
      casefile.beginGroup(QString::number(i));
      casefile.sync();
      casefile.setValue("name", local_evidence_list[i].name);
      casefile.setValue("description", local_evidence_list[i].description);
      casefile.setValue("image", local_evidence_list[i].image);
      casefile.endGroup();
    }
    casefile.sync();
    append_server_chatmessage(
        "CLIENT", tr("Succesfully saved, edit doc and cmdoc link on the ini!"),
        "1");
    ui_ooc_chat_message->clear();
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
  if (server_ooc) {
    ui_ms_chatlog->show();
    ui_server_chatlog->hide();
    ui_ooc_toggle->setText(tr("Master"));

    server_ooc = false;
  }
  else {
    ui_ms_chatlog->hide();
    ui_server_chatlog->show();
    ui_ooc_toggle->setText(tr("Server"));

    server_ooc = true;
  }
}

// Todo: multithread this due to some servers having large as hell music list
void Courtroom::on_music_search_edited(QString p_text)
{
  // Iterate through all QTreeWidgetItem items
  if (!ui_music_list->isHidden()) {
    QTreeWidgetItemIterator it(ui_music_list);
    while (*it) {
      (*it)->setHidden(p_text != "");
      ++it;
    }
  }

  if (!ui_area_list->isHidden()) {
    QTreeWidgetItemIterator ait(ui_area_list);
    while (*ait) {
      (*ait)->setHidden(p_text != "");
      ++ait;
    }
  }

  if (p_text != "") {
    if (!ui_music_list->isHidden()) {
      // Search in metadata
      QList<QTreeWidgetItem *> clist = ui_music_list->findItems(
          ui_music_search->text(), Qt::MatchContains | Qt::MatchRecursive, 1);
      foreach (QTreeWidgetItem *item, clist) {
        if (item->parent() != nullptr) // So the category shows up too
          item->parent()->setHidden(false);
        item->setHidden(false);
      }
    }

    if (!ui_area_list->isHidden()) {
      // Search in metadata
      QList<QTreeWidgetItem *> alist = ui_area_list->findItems(
          ui_music_search->text(), Qt::MatchContains | Qt::MatchRecursive, 1);
      foreach (QTreeWidgetItem *item, alist) {
        if (item->parent() != nullptr) // So the category shows up too
          item->parent()->setHidden(false);
        item->setHidden(false);
      }
    }
  }
}

void Courtroom::on_music_search_return_pressed()
{
  if (ui_music_search->text() == "") {
    ui_music_list->collapseAll();
  }
}

void Courtroom::on_pos_dropdown_changed(int p_index)
{
  if (p_index < 0 || p_index > 7)
    return;

  toggle_judge_buttons(false);

  QString f_pos = ui_pos_dropdown->itemText(p_index);

  if (f_pos == "")
    return;

  if (f_pos == "jud")
    toggle_judge_buttons(true);

  // YEAH SENDING LIKE 20 PACKETS IF THE USER SCROLLS THROUGH, GREAT IDEA
  // how about this instead
  set_side(f_pos);
}

void Courtroom::set_iniswap_dropdown()
{
  ui_iniswap_dropdown->blockSignals(true);
  ui_iniswap_dropdown->clear();
  if (m_cid == -1) {
    ui_iniswap_dropdown->hide();
    ui_iniswap_remove->hide();
    return;
  }
  QStringList iniswaps = ao_app->get_list_file(
      ao_app->get_character_path(char_list.at(m_cid).name, "iniswaps.ini"));
  iniswaps.prepend(char_list.at(m_cid).name);
  if (iniswaps.size() <= 0) {
    ui_iniswap_dropdown->hide();
    ui_iniswap_remove->hide();
    return;
  }
  ui_iniswap_dropdown->show();
  ui_iniswap_dropdown->addItems(iniswaps);

  for (int i = 0; i < iniswaps.size(); ++i) {
    if (iniswaps.at(i) == current_char) {
      ui_iniswap_dropdown->setCurrentIndex(i);
      if (i != 0)
        ui_iniswap_remove->show();
      else
        ui_iniswap_remove->hide();
      break;
    }
  }
  ui_iniswap_dropdown->blockSignals(false);
}

void Courtroom::on_iniswap_dropdown_changed(int p_index)
{
  ui_ic_chat_message->setFocus();
  QString iniswap = ui_iniswap_dropdown->itemText(p_index);
  ao_app->set_char_ini(char_list.at(m_cid).name, iniswap, "name", "Options");

  QStringList swaplist;
  for (int i = 0; i < ui_iniswap_dropdown->count(); ++i) {
    QString entry = ui_iniswap_dropdown->itemText(i);
    if (!swaplist.contains(entry) && entry != char_list.at(m_cid).name)
      swaplist.append(entry);
  }
  ao_app->write_to_file(
      swaplist.join("\n"),
      ao_app->get_character_path(char_list.at(m_cid).name, "iniswaps.ini"));
  ui_iniswap_dropdown->blockSignals(true);
  ui_iniswap_dropdown->setCurrentIndex(p_index);
  ui_iniswap_dropdown->blockSignals(false);
  update_character(m_cid);
  if (p_index != 0)
    ui_iniswap_remove->show();
  else
    ui_iniswap_remove->hide();
}

void Courtroom::on_iniswap_context_menu_requested(const QPoint &pos)
{
  QMenu *menu = ui_iniswap_dropdown->lineEdit()->createStandardContextMenu();

  menu->addSeparator();
  if (file_exists(ao_app->get_character_path(current_char, "char.ini")))
    menu->addAction(QString("Edit " + current_char + "/char.ini"), this,
                    SLOT(on_iniswap_edit_requested()));
  if (ui_iniswap_dropdown->itemText(ui_iniswap_dropdown->currentIndex()) !=
      char_list.at(m_cid).name)
    menu->addAction(QString("Remove " + current_char), this,
                    SLOT(on_iniswap_remove_clicked()));
  menu->popup(ui_iniswap_dropdown->mapToGlobal(pos));
}
void Courtroom::on_iniswap_edit_requested()
{
  QString p_path = ao_app->get_character_path(current_char, "char.ini");
  if (!file_exists(p_path))
    return;
  QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
}

void Courtroom::on_iniswap_remove_clicked()
{
  if (ui_iniswap_dropdown->count() <= 0) {
    ui_iniswap_remove->hide(); // We're not supposed to see it. Do this or the
                               // client will crash
    return;
  }
  if (ui_iniswap_dropdown->itemText(ui_iniswap_dropdown->currentIndex()) !=
      char_list.at(m_cid).name) {
    ui_iniswap_dropdown->removeItem(ui_iniswap_dropdown->currentIndex());
    on_iniswap_dropdown_changed(0); // Reset back to original
    update_character(m_cid);
  }
}

void Courtroom::set_sfx_dropdown()
{
  ui_sfx_dropdown->blockSignals(true);
  ui_sfx_dropdown->clear();
  if (m_cid == -1) {
    ui_sfx_dropdown->hide();
    ui_sfx_remove->hide();
    return;
  }
  QStringList soundlist = ao_app->get_list_file(
      ao_app->get_character_path(current_char, "soundlist.ini"));

  if (soundlist.size() <= 0) {
    soundlist = ao_app->get_list_file(
        ao_app->get_theme_path("character_soundlist.ini"));
    if (soundlist.size() <= 0) {
      soundlist = ao_app->get_list_file(
          ao_app->get_default_theme_path("character_soundlist.ini"));
    }
  }

  if (soundlist.size() <= 0) {
    ui_sfx_dropdown->hide();
    ui_sfx_remove->hide();
    return;
  }
  soundlist.prepend("Default");

  ui_sfx_dropdown->show();
  ui_sfx_dropdown->addItems(soundlist);
  ui_sfx_dropdown->setCurrentIndex(0);
  ui_sfx_remove->hide();
  ui_sfx_dropdown->blockSignals(false);
}

void Courtroom::on_sfx_dropdown_changed(int p_index)
{
  ui_ic_chat_message->setFocus();

  QStringList soundlist;
  for (int i = 0; i < ui_sfx_dropdown->count(); ++i) {
    QString entry = ui_sfx_dropdown->itemText(i);
    if (!soundlist.contains(entry) && entry != "Default")
      soundlist.append(entry);
  }

  QStringList defaultlist =
      ao_app->get_list_file(ao_app->get_theme_path("character_soundlist.ini"));
  if (defaultlist.size() <= 0) {
    defaultlist = ao_app->get_list_file(
        ao_app->get_default_theme_path("character_soundlist.ini"));
  }

  if (defaultlist.size() > 0 &&
      defaultlist.toSet().subtract(soundlist.toSet()).size() >
          0) // There's a difference from the default configuration
    ao_app->write_to_file(
        soundlist.join("\n"),
        ao_app->get_character_path(current_char,
                                   "soundlist.ini")); // Create a new sound list

  ui_sfx_dropdown->blockSignals(true);
  ui_sfx_dropdown->setCurrentIndex(p_index);
  ui_sfx_dropdown->blockSignals(false);
  if (p_index != 0)
    ui_sfx_remove->show();
  else
    ui_sfx_remove->hide();
}

void Courtroom::on_sfx_context_menu_requested(const QPoint &pos)
{
  QMenu *menu = ui_sfx_dropdown->lineEdit()->createStandardContextMenu();

  menu->addSeparator();
  if (file_exists(ao_app->get_character_path(current_char, "soundlist.ini")))
    menu->addAction(QString("Edit " + current_char + "/soundlist.ini"), this,
                    SLOT(on_sfx_edit_requested()));
  else
    menu->addAction(QString("Edit theme's character_soundlist.ini"), this,
                    SLOT(on_sfx_edit_requested()));
  if (ui_sfx_dropdown->currentIndex() != 0)
    menu->addAction(QString("Remove " + ui_sfx_dropdown->itemText(
                                            ui_sfx_dropdown->currentIndex())),
                    this, SLOT(on_sfx_remove_clicked()));
  menu->popup(ui_sfx_dropdown->mapToGlobal(pos));
}
void Courtroom::on_sfx_edit_requested()
{
  QString p_path = ao_app->get_character_path(current_char, "soundlist.ini");
  if (!file_exists(p_path)) {
    p_path = ao_app->get_theme_path("character_soundlist.ini");
    if (!file_exists(p_path)) {
      p_path = ao_app->get_default_theme_path("character_soundlist.ini");
      if (!file_exists(p_path)) {
        return;
      }
    }
  }
  QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
}

void Courtroom::on_sfx_remove_clicked()
{
  if (ui_sfx_dropdown->count() <= 0) {
    ui_sfx_remove->hide(); // We're not supposed to see it. Do this or the
                           // client will crash
    return;
  }
  if (ui_sfx_dropdown->itemText(ui_sfx_dropdown->currentIndex()) != "Default") {
    ui_sfx_dropdown->removeItem(ui_sfx_dropdown->currentIndex());
    on_sfx_dropdown_changed(0); // Reset back to original
  }
}

void Courtroom::set_effects_dropdown()
{
  ui_effects_dropdown->blockSignals(true);
  ui_effects_dropdown->clear();
  if (m_cid == -1) {
    ui_effects_dropdown->hide();
    return;
  }
  QStringList effectslist = ao_app->get_effects(current_char);

  if (effectslist.size() <= 0) {
    ui_effects_dropdown->hide();
    return;
  }

  effectslist.prepend(tr("None"));

  ui_effects_dropdown->show();
  ui_effects_dropdown->addItems(effectslist);

  // ICON-MAKING HELL
  QString p_effect = ao_app->read_char_ini(current_char, "effects", "Options");
  QString custom_path =
      ao_app->get_base_path() + "misc/" + p_effect + "/icons/";
  QString theme_path = ao_app->get_theme_path("effects/icons/");
  QString default_path = ao_app->get_default_theme_path("effects/icons/");
  for (int i = 0; i < ui_effects_dropdown->count(); ++i) {
    QString entry = ui_effects_dropdown->itemText(i);
    QString iconpath = ao_app->get_static_image_suffix(custom_path + entry);
    if (!file_exists(iconpath)) {
      iconpath = ao_app->get_static_image_suffix(theme_path + entry);
      if (!file_exists(iconpath)) {
        iconpath = ao_app->get_static_image_suffix(default_path + entry);
        if (!file_exists(iconpath))
          continue;
      }
    }
    ui_effects_dropdown->setItemIcon(i, QIcon(iconpath));
  }

  ui_effects_dropdown->setCurrentIndex(0);
  ui_effects_dropdown->blockSignals(false);
}

void Courtroom::on_effects_context_menu_requested(const QPoint &pos)
{
  QMenu *menu = new QMenu();

  if (!ao_app->read_char_ini(current_char, "effects", "Options").isEmpty())
    menu->addAction(
        QString("Open misc/" +
                ao_app->read_char_ini(current_char, "effects", "Options") +
                " folder"),
        this, SLOT(on_character_effects_edit_requested()));
  menu->addAction(QString("Open theme's effects folder"), this,
                  SLOT(on_effects_edit_requested()));
  menu->popup(ui_effects_dropdown->mapToGlobal(pos));
}
void Courtroom::on_effects_edit_requested()
{
  QString p_path = ao_app->get_theme_path("effects/");
  if (!dir_exists(p_path)) {
    p_path = ao_app->get_default_theme_path("effects/");
    if (!dir_exists(p_path)) {
      return;
    }
  }
  QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
}
void Courtroom::on_character_effects_edit_requested()
{
  QString p_effect = ao_app->read_char_ini(current_char, "effects", "Options");
  QString p_path = ao_app->get_base_path() + "misc/" + p_effect + "/";
  if (!dir_exists(p_path))
    return;

  QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
}

void Courtroom::on_effects_dropdown_changed(int p_index)
{
  effect = ui_effects_dropdown->itemText(p_index);
  ui_ic_chat_message->setFocus();
}

bool Courtroom::effects_dropdown_find_and_set(QString effect)
{
  for (int i = 0; i < ui_effects_dropdown->count(); ++i) {
    QString entry = ui_effects_dropdown->itemText(i);
    if (entry == effect) {
      ui_effects_dropdown->setCurrentIndex(i);
      return true;
    }
  }
  return false;
}

QString Courtroom::get_char_sfx()
{
  QString sfx = ui_sfx_dropdown->itemText(ui_sfx_dropdown->currentIndex());
  if (sfx != "" && sfx != "Default")
    return sfx;
  return ao_app->get_sfx_name(current_char, current_emote);
}

int Courtroom::get_char_sfx_delay()
{
  //  QString sfx = ui_sfx_dropdown->itemText(ui_sfx_dropdown->currentIndex());
  //  if (sfx != "" && sfx != "Default")
  //    return 0; //todo: a way to define this
  return ao_app->get_sfx_delay(current_char, current_emote);
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

  for (int n_char = 0; n_char < char_list.size(); n_char++) {
    if (char_list.at(n_char).name == real_char)
      f_cid = n_char;
  }

  if (f_cid < 0 || f_cid >= char_list.size()) {
    qDebug() << "W: " << real_char << " not present in char_list";
    return;
  }

  if (mute_map.value(f_cid)) {
    mute_map.insert(f_cid, false);
    f_item->setText(real_char);
  }
  else {
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

  if (f_char.endsWith(" [x]")) {
    real_char = f_char.left(f_char.size() - 4);
    f_item->setText(real_char);
  }
  else {
    real_char = f_char;
    for (int n_char = 0; n_char < char_list.size(); n_char++) {
      if (char_list.at(n_char).name == real_char)
        f_cid = n_char;
    }
  }

  if (f_cid < -2 || f_cid >= char_list.size()) {
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
  if (other_charid != -1) {
    f_item->setText(real_char + " [x]");
  }
}

void Courtroom::on_music_list_double_clicked(QTreeWidgetItem *p_item,
                                             int column)
{
  if (is_muted)
    return;
  if (p_item->parent() == nullptr) // i.e. we've clicked a category
    return;
  column = 1; // Column 1 is always the metadata (which we want)
  QString p_song = p_item->text(column);
  QStringList packet_contents;
  packet_contents.append(p_song);
  packet_contents.append(QString::number(m_cid));
  if ((!ui_ic_chat_name->text().isEmpty() && ao_app->cccc_ic_support_enabled) ||
      ao_app->effects_enabled)
    packet_contents.append(ui_ic_chat_name->text());
  if (ao_app->effects_enabled)
    packet_contents.append(QString::number(music_flags));
  ao_app->send_server_packet(new AOPacket("MC", packet_contents), false);
}

void Courtroom::on_music_list_context_menu_requested(const QPoint &pos)
{
  QMenu *menu = new QMenu();
  menu->addAction(QString(tr("Stop Current Song")), this, SLOT(music_stop()));
  menu->addAction(QString(tr("Play Random Song")), this, SLOT(music_random()));
  menu->addSeparator();
  menu->addAction(QString(tr("Expand All Categories")), this,
                  SLOT(music_list_expand_all()));
  menu->addAction(QString(tr("Collapse All Categories")), this,
                  SLOT(music_list_collapse_all()));
  menu->addSeparator();

  menu->addAction(new QAction(tr("Fade Out Previous"), this));
  menu->actions().back()->setCheckable(true);
  menu->actions().back()->setChecked(music_flags & FADE_OUT);
  connect(menu->actions().back(), SIGNAL(toggled(bool)), this,
          SLOT(music_fade_out(bool)));

  menu->addAction(new QAction(tr("Fade In"), this));
  menu->actions().back()->setCheckable(true);
  menu->actions().back()->setChecked(music_flags & FADE_IN);
  connect(menu->actions().back(), SIGNAL(toggled(bool)), this,
          SLOT(music_fade_in(bool)));

  menu->addAction(new QAction(tr("Synchronize"), this));
  menu->actions().back()->setCheckable(true);
  menu->actions().back()->setChecked(music_flags & SYNC_POS);
  connect(menu->actions().back(), SIGNAL(toggled(bool)), this,
          SLOT(music_synchronize(bool)));

  menu->popup(ui_music_list->mapToGlobal(pos));
}

void Courtroom::music_fade_out(bool toggle)
{
  if (toggle)
    music_flags |= FADE_OUT;
  else
    music_flags &= ~FADE_OUT;
}

void Courtroom::music_fade_in(bool toggle)
{
  if (toggle)
    music_flags |= FADE_IN;
  else
    music_flags &= ~FADE_IN;
}

void Courtroom::music_synchronize(bool toggle)
{
  if (toggle)
    music_flags |= SYNC_POS;
  else
    music_flags &= ~SYNC_POS;
}

void Courtroom::music_random()
{
  QList<QTreeWidgetItem *> clist;
  QTreeWidgetItemIterator it(ui_music_list,
                             QTreeWidgetItemIterator::NotHidden |
                                 QTreeWidgetItemIterator::NoChildren);
  while (*it) {
    clist += (*it);
    ++it;
  }
  int i = qrand() % clist.length();
  on_music_list_double_clicked(clist.at(i), 1);
}

void Courtroom::music_list_expand_all() { ui_music_list->expandAll(); }
void Courtroom::music_list_collapse_all()
{
  ui_music_list->collapseAll();
  QTreeWidgetItem *current = ui_music_list->selectedItems()[0];
  if (current->parent() != nullptr)
    current = current->parent();
  ui_music_list->setCurrentItem(current);
}

void Courtroom::music_stop()
{               // send a fake music packet with a nonexistent song
  if (is_muted) // this requires a special exception for "~stop.mp3" in
    return;     // tsuserver3, as it will otherwise reject songs not on
  QStringList packet_contents; // its music list
  packet_contents.append(
      "~stop.mp3"); // this is our fake song, playing it triggers special code
  packet_contents.append(QString::number(m_cid));
  if ((!ui_ic_chat_name->text().isEmpty() && ao_app->cccc_ic_support_enabled) ||
      ao_app->effects_enabled)
    packet_contents.append(ui_ic_chat_name->text());
  if (ao_app->effects_enabled)
    packet_contents.append(QString::number(music_flags));
  ao_app->send_server_packet(new AOPacket("MC", packet_contents), false);
}

void Courtroom::on_area_list_double_clicked(QTreeWidgetItem *p_item, int column)
{
  column = 0; // The metadata
  QString p_area = p_item->text(0);

  QStringList packet_contents;
  packet_contents.append(p_area);
  packet_contents.append(QString::number(m_cid));
  qDebug() << packet_contents;
  ao_app->send_server_packet(new AOPacket("MC", packet_contents), false);
}

void Courtroom::on_hold_it_clicked()
{
  if (objection_state == 1) {
    ui_hold_it->set_image("holdit");
    objection_state = 0;
  }
  else {
    ui_objection->set_image("objection");
    ui_take_that->set_image("takethat");
    ui_custom_objection->set_image("custom");

    ui_hold_it->set_image("holdit_selected");
    objection_state = 1;
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_objection_clicked()
{
  if (objection_state == 2) {
    ui_objection->set_image("objection");
    objection_state = 0;
  }
  else {
    ui_hold_it->set_image("holdit");
    ui_take_that->set_image("takethat");
    ui_custom_objection->set_image("custom");

    ui_objection->set_image("objection_selected");
    objection_state = 2;
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_take_that_clicked()
{
  if (objection_state == 3) {
    ui_take_that->set_image("takethat");
    objection_state = 0;
  }
  else {
    ui_objection->set_image("objection");
    ui_hold_it->set_image("holdit");
    ui_custom_objection->set_image("custom");

    ui_take_that->set_image("takethat_selected");
    objection_state = 3;
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_custom_objection_clicked()
{
  if (objection_state == 4) {
    ui_custom_objection->set_image("custom");
    objection_state = 0;
  }
  else {
    ui_objection->set_image("objection");
    ui_take_that->set_image("takethat");
    ui_hold_it->set_image("holdit");

    ui_custom_objection->set_image("custom_selected");
    objection_state = 4;
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::show_custom_objection_menu(const QPoint &pos)
{
  QPoint globalPos = ui_custom_objection->mapToGlobal(pos);
  QAction *selecteditem = custom_obj_menu->exec(globalPos);
  if (selecteditem) {
    ui_objection->set_image("objection");
    ui_take_that->set_image("takethat");
    ui_hold_it->set_image("holdit");
    ui_custom_objection->set_image("custom_selected");
    if (selecteditem->text() == "Default")
      objection_custom = "";
    else
      objection_custom = selecteditem->text();
    objection_state = 4;
    custom_obj_menu->setDefaultAction(selecteditem);
  }
}

void Courtroom::on_realization_clicked()
{
  if (realization_state == 0) {
    realization_state = 1;
    if (effects_dropdown_find_and_set("realization"))
      on_effects_dropdown_changed(ui_effects_dropdown->currentIndex());

    ui_realization->set_image("realization_pressed");
  }
  else {
    realization_state = 0;
    ui_effects_dropdown->setCurrentIndex(0);
    on_effects_dropdown_changed(ui_effects_dropdown->currentIndex());
    ui_realization->set_image("realization");
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_screenshake_clicked()
{
  if (screenshake_state == 0) {
    screenshake_state = 1;
    ui_screenshake->set_image("screenshake_pressed");
  }
  else {
    screenshake_state = 0;
    ui_screenshake->set_image("screenshake");
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_mute_clicked()
{
  if (ui_mute_list->isHidden()) {
    ui_mute_list->show();
    ui_pair_list->hide();
    ui_pair_offset_spinbox->hide();
    ui_pair_vert_offset_spinbox->hide();
    ui_pair_order_dropdown->hide();
    ui_pair_button->set_image("pair_button");
    ui_mute->set_image("mute_pressed");
  }
  else {
    ui_mute_list->hide();
    ui_mute->set_image("mute");
  }
}

void Courtroom::on_pair_clicked()
{
  if (ui_pair_list->isHidden()) {
    ui_pair_list->show();
    ui_pair_offset_spinbox->show();
    ui_pair_vert_offset_spinbox->show();
    ui_pair_order_dropdown->show();
    ui_mute_list->hide();
    ui_mute->set_image("mute");
    ui_pair_button->set_image("pair_button_pressed");
  }
  else {
    ui_pair_list->hide();
    ui_pair_offset_spinbox->hide();
    ui_pair_vert_offset_spinbox->hide();
    ui_pair_order_dropdown->hide();
    ui_pair_button->set_image("pair_button");
  }
}

void Courtroom::on_pair_order_dropdown_changed(int p_index)
{
  pair_order = p_index;
}

void Courtroom::on_defense_minus_clicked()
{
  int f_state = defense_bar_state - 1;

  if (f_state >= 0)
    ao_app->send_server_packet(
        new AOPacket("HP#1#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_defense_plus_clicked()
{
  int f_state = defense_bar_state + 1;

  if (f_state <= 10)
    ao_app->send_server_packet(
        new AOPacket("HP#1#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_prosecution_minus_clicked()
{
  int f_state = prosecution_bar_state - 1;

  if (f_state >= 0)
    ao_app->send_server_packet(
        new AOPacket("HP#2#" + QString::number(f_state) + "#%"));
}

void Courtroom::on_prosecution_plus_clicked()
{
  int f_state = prosecution_bar_state + 1;

  if (f_state <= 10)
    ao_app->send_server_packet(
        new AOPacket("HP#2#" + QString::number(f_state) + "#%"));
}

void Courtroom::set_text_color_dropdown()
{
  // Clear the lists
  ui_text_color->clear();
  color_row_to_number.clear();

  // Clear the stored optimization information
  color_rgb_list.clear();
  color_markdown_start_list.clear();
  color_markdown_end_list.clear();
  color_markdown_remove_list.clear();
  color_markdown_talking_list.clear();

  // Update markdown colors. TODO: make a loading function that only loads the
  // config file once instead of several times
  for (int c = 0; c < max_colors; ++c) {
    QColor color =
        ao_app->get_chat_color("c" + QString::number(c), current_char);
    color_rgb_list.append(color);
    color_markdown_start_list.append(ao_app->get_chat_markdown(
        "c" + QString::number(c) + "_start", current_char));
    color_markdown_end_list.append(ao_app->get_chat_markdown(
        "c" + QString::number(c) + "_end", current_char));
    color_markdown_remove_list.append(
        ao_app->get_chat_markdown("c" + QString::number(c) + "_remove",
                                  current_char) == "1");
    color_markdown_talking_list.append(
        ao_app->get_chat_markdown("c" + QString::number(c) + "_talking",
                                  current_char) != "0");

    QString color_name = ao_app->get_chat_markdown(
        "c" + QString::number(c) + "_name", current_char);
    if (color_name.isEmpty()) // Not defined
    {
      if (c > 0)
        continue;
      color_name = tr("Default");
    }
    ui_text_color->addItem(color_name);
    QPixmap pixmap(16, 16);
    pixmap.fill(color);
    ui_text_color->setItemIcon(ui_text_color->count() - 1, QIcon(pixmap));
    color_row_to_number.append(c);
  }
}

void Courtroom::on_text_color_changed(int p_color)
{
  if (ui_ic_chat_message->selectionStart() != -1) // We have a selection!
  {
    int c = color_row_to_number.at(p_color);
    QString markdown_start = color_markdown_start_list.at(c);
    if (markdown_start.isEmpty()) {
      qDebug() << "W: Color list dropdown selected a non-existent markdown "
                  "start character";
      return;
    }
    QString markdown_end = color_markdown_end_list.at(c);
    if (markdown_end.isEmpty())
      markdown_end = markdown_start;
    int start = ui_ic_chat_message->selectionStart();
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    int end = ui_ic_chat_message->selectionEnd() + 1;
#else
    int end = ui_ic_chat_message->selectedText().length() + 1;
#endif

    ui_ic_chat_message->setCursorPosition(start);
    ui_ic_chat_message->insert(markdown_start);
    ui_ic_chat_message->setCursorPosition(end);
    ui_ic_chat_message->insert(markdown_end);
    //    ui_ic_chat_message->end(false);
    ui_text_color->setCurrentIndex(0);
  }
  else {
    if (p_color != -1 && p_color < color_row_to_number.size())
      text_color = color_row_to_number.at(p_color);
    else
      text_color = 0;
  }
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_music_slider_moved(int p_value)
{
  music_player->set_volume(p_value, 0); // Set volume on music layer
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_sfx_slider_moved(int p_value)
{
  sfx_player->set_volume(p_value);
  // Set the ambience and other misc. music layers
  for (int i = 1; i < music_player->m_channelmax; ++i) {
    music_player->set_volume(p_value, i);
  }
  objection_player->set_volume(p_value);
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_blip_slider_moved(int p_value)
{
  blip_player->set_volume(p_value);
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_log_limit_changed(int value) { log_maximum_blocks = value; }

void Courtroom::on_pair_offset_changed(int value) { char_offset = value; }

void Courtroom::on_pair_vert_offset_changed(int value) { char_vert_offset = value; }

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
  sfx_player->set_volume(0);
  blip_player->set_volume(0);

  set_char_select();

  ui_char_select_background->show();
}

void Courtroom::on_reload_theme_clicked()
{
  ao_app->reload_theme();

  enter_courtroom();
  update_character(m_cid);

  anim_state = 4;
  text_state = 3;
  objection_custom = "";

  // to update status on the background
  set_background(current_background, true);
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

void Courtroom::on_spectator_clicked() { char_clicked(-1); }

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
    }
    else if (text.length() > 256) {
      errorBox.critical(nullptr, tr("Error"), tr("The message is too long."));
      return;
    }

    QStringList mod_reason;
    mod_reason.append(text);

    ao_app->send_server_packet(new AOPacket("ZZ", mod_reason));
  }
  else {
    ao_app->send_server_packet(new AOPacket("ZZ#%"));
  }

  ui_ic_chat_message->setFocus();
}

void Courtroom::on_settings_clicked() { ao_app->call_settings_menu(); }

void Courtroom::on_announce_casing_clicked()
{
  ao_app->call_announce_menu(this);
}

void Courtroom::on_pre_clicked() { ui_ic_chat_message->setFocus(); }

void Courtroom::on_flip_clicked() { ui_ic_chat_message->setFocus(); }

void Courtroom::on_additive_clicked()
{
  if (ui_additive->isChecked()) {
    ui_ic_chat_message->home(false); // move cursor to the start of the message
    ui_ic_chat_message->insert(" "); // preface the message by whitespace
    ui_ic_chat_message->end(false);  // move cursor to the end of the message
                                     // without selecting anything
  }
  ui_ic_chat_message->setFocus();
}

void Courtroom::on_guard_clicked() { ui_ic_chat_message->setFocus(); }

void Courtroom::on_showname_enable_clicked()
{
  regenerate_ic_chatlog();
  ui_ic_chat_message->setFocus();
}

void Courtroom::regenerate_ic_chatlog()
{
  ui_ic_chatlog->clear();
  foreach (chatlogpiece item, ic_chatlog_history) {
    append_ic_text(item.get_message(),
                   ui_showname_enable->isChecked() ? item.get_showname()
                                                   : item.get_name(),
                   item.get_action(), item.get_chat_color());
  }
}

void Courtroom::on_evidence_button_clicked()
{
  if (ui_evidence->isHidden()) {
    ui_evidence->show();
    ui_evidence_overlay->hide();
  }
  else {
    ui_evidence->hide();
  }
}

void Courtroom::on_switch_area_music_clicked()
{
  ui_music_search->setText("");
  on_music_search_edited(ui_music_search->text());
  if (ui_area_list->isHidden()) {
    ui_area_list->show();
    ui_music_list->hide();
  }
  else {
    ui_area_list->hide();
    ui_music_list->show();
  }
}

void Courtroom::ping_server()
{
  ao_app->send_server_packet(
      new AOPacket("CH#" + QString::number(m_cid) + "#%"));
}

void Courtroom::on_casing_clicked()
{
  if (ao_app->casing_alerts_enabled) {
    if (ui_casing->isChecked()) {
      QStringList f_packet;

      f_packet.append(ao_app->get_casing_can_host_cases());
      f_packet.append(QString::number(ao_app->get_casing_cm_enabled()));
      f_packet.append(QString::number(ao_app->get_casing_defence_enabled()));
      f_packet.append(
          QString::number(ao_app->get_casing_prosecution_enabled()));
      f_packet.append(QString::number(ao_app->get_casing_judge_enabled()));
      f_packet.append(QString::number(ao_app->get_casing_juror_enabled()));
      f_packet.append(QString::number(ao_app->get_casing_steno_enabled()));

      ao_app->send_server_packet(new AOPacket("SETCASE", f_packet));
    }
    else
      ao_app->send_server_packet(new AOPacket("SETCASE#\"\"#0#0#0#0#0#0#%"));
  }
}

void Courtroom::announce_case(QString title, bool def, bool pro, bool jud,
                              bool jur, bool steno)
{
  if (ao_app->casing_alerts_enabled) {
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
