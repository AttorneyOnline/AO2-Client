#include "aouiloader.h"
#include "widgets/aoviewport.h"
#include "widgets/courtroom.h"

#include <QLayout>
#include <QMenu>
#include <aocharselect.h>

#include <widgets/aocaseannouncerdialog.h>
#include <widgets/aomutedialog.h>
#include <widgets/aooptionsdialog.h>

Courtroom::Courtroom(AOApplication *ao_app, std::shared_ptr<Client> client)
  : QMainWindow(), ao_app(ao_app), client(client)
{
  initBASS();

  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/courtroom.ui");
  uiFile.open(QFile::ReadOnly);
  QMainWindow *windowWidget = static_cast<QMainWindow *>(loader.load(&uiFile, this));
  QMetaObject::connectSlotsByName(this);

  windowWidget->setWindowFlag(Qt::Window, false);
  windowWidget->setWindowFlag(Qt::Widget);
  windowWidget->centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
  windowWidget->show();

  setWindowTitle("Attorney Online 2");
  setWindowIcon(QIcon(":/logo.png"));

  FROM_UI(AOViewport, viewport)
  FROM_UI(AOChat, ic_chat)

  FROM_UI(QAction, change_character)
  FROM_UI(QAction, reload_theme)
  FROM_UI(QAction, call_mod)
  FROM_UI(QAction, open_settings)
  FROM_UI(QAction, announce_case)
  FROM_UI(QAction, modcall_notify)
  FROM_UI(QAction, casing)
  FROM_UI(QAction, showname_enable)

  FROM_UI(QMenu, window_menu)

  // Connect each widget to its toggle button. If the widget was not instantiated
  // in the .ui file, then instantiate it now. Even if a widget is hidden,
  // it will not be destroyed, so connections will not be invalidated.
  REGISTER_WINDOW(AOICLog, ic_chatlog, toggle_ic_log, ao_app)
  REGISTER_WINDOW(AOServerChat, ms_chat, toggle_ms_chat, ao_app)
  REGISTER_WINDOW(AOServerChat, server_chat, toggle_server_chat, ao_app)
  REGISTER_WINDOW(AOJukebox, music_list, toggle_jukebox, ao_app)
  REGISTER_WINDOW(AORoomChooser, room_chooser, toggle_room_chooser, ao_app)
  REGISTER_WINDOW(AOMixer, mixer, toggle_mixer, ao_app)
  REGISTER_WINDOW(AORoomControls, room_controls, toggle_room_controls, ao_app)
  REGISTER_WINDOW(AOEvidence, evidence, toggle_evidence, ao_app)

  music_player = new AOMusicPlayer(this, ao_app);
  music_player->set_volume(options.defaultMusicVolume());

  modcall_player = new AOSfxPlayer(this, ao_app);
  modcall_player->set_volume(50);

  ui_viewport->set_sfx_volume(options.defaultSfxVolume());
  ui_viewport->set_blip_volume(options.defaultBlipVolume());

  ui_casing->setChecked(options.casingEnabled());
  ui_showname_enable->setChecked(options.shownamesEnabled());

  connect(ui_server_chat, &AOServerChat::messageSent, this, &Courtroom::on_ooc_return_pressed);
  connect(ui_ms_chat, &AOServerChat::messageSent, this, &Courtroom::on_ms_return_pressed);

  // TODO: remove the autoconnect name of these slots - they don't autoconnect
  // due to the nature of the shared_ptr
  connect(client.get(), &Client::icReceived, this, &Courtroom::on_client_icReceived);
  connect(client.get(), &Client::connectionLost, this, &Courtroom::on_client_disconnected);
  connect(client.get(), &Client::trackChanged, this, &Courtroom::on_client_trackChanged);
  connect(client.get(), &Client::modCalled, this, &Courtroom::on_client_modCalled);
  connect(client.get(), &Client::caseCalled, this, &Courtroom::on_client_caseCalled);

  connect(client.get(), &Client::oocReceived, this,
          [&](const QString &name, const QString &message) {
    append_server_chatmessage(name, message, false);
  });
  connect(client.get(), &Client::characterChanged, this, &Courtroom::resetCourtroom);
  connect(client.get(), &Client::backgroundChanged, ui_viewport, &AOViewport::set_background);
  connect(client.get(), &Client::wtceReceived, ui_viewport, &AOViewport::wtce);
  connect(client.get(), &Client::healthChanged, ui_room_controls, &AORoomControls::setHealth);
  connect(client.get(), &Client::areasUpdated, this, [&] {
    ui_room_chooser->setAreas(this->client->rooms());
  });
  connect(client.get(), &Client::evidenceChanged, this, [&] {
    ui_evidence->setEvidenceList(this->client->evidence());
  });

  ui_room_chooser->setAreas(client->rooms());
  ui_music_list->setTracks(client->tracks().toVector());
}

void Courtroom::chooseCharacter()
{
  auto charSelect = new AOCharSelect(this, ao_app);

  charSelect->setCharacters(client->characters());

  connect(client.get(), &Client::takenCharsChanged, charSelect, [this, charSelect] {
    charSelect->setCharacters(client->characters());
  });

  connect(charSelect, &AOCharSelect::characterSelected, this, [&](int charId) {
    client->setCharacter(charId);
  });

  connect(charSelect, &QDialog::rejected, this, [&] {
    if (quitOnCancel)
      on_quit_triggered();
  });

  connect(client.get(), &Client::characterChanged,
          charSelect, &AOCharSelect::responseReceived);

  connect(charSelect, &QDialog::accepted, this, &Courtroom::resetCourtroom);

  charSelect->open();
}

void Courtroom::resetCourtroom()
{
  quitOnCancel = false;

  const char_type character = client->character();
  QString f_char = character.name;

  if (client->spectating())
  {
    if (options.discordEnabled())
      ao_app->discord->state_spectate();
  }
  else
  {
    f_char = ao_app->get_char_name(character.name);

    if (options.discordEnabled())
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

void Courtroom::append_server_chatmessage(QString p_name, QString p_message, bool special)
{
  QString color = "#000000";

  if (special)
    color = ao_app->get_color("ooc_server_color", "courtroom_design.ini").name();
  else
    color = ao_app->get_color("ooc_default_color", "courtroom_design.ini").name();

  ui_server_chat->append_chat_message(p_name, p_message, color);
}

void Courtroom::on_ic_chat_messageSent()
{
  if (ui_viewport->is_busy() && !ui_ic_chat->interjectionSelected())
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
  QStringList call_words = options.callWords();

  for (QString word : call_words)
  {
    if (f_message.contains(word, Qt::CaseInsensitive))
    {
      qDebug() << "Word call triggered:" << word;
      modcall_player->play(ao_app->get_sfx("word_call"));
      ao_app->alert(this);
      break;
    }
  }

  ui_viewport->chat(message, f_showname, evidence_image);
}

void Courtroom::on_client_disconnected(DisconnectReason code, const QString &message)
{
  const QString reasonText = QMap<DisconnectReason, QString> {
    {CONNECTION_RESET, tr("Connection to the server was lost.")},
    {KICKED, tr("You were kicked.")},
    {BANNED, tr("You were banned.")}
  }[code];

  if (!message.isEmpty()) {
    call_notice(tr("%1\nMessage: %2").arg(reasonText).arg(message));
  } else {
    call_notice(tr("%1").arg(reasonText));
  }

  ao_app->openLobby();
  deleteLater();
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
    if ((options.casingFlags() & flags).any())
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
    AOOptionsDialog(nullptr, ao_app).exec();
    return;
  }
  else if (message.startsWith("/save_chatlog"))
  {
    QFile file("chatlog.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      append_server_chatmessage("CLIENT", "Couldn't open chatlog.txt to write into.", true);
      return;
    }

    ui_ic_chatlog->export_to_file(file);

    append_server_chatmessage("CLIENT", "Transcript saved to chatlog.txt.", true);
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
                                          "likely the case file you're looking for can't be found in there.", true);
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
                                          "it is a correctly formatted ini.\nCases you can load: " + caseslist.join(", "), true);
      return;
    }


    if (command.size() > 2)
    {
      append_server_chatmessage("CLIENT", "Too many arguments to load a case! You only need one filename, "
                                          "without extension.", true);
      return;
    }

    QSettings casefile("base/cases/" + command[1] + ".ini", QSettings::IniFormat);

    QString caseauth = casefile.value("author", "").value<QString>();
    QString casedoc = casefile.value("doc", "").value<QString>();
    QString cmdoc = casefile.value("cmdoc", "").value<QString>();
    QString casestatus = casefile.value("status", "").value<QString>();

    if (!caseauth.isEmpty())
      append_server_chatmessage("CLIENT", "Case made by " + caseauth + ".", true);
    if (!casedoc.isEmpty())
      client->sendOOC(name, QStringLiteral("/doc %1").arg(casedoc));
    if (!casestatus.isEmpty())
      client->sendOOC(name, QStringLiteral("/status %1").arg(casestatus));
    if (!cmdoc.isEmpty())
      append_server_chatmessage("CLIENT", "Navigate to " + cmdoc + " for the CM doc.", true);

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

    append_server_chatmessage("CLIENT", "Your case \"" + command[1] + "\" was loaded!", true);
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

void Courtroom::on_mixer_volumeChanged(AUDIO_TYPE type, int volume)
{
  switch (type)
  {
  case SFX:
    ui_viewport->set_sfx_volume(volume);
    modcall_player->set_volume(volume);
    break;
  case BLIPS:
    ui_viewport->set_blip_volume(volume);
    break;
  case MUSIC:
    music_player->set_volume(volume);
    break;
  }
}

void Courtroom::on_room_controls_requestHealthChange(HEALTH_TYPE type, int value)
{
  client->sendHealth(type, value);
}

void Courtroom::on_room_controls_wtce(WTCE_TYPE type)
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
  disconnect(client.get(), &Client::connectionLost,
             this, &Courtroom::on_client_disconnected);

  ao_app->openLobby();
  deleteLater();
}

void Courtroom::on_call_mod_triggered()
{
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
}

void Courtroom::on_open_settings_triggered()
{
  AOOptionsDialog(nullptr, ao_app).exec();
}

void Courtroom::on_announce_case_triggered()
{
  AOCaseAnnouncerDialog(nullptr, ao_app, this).exec();
}

void Courtroom::on_showname_enable_triggered()
{
  ui_ic_chatlog->reload();
}

void Courtroom::on_casing_triggered()
{
  // TODO: remove button
  QMessageBox::warning(this, "Casing Filter", "This button is obsolete.");
}

void Courtroom::announce_case(QString title, bool def, bool pro, bool jud, bool jur, bool steno)
{
  // TODO: replace with direct call by announcer dialog to announceCase
  client->announceCase(title, casing_flags_to_bitset(def, pro, jud, jur, steno, false));
}

Courtroom::~Courtroom()
{
  delete music_player;
}

void Courtroom::initBASS()
{
  BASS_DEVICEINFO info;

  if (options.audioDevice() == "default")
  {
    BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
  }
  else
  {
    for (DWORD i = 0; BASS_GetDeviceInfo(i, &info); i++)
    {
      if (options.audioDevice() == info.name)
      {
        BASS_SetDevice(i);
        BASS_Init(static_cast<int>(i), 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
        qDebug() << info.name << "was set as the default audio output device.";
        break;
      }
    }
  }

  load_bass_opus_plugin();
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
