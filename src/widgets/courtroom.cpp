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
  connect(client, &Client::characterChanged, this, &Courtroom::enter_courtroom);
  connect(client, &Client::backgroundChanged, ui_viewport, &AOViewport::set_background);
  connect(client, &Client::wtceReceived, ui_viewport, &AOViewport::wtce);
  connect(client, &Client::healthChanged, ui_room_controls, &AORoomControls::setHealth);
  connect(client, &Client::areasUpdated, ui_area_list, &AORoomChooser::setAreas);
  connect(client, &Client::evidenceChanged, ui_evidence, &AOEvidence::setEvidenceList);

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

  client->sendIC(message).then([&] {
    objection_state = 0;
    realization_state = 0;
    ui_evidence->togglePresenting(false);
    ui_ic_chat->clearEntry();
  });
}

void Courtroom::on_client_icReceived(const chat_message_type &message)
{
  if (message.char_id < 0 || message.char_id >= client->characters().size() ||
      mute_map.value(message.char_id))
    return;

  QString f_showname;
  QString f_default_showname = ao_app->get_showname(client->characters()[message.char_id].name);
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

void Courtroom::on_client_trackChanged(const QString &track, const QString &showname)
{
  // TODO: ignore track change if the character who changed it is on the mute map

  if (!showname.isEmpty())
  {
    ui_ic_chatlog->append_ic_text(showname, showname, track, true);
  }

  music_player->play(track);
}

void Courtroom::on_client_modCalled(const QString &message)
{
  ui_server_chat->append_text(message);
  if (ui_modcall_notify->isChecked())
  {
    modcall_player->play(ao_app->get_sfx("mod_call"));
    ao_app->alert(this);
  }
}

void Courtroom::on_client_caseCalled(const QString &message,
                                     std::bitset<CASING_FLAGS_COUNT> flags)
{
  if (ui_casing->isChecked())
  {
    ui_server_chat->append_text(message);
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
      client->sendOOC(name, QStringLiteral("/doc %1").arg(casedoc));
    if (!casestatus.isEmpty())
      client->sendOOC(name, QStringLiteral("/status %1").arg(casestatus));
    if (!cmdoc.isEmpty())
      append_server_chatmessage("CLIENT", "Navigate to " + cmdoc + " for the CM doc.", "1");

    for (int i = client->evidence().size() - 1; i >= 0; i--)
    {
        client->removeEvidence(i);
    }

    for (QString &evi : casefile.childGroups()) {
        if (evi == "General")
          continue;

        evi_type evidence;
        evidence.name = casefile.value(evi + "/name", "UNKNOWN")
            .value<QString>();
        evidence.description = casefile.value(evi + "/description", "UNKNOWN")
            .value<QString>();
        evidence.image = casefile.value(evi + "/image", "UNKNOWN.png")
            .value<QString>();

        client->addEvidence(evidence);
      }

    append_server_chatmessage("CLIENT", "Your case \"" + command[1] + "\" was loaded!", "1");
    return;
  }

  client->sendOOC(name, message);
}

void Courtroom::on_ms_return_pressed(QString name, QString message)
{
  client->sendOOC(name, message);
}

void Courtroom::on_icChat_positionChanged(QString pos)
{
  client->sendOOC(ui_server_chat->name(), QStringLiteral("/pos %1").arg(pos));
}

void Courtroom::on_jukebox_trackSelected(QString track)
{
  client->playTrack(track, ui_ic_chat->showname());
}

void Courtroom::on_rooms_roomSelected(QString room)
{
  client->joinRoom(room);
}

void Courtroom::on_mute_triggered()
{
  auto dialog = new AOMuteDialog(this);
  connect(dialog, &AOMuteDialog::mute, this, [&](int mute_cid, bool muted) {
    mute_map.insert(mute_cid, muted);
  });

  QVector<QString> char_names;
  for (const char_type &character : client->characters())
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
  client->sendHealth(type, value);
}

void Courtroom::on_roomControls_wtce(WTCE_TYPE type)
{
  client->sendWTCE(type);
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

    client->callMod(text);
  } else {
    client->callMod();
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

void Courtroom::on_casing_triggered()
{
  if (!ao_app->casing_alerts_enabled)
    return;

  QMessageBox::warning(this, "Casing Filter", "This button is obsolete.");
}

void Courtroom::announce_case(QString title, bool def, bool pro, bool jud, bool jur, bool steno)
{
  if (!ao_app->casing_alerts_enabled)
    return;

  client->announceCase(title, casing_flags_to_bitset(def, pro, jud, jur, steno, false));
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
