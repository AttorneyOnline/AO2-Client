#include "aouiloader.h"
#include "widgets/aoviewport.h"
#include "widgets/courtroom.h"

#include <QLayout>
#include <aocharselect.h>

#include <widgets/aomutedialog.h>

Courtroom::Courtroom(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

  // Change the default audio output device to be the one the user has given
  // in his config.ini file for now.
  DWORD a = 0;
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
  FROM_UI(AOViewport, viewport)
  FROM_UI(AOICLog, ic_chatlog)
  FROM_UI(AOServerChat, ms_chat)
  FROM_UI(AOServerChat, server_chat)
  FROM_UI(AOJukebox, music_list)
  FROM_UI(AORoomChooser, area_list)
  FROM_UI(AOMixer, mixer)
  FROM_UI(AOChat, ic_chat)
  FROM_UI(AORoomControls, room_controls)

  music_player = new AOMusicPlayer(this, ao_app);
  music_player->set_volume(0);

  modcall_player = new AOSfxPlayer(this, ao_app);
  modcall_player->set_volume(50);

  ui_casing->setChecked(ao_app->get_casing_enabled());
  ui_showname_enable->setChecked(ao_app->get_showname_enabled_by_default());

  connect(ui_server_chat, SIGNAL(messageSent(QString, QString)), this, SLOT(on_ooc_return_pressed(QString, QString)));
  connect(ui_ms_chat, SIGNAL(messageSent(QString, QString)), this, SLOT(on_ms_return_pressed(QString, QString)));

  connect(client, &Client::oocReceived, this, &Courtroom::append_server_chatmessage);
  connect(client, &Client::icReceived, this, &Courtroom::handle_chatmessage);
  connect(client, &Client::trackChanged, this, &Courtroom::handle_song);
  connect(client, &Client::caseCalled, this, &Courtroom::case_called);
  connect(client, &Client::modCalled, this, &Courtroom::mod_called);
  connect(client, &Client::characterChanged, this, &Courtroom::enter_courtroom);

  connect(client, &Client::backgroundChanged, ui_viewport, &AOViewport::set_background);
  connect(client, &Client::wtceReceived, ui_viewport, &AOViewport::wtce);

  connect(client, &Client::healthChanged, ui_room_controls, &AORoomControls::setHealth);

  connect(client, &Client::areasUpdated, ui_area_list, &AORoomChooser::setAreas);

  connect(ui_mixer, &AOMixer::volumeChanged, this, &Courtroom::setVolume);
}

void Courtroom::done_received()
{
  music_player->set_volume(0);
  ui_viewport->stop_sounds();

  show();

  if (!chooseCharacter())
    on_quit_triggered();
}

bool Courtroom::chooseCharacter()
{
  AOCharSelect charSelect(this, ao_app);
  if (charSelect.exec() == QDialog::Accepted)
  {
    enter_courtroom(charSelect.selectedCharacterIndex());
    return true;
  }
  else
  {
    return false;
  }
}

void Courtroom::enter_courtroom(int p_cid)
{
  QString f_char;

  if (p_cid == -1)
  {
    f_char = "";

    if (ao_app->is_discord_enabled())
      ao_app->discord->state_spectate();
  }
  else
  {
    f_char = ao_app->get_char_name(client->character().name);

    if (ao_app->is_discord_enabled())
      ao_app->discord->state_character(f_char.toStdString());
  }

  ui_ic_chat->setCharacter(f_char);

  const QString side = ao_app->get_char_side(f_char);
  ui_room_controls->toggleJudgeControls(side == "jud");

  // TODO: ensure that when the spectator button is clicked, the set character
  // signal is emitted and the IC widget empties the emote list
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

void Courtroom::on_ic_chat_messageSent()
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

  chat_message_type message;
  ui_ic_chat->addMessageData(message);
  ui_evidence->addMessageData(message);

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
    QString::number(message.evidence),
    QString::number(message.flip),
    QString::number(message.realization),
    QString::number(message.text_color),
    message.showname,
    QString::number(message.pair_char_id),
    QString::number(message.pair_offset),
    QString::number(message.noninterrupting_preanim)
  };

  client->sendIC(message).then([&] {
    objection_state = 0;
    realization_state = 0;
    ui_evidence->togglePresenting(false);
    ui_ic_chat->clearEntry();
  });
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

  // Append to IC box
  ui_ic_chatlog->append_ic_text(f_default_showname, f_showname, ": " + message.message);

  // Send message to viewport
  previous_ic_message = f_message;

  // Parse evidence
  int f_evi_id = message.evidence;

  QString evidence_image;
  if (f_evi_id > 0 && f_evi_id <= client->evidence().size())
  {
    //shifted by 1 because 0 is no evidence per legacy standards
    evidence_image = client->evidence()[f_evi_id - 1].image;
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

void Courtroom::on_client_kicked(const QString &message, bool banned)
{
  const QString verb = banned ? tr("banned") : tr("kicked");
  if (!message.isEmpty()) {
    call_notice(tr("You were %1.\nMessage: %2").arg(verb).arg(message));
  } else {
    call_notice(tr("You were %1.").arg(verb));
  }

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

void Courtroom::mod_called(QString p_ip)
{
  ui_server_chat->append_text(p_ip);
  if (ui_modcall_notify->isChecked())
  {
    modcall_player->play(ao_app->get_sfx("mod_call"));
    ao_app->alert(this);
  }
}

void Courtroom::case_called(QString msg, std::bitset<CASING_FLAGS_COUNT> flags)
{
  if (ui_casing->isChecked())
  {
    ui_server_chat->append_text(msg);
    if ((ao_app->get_casing_flags() & flags).any())
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
    ui_modcall_notify->setEnabled(true);
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
    on_reload_theme_triggered();
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

void Courtroom::on_jukebox_trackSelected(QString track)
{
  const QString packetBody = QStringLiteral("MC#%1#%2#%3#%")
      .arg(track)
      .arg(QString::number(m_cid))
      .arg(ui_ic_chat->showname());
  ao_app->send_server_packet(new AOPacket(packetBody), false);
}

void Courtroom::on_rooms_roomSelected(QString room)
{
  const QString packetBody = QStringLiteral("MC#%1#%2#%")
      .arg(room)
      .arg(QString::number(m_cid));
  ao_app->send_server_packet(new AOPacket(packetBody), false);
}

void Courtroom::on_mute_triggered()
{
  auto dialog = new AOMuteDialog(this);
  connect(dialog, &AOMuteDialog::mute, this, [&](int mute_cid, bool muted) {
    mute_map.insert(mute_cid, muted);
  });

  QVector<QString> char_names;
  for (const char_type &character : char_list)
    char_names.push_back(character.name);

  dialog->setCharacters(char_names);
  dialog->show();

  // Dialog will be freed by Qt on dialog close
}

void Courtroom::on_pair_triggered()
{
  QMessageBox::warning(this, "Pairing", "Unfortunately, pairing was removed due "
                                        "to time constraints. It will return in "
                                        "the next version.");
}

void Courtroom::healthChangeRequested(HEALTH_TYPE type, int value)
{
  if (value > 10 || value < 0)
    return;

  QStringList params = {
    QString::number(type),
    QString::number(value)
  };

  ao_app->send_server_packet(new AOPacket("HP", params));
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

void Courtroom::on_change_character_triggered()
{
  music_player->set_volume(0);

  chooseCharacter();
}

/*
 * Reloading theme is not supported (yet). The reason is that it has
 * to be done recursively - in other words, the entire UI has to be
 * closed and reopened.
 */
void Courtroom::on_reload_theme_triggered()
{
  //ao_app->reload_theme();
  //enter_courtroom(m_cid);
  QMessageBox::warning(this, tr("Reload Theme"),
                       tr("Not supported yet - sorry!"));
}

void Courtroom::on_quit_triggered()
{
  ao_app->construct_lobby();
  ao_app->destruct_courtroom();
}

void Courtroom::on_call_mod_triggered()
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

void Courtroom::on_settings_triggered()
{
  ao_app->call_settings_menu();
}

void Courtroom::on_announce_case_triggered()
{
  ao_app->call_announce_menu(this);
}

void Courtroom::on_showname_enable_triggered()
{
  ui_ic_chatlog->reload();
}

void Courtroom::ping_server()
{
  ao_app->send_server_packet(new AOPacket("CH#" + QString::number(m_cid) + "#%"));
}

void Courtroom::on_casing_triggered()
{
  if (!ao_app->casing_alerts_enabled)
    return;

  if (ui_casing->isChecked())
  {
    QStringList f_packet = {
      ao_app->get_casing_can_host_cases(),
      QString::number(ao_app->get_casing_cm_enabled()),
      QString::number(ao_app->get_casing_defence_enabled()),
      QString::number(ao_app->get_casing_prosecution_enabled()),
      QString::number(ao_app->get_casing_judge_enabled()),
      QString::number(ao_app->get_casing_juror_enabled()),
      QString::number(ao_app->get_casing_steno_enabled())
    };

    ao_app->send_server_packet(new AOPacket("SETCASE", f_packet));
  }
  else
  {
    ao_app->send_server_packet(new AOPacket("SETCASE#\"\"#0#0#0#0#0#0#%"));
  }
}

void Courtroom::announce_case(QString title, bool def, bool pro, bool jud, bool jur, bool steno)
{
  if (!ao_app->casing_alerts_enabled)
    return;

  QStringList f_packet = {
    title,
    QString::number(def),
    QString::number(pro),
    QString::number(jud),
    QString::number(jur),
    QString::number(steno)
  };

  ao_app->send_server_packet(new AOPacket("CASEA", f_packet));
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
