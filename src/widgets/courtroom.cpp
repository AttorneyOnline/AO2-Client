#include "aouiloader.h"
#include "widgets/aoviewport.h"
#include "widgets/courtroom.h"

#include <QLayout>

Courtroom::Courtroom(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

  // Change the default audio output device to be the one the user has given
  // in his config.ini file for now.
  int a = 0;
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
              BASS_Init(a, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
              load_bass_opus_plugin();
              qDebug() << info.name << "was set as the default audio output device.";
              break;
          }
      }
  }

  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/courtroom.ui");
  uiFile.open(QFile::ReadOnly);
  QMainWindow *windowWidget = static_cast<QMainWindow *>(loader.load(&uiFile, this));
  QMetaObject::connectSlotsByName(this);

//  QVBoxLayout *parentLayout = new QVBoxLayout;
//  parentLayout->addWidget(windowWidget);
//  setLayout(parentLayout);
  windowWidget->show();

//  setWindowTitle("Attorney Online 2");
//  setWindowIcon(QIcon(":/logo.png"));
//  setFixedSize(windowWidget->size());

  windowWidget->centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
  ui_viewport = findChild<AOViewport *>("viewport");
  ui_ic_chatlog = findChild<AOICLog *>("ic_chatlog");
  ui_ms_chat = findChild<AOServerChat *>("ms_chat");
  ui_server_chat = findChild<AOServerChat *>("server_chat");
  ui_music_list = findChild<AOJukebox *>("jukebox");
  ui_area_list = findChild<AORoomChooser *>("rooms");
  ui_mixer = findChild<AOMixer *>("mixer");
  ui_ic_chat = findChild<AOChat *>("ic_chat");
  ui_room_controls = findChild<AORoomControls *>("room_controls");

  keepalive_timer = new QTimer(this);
  keepalive_timer->start(60000);

  char_button_mapper = new QSignalMapper(this);

  music_player = new AOMusicPlayer(this, ao_app);
  music_player->set_volume(0);

  modcall_player = new AOSfxPlayer(this, ao_app);
  modcall_player->set_volume(50);

  ui_background = new AOImage(this, ao_app);

  ui_change_character = new AOButton(this, ao_app);
  ui_reload_theme = new AOButton(this, ao_app);
  ui_call_mod = new AOButton(this, ao_app);
  ui_settings = new AOButton(this, ao_app);
  ui_announce_casing = new AOButton(this, ao_app);
  ui_switch_area_music = new AOButton(this, ao_app);

  ui_guard = new QCheckBox(this);
  ui_guard->setText("Guard");
  ui_guard->hide();
  ui_casing = new QCheckBox(this);
  ui_casing->setChecked(ao_app->get_casing_enabled());
  ui_casing->setText(tr("Casing"));
  ui_casing->hide();

  ui_showname_enable = new QCheckBox(this);
  ui_showname_enable->setChecked(ao_app->get_showname_enabled_by_default());
  ui_showname_enable->setText(tr("Shownames"));

  ui_mute = new AOButton(this, ao_app);

  ui_text_color = new QComboBox(this);
  ui_text_color->addItem("White");
  ui_text_color->addItem("Green");
  ui_text_color->addItem("Red");
  ui_text_color->addItem("Orange");
  ui_text_color->addItem("Blue");
  ui_text_color->addItem("Yellow");
  ui_text_color->addItem("Rainbow");
  ui_text_color->addItem("Pink");
  ui_text_color->addItem("Cyan");

  ui_mute_list = new QListWidget(this);
  ui_pair_list = new QListWidget(this);
  ui_pair_offset_spinbox = new QSpinBox(this);
  ui_pair_offset_spinbox->setRange(-100,100);
  ui_pair_offset_spinbox->setSuffix("% offset");
  ui_pair_button = new AOButton(this, ao_app);

  ui_evidence_button = new AOButton(this, ao_app);

  construct_char_select();

  connect(keepalive_timer, SIGNAL(timeout()), this, SLOT(ping_server()));

  connect(ui_mute_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_mute_list_clicked(QModelIndex)));

  connect(ui_server_chat, SIGNAL(messageSent(QString, QString)), this, SLOT(on_ooc_return_pressed(QString, QString)));
  connect(ui_ms_chat, SIGNAL(messageSent(QString, QString)), this, SLOT(on_ms_return_pressed(QString, QString)));

  connect(ui_mute, SIGNAL(clicked()), this, SLOT(on_mute_clicked()));

  connect(ui_text_color, SIGNAL(currentIndexChanged(int)), this, SLOT(on_text_color_changed(int)));

  connect(ui_casing, SIGNAL(clicked()), this, SLOT(on_casing_clicked()));

  connect(ui_showname_enable, SIGNAL(clicked()), this, SLOT(on_showname_enable_clicked()));

  connect(ui_pair_button, SIGNAL(clicked()), this, SLOT(on_pair_clicked()));
  connect(ui_pair_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_pair_list_clicked(QModelIndex)));
  connect(ui_pair_offset_spinbox, SIGNAL(valueChanged(int)), this, SLOT(on_pair_offset_changed(int)));

  connect(ui_mixer, &AOMixer::volumeChanged, this, &Courtroom::setVolume);

  set_widgets();

  set_char_select();
}

void Courtroom::arup_modify(int type, int place, QString value)
{
  if (place >= area_list.length() || place < 0)
  {
    qWarning() << tr("ARUP area %1 out of bounds").arg(place);
    return;
  }

  switch (type)
  {
  case ARUP_PARAM::PLAYERS:
    area_list[place].players = value.toInt();
    break;
  case ARUP_PARAM::STATUS:
    area_list[place].status = value;
    break;
  case ARUP_PARAM::CM:
    area_list[place].cm = value;
    break;
  case ARUP_PARAM::LOCKED:
    area_list[place].locked = value;
    break;
  default:
    qWarning() << tr("Unknown ARUP parameter %1").arg(type);
  }

  set_areas(area_list);
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

  set_size_and_pos(ui_guard, "guard");

  set_size_and_pos(ui_casing, "casing");

  set_size_and_pos(ui_showname_enable, "showname_enable");

  set_size_and_pos(ui_mute, "mute_button");
  ui_mute->set_image("mute.png");

  set_size_and_pos(ui_text_color, "text_color");

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
}

void Courtroom::set_fonts()
{
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

  widget->setFont(QFont("Sans", f_weight));

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
  ui_viewport->stop_sounds();

  set_char_select_page();

  set_mute_list();
  set_pair_list();

  set_char_select();

  show();

  ui_spectator->show();
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
  current_emote = 0;

  current_evidence_page = 0;
  current_evidence = 0;

  set_evidence_page();

  ui_ic_chat->setCharacter(f_char);

  QString side = ao_app->get_char_side(f_char);

  ui_room_controls->toggleJudgeControls(side == "jud");

  if (ao_app->casing_alerts_enabled)
    ui_casing->show();
  else
    ui_casing->hide();

  set_widgets();

  ui_char_select_background->hide();
}

void Courtroom::append_ms_chatmessage(QString f_name, QString f_message)
{
  ui_ms_chat->append_chat_message(f_name, f_message, ao_app->get_color("ooc_default_color", "courtroom_design.ini").name());
}

void Courtroom::append_server_chatmessage(QString p_name, QString p_message, QString p_colour)
{
  QString colour = "#000000";

  if (p_colour == "0")
    colour = ao_app->get_color("ooc_default_color", "courtroom_design.ini").name();
  if (p_colour == "1")
    colour = ao_app->get_color("ooc_server_color", "courtroom_design.ini").name();

  ui_server_chat->append_chat_message(p_name, p_message, colour);
}

void Courtroom::on_ic_chat_sendMessage(chat_message_type message)
{
  if (ui_viewport->is_busy() && objection_state == 0)
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

  message.char_id = m_cid;

  // We send over whom we're paired with, unless we have chosen ourselves.
  // Or a charid of -1 or lower, through some means.
  if (message.pair_char_id == -1 || message.pair_char_id == message.char_id)
  {
    message.pair_char_id = -1;
    message.pair_offset = 0;
  }

  QStringList packet_contents = {
    QString::number(message.desk),
    message.preanim,
    message.character,
    message.anim,
    message.side,
    message.sfx_name,
    QString::number(message.emote_modifier),
    QString::number(message.char_id),
    QString::number(message.sfx_delay),
    QString::number(message.objection_modifier),

    // the evidence index is shifted by 1 because 0 is no evidence per legacy standards
    QString::number(current_evidence + 1),

    QString::number(message.flip),
    QString::number(message.realization),
    QString::number(message.text_color),
    message.showname,
    QString::number(message.pair_char_id),
    QString::number(message.pair_offset),
    QString::number(message.noninterrupting_preanim)
  };

  ao_app->send_server_packet(new AOPacket("MS", packet_contents));
}

void Courtroom::handle_chatmessage(QStringList *p_contents)
{
  // Instead of checking for whether a message has at least chatmessage_size
  // amount of packages, we'll check if it has at least 15.
  // That was the original chatmessage_size.
  if (p_contents->size() < 15)
  {
    qWarning() << "IC message is too short:" << p_contents->size();
    return;
  }

  chat_message_type message(*p_contents);

  if (message.char_id < 0 || message.char_id >= char_list.size() ||
      mute_map.value(message.char_id))
    return;

  QString f_showname;
  QString f_default_showname = ao_app->get_showname(char_list.at(message.char_id).name);
  if (message.showname.isEmpty() || !ui_showname_enable->isChecked())
  {
      f_showname = f_default_showname;
  }
  else
  {
      f_showname = message.showname;
  }

  QString f_message = f_showname + ": " + message.message + '\n';

  if (f_message == previous_ic_message)
    return;

  // Were we the sender of the message? If so, reset IC widgets.
  if (message.char_id == m_cid)
  {
    ui_ic_chat_message->clear();
    objection_state = 0;
    realization_state = 0;
    is_presenting_evidence = false;
    ui_evidence_present->set_image("present_disabled.png");
    ui_ic_chat->clearEntry();
  }

  // Append to IC box
  ui_ic_chatlog->append_ic_text(f_default_showname, f_showname, ": " + message.message);

  // Send message to viewport
  previous_ic_message = f_message;

  // Parse evidence
  int f_evi_id = message.evidence;

  QString evidence_image;
  if (f_evi_id > 0 && f_evi_id <= local_evidence_list.size())
  {
    //shifted by 1 because 0 is no evidence per legacy standards
    evidence_image = local_evidence_list.at(f_evi_id - 1).image;
  }

  // Check if any word in the call words list is contained in the message
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

  ui_viewport->chat(message, f_showname, evidence_image);
}

void Courtroom::handle_background(QString background)
{
  ui_viewport->set_background(background);
}

void Courtroom::handle_wtce(QString wtce, int variant)
{
  ui_viewport->wtce(wtce, variant);
}

void Courtroom::set_ip_list(QString p_list)
{
  QString f_list = p_list.replace("|", ":").replace("*", "\n");

  ui_server_chat->append_text(f_list);
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
      ui_ic_chatlog->append_ic_text(str_show, str_show, f_song_clear, true);
      music_player->play(f_song);
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

void Courtroom::mod_called(QString p_ip)
{
  ui_server_chat->append_text(p_ip);
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
    ui_server_chat->append_text(msg);
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

void Courtroom::on_ooc_return_pressed(QString name, QString message)
{
  if (message.isEmpty())
    return;

  if (message.startsWith("/pos"))
  {
    ui_room_controls->toggleJudgeControls(message.startsWith("/pos jud"));
  }
  else if (message.startsWith("/login"))
  {
    ui_guard->show();
    append_server_chatmessage("CLIENT", "You were granted the Guard button.", "1");
  }
  else if (message.startsWith("/settings"))
  {
    ao_app->call_settings_menu();
    append_server_chatmessage("CLIENT", "You opened the settings menu.", "1");
    return;
  }
  else if (message.startsWith("/enable_blocks"))
  {
    append_server_chatmessage("CLIENT", "You have forcefully enabled features that the server "
                                        "may not support. You may not be able to talk IC, or worse, because of this.", "1");
    ao_app->cccc_ic_support_enabled = true;
    ao_app->arup_enabled = true;
    ao_app->modcall_reason_enabled = true;
    on_reload_theme_clicked();
    return;
  }
  else if (message.startsWith("/save_chatlog"))
  {
    QFile file("chatlog.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      append_server_chatmessage("CLIENT", "Couldn't open chatlog.txt to write into.", "1");
      return;
    }

    ui_ic_chatlog->export_to_file(file);

    append_server_chatmessage("CLIENT", "Transcript saved to chatlog.txt.", "1");
    return;
  }
  else if (message.startsWith("/load_case"))
  {
    QStringList command = message.split(" ", QString::SkipEmptyParts);

    QDir casefolder("base/cases");
    if (!casefolder.exists())
    {
        QDir::current().mkdir("base/" + casefolder.dirName());
        append_server_chatmessage("CLIENT", "You don't have a `base/cases/` folder! It was just made "
                                            "for you, but seeing as it WAS just made for you, it's "
                                            "likely the case file you're looking for can't be found in there.", "1");
        return;
    }
    QStringList caseslist = casefolder.entryList();
    caseslist.removeOne(".");
    caseslist.removeOne("..");
    caseslist.replaceInStrings(".ini","");

    if (command.size() < 2)
    {
      append_server_chatmessage("CLIENT", "You need to give a filename to load (extension not needed)! "
                                          "Make sure that it is in the `base/cases/` folder, and that "
                                          "it is a correctly formatted ini.\nCases you can load: " + caseslist.join(", "), "1");
      return;
    }


    if (command.size() > 2)
    {
      append_server_chatmessage("CLIENT", "Too many arguments to load a case! You only need one filename, "
                                          "without extension.", "1");
      return;
    }

    QSettings casefile("base/cases/" + command[1] + ".ini", QSettings::IniFormat);

    QString caseauth = casefile.value("author", "").value<QString>();
    QString casedoc = casefile.value("doc", "").value<QString>();
    QString cmdoc = casefile.value("cmdoc", "").value<QString>();
    QString casestatus = casefile.value("status", "").value<QString>();

    if (!caseauth.isEmpty())
      append_server_chatmessage("CLIENT", "Case made by " + caseauth + ".", "1");
    if (!casedoc.isEmpty())
      ao_app->send_server_packet(new AOPacket("CT#" + name + "#/doc " + casedoc + "#%"));
    if (!casestatus.isEmpty())
      ao_app->send_server_packet(new AOPacket("CT#" + name + "#/status " + casestatus + "#%"));
    if (!cmdoc.isEmpty())
      append_server_chatmessage("CLIENT", "Navigate to " + cmdoc + " for the CM doc.", "1");

    for (int i = local_evidence_list.size() - 1; i >= 0; i--)
    {
        ao_app->send_server_packet(new AOPacket("DE#" + QString::number(i) + "#%"));
    }

    for (QString &evi : casefile.childGroups()) {
        if (evi == "General")
          continue;

        QStringList f_contents;

        f_contents.append(casefile.value(evi + "/name", "UNKNOWN").value<QString>());
        f_contents.append(casefile.value(evi + "/description", "UNKNOWN").value<QString>());
        f_contents.append(casefile.value(evi + "/image", "UNKNOWN.png").value<QString>());

        ao_app->send_server_packet(new AOPacket("PE", f_contents));
      }

    append_server_chatmessage("CLIENT", "Your case \"" + command[1] + "\" was loaded!", "1");
    return;
  }

  QStringList packet_contents;
  packet_contents.append(name);
  packet_contents.append(message);

  AOPacket *f_packet = new AOPacket("CT", packet_contents);

  ao_app->send_server_packet(f_packet);
}

void Courtroom::on_ms_return_pressed(QString name, QString message)
{
  QStringList packet_contents({name, message});
  AOPacket *f_packet = new AOPacket("CT", packet_contents);

  ao_app->send_ms_packet(f_packet);
}

void Courtroom::on_icChat_positionChanged(QString pos)
{
  ao_app->send_server_packet(new AOPacket("CT#" + ui_server_chat->name() + "#/pos "
                                          + pos + "#%"));
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

  if (f_char.endsWith(" [x]"))
  {
    real_char = f_char.left(f_char.size() - 4);
    f_item->setText(real_char);
  }
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

  other_charid = f_cid;

  // Redo the character list.
  QStringList sorted_pair_list;

  for (char_type i_char : char_list)
    sorted_pair_list.append(i_char.name);

  sorted_pair_list.sort();

  for (int i = 0; i < ui_pair_list->count(); i++) {
    ui_pair_list->item(i)->setText(sorted_pair_list.at(i));
  }

  f_item->setText(real_char + " [x]");
}

void Courtroom::on_jukebox_trackSelected(QString track)
{
  if (!ui_ic_chat_name->text().isEmpty() && ao_app->cccc_ic_support_enabled)
  {
    ao_app->send_server_packet(new AOPacket("MC#" + track + "#" + QString::number(m_cid) + "#" + ui_ic_chat_name->text() + "#%"), false);
  }
  else
  {
    ao_app->send_server_packet(new AOPacket("MC#" + track + "#" + QString::number(m_cid) + "#%"), false);
  }
}

void Courtroom::on_rooms_roomSelected(QString room)
{
    ao_app->send_server_packet(new AOPacket("MC#" + room + "#" + QString::number(m_cid) + "#%"), false);
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

void Courtroom::on_pair_offset_changed(int value)
{
  offset_with_pair = value;
}

void Courtroom::on_roomControls_wtce(WTCE_TYPE type)
{
  const std::map<WTCE_TYPE, QString> packets = {
    {WITNESS_TESTIMONY, "testimony1"},
    {CROSS_EXAMINATION, "testimony2"},
    {NOT_GUILTY, "judgeruling#0"},
    {GUILTY, "judgeruling#1"}
  };

  try {
    ao_app->send_server_packet(new AOPacket("RT#" + packets.at(type) + "#%"));
  } catch (std::out_of_range) {
    qWarning() << "out-of-range WTCE_TYPE" << type;
  }
}

void Courtroom::on_change_character_clicked()
{
  music_player->set_volume(0);

  set_char_select();

  ui_char_select_background->show();
  ui_spectator->hide();
}

void Courtroom::on_reload_theme_clicked()
{ 
  ao_app->reload_theme();
  enter_courtroom(m_cid);
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

  // TODO: ensure that when the spectator button is clicked, the set character
  // signal is emitted and the IC widget empties the emote list
  // ui_emotes->hide();

  ui_char_select_background->hide();
}

void Courtroom::on_call_mod_clicked()
{
  if (ao_app->modcall_reason_enabled) {
    QMessageBox errorBox;
    QInputDialog input;

    input.setWindowFlags(Qt::WindowSystemMenuHint);
    input.setLabelText("Reason:");
    input.setWindowTitle("Call Moderator");
    auto code = input.exec();

    if (code != QDialog::Accepted)
      return;

    QString text = input.textValue();
    if (text.isEmpty()) {
      errorBox.critical(nullptr, "Error", "You must provide a reason.");
      return;
    } else if (text.length() > 256) {
      errorBox.critical(nullptr, "Error", "The message is too long.");
      return;
    }

    QStringList mod_reason;
    mod_reason.append(text);

    ao_app->send_server_packet(new AOPacket("ZZ", mod_reason));
  } else {
    ao_app->send_server_packet(new AOPacket("ZZ#%"));
  }
}

void Courtroom::on_settings_clicked()
{
  ao_app->call_settings_menu();
}

void Courtroom::on_announce_casing_clicked()
{
  ao_app->call_announce_menu(this);
}

void Courtroom::on_showname_enable_clicked()
{
  ui_ic_chatlog->reload();
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
}

#if (defined (_WIN32) || defined (_WIN64))
void Courtroom::load_bass_opus_plugin()
{
  BASS_PluginLoad("bassopus.dll", 0);
}
#elif (defined (LINUX) || defined (__linux__))
void Courtroom::load_bass_opus_plugin()
{
  BASS_PluginLoad("libbassopus.so", 0);
}
#else
#error This operating system is unsupported for bass plugins.
#endif
