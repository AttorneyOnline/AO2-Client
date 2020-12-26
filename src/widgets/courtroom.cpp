#include "aouiloader.h"
#include "widgets/aoviewport.h"
#include "widgets/courtroom.h"

#include <QLayout>
#include <QMenu>

#include "widgets/aocaseannouncerdialog.h"
#include "widgets/aocharselect.h"
#include "widgets/aomutedialog.h"
#include "widgets/aooptionsdialog.h"
#include "widgets/aopairdialog.h"
#include "network/legacyclient.h"
#include "widgetdumper.h"

Courtroom::Courtroom(AOApplication *ao_app, std::shared_ptr<Client> client)
  : QMainWindow(), ao_app(ao_app), client(client)
{
  initBASS();

  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/courtroom.ui");
  uiFile.open(QFile::ReadOnly);
  windowWidget = static_cast<QMainWindow *>(loader.load(&uiFile, this));

  windowWidget->setWindowFlag(Qt::Window, false);
  windowWidget->setWindowFlag(Qt::Widget);
  windowWidget->centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
  setContentsMargins(0, 0, 0, 0);

  windowWidget->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  windowWidget->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  windowWidget->show();

  setWindowTitle("Attorney Online 2");
  setWindowIcon(QIcon(":/logo.png"));
  setAttribute(Qt::WA_DeleteOnClose, true);

  FROM_UI(AOViewport, viewport)
  FROM_UI(AOChat, ic_chat)

  FROM_UI(QAction, mute)
  FROM_UI(QAction, pair)
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
  REGISTER_WINDOW(AOICLog, ic_chatlog)
  REGISTER_WINDOW(AOServerChat, ms_chat)
  REGISTER_WINDOW(AOServerChat, server_chat)
  REGISTER_WINDOW(AOJukebox, music_list)
  REGISTER_WINDOW(AORoomChooser, room_chooser)
  REGISTER_WINDOW(AOMixer, mixer)
  REGISTER_WINDOW(AORoomControls, room_controls)
  REGISTER_WINDOW(AOEvidence, evidence)

  ui_ms_chat->setWindowTitle(tr("Master Chat"));

  music_player = new AOMusicPlayer(this, ao_app);
  music_player->set_volume(options.defaultMusicVolume());

  modcall_player = new AOSfxPlayer(this, ao_app);
  modcall_player->set_volume(50);

  ui_viewport->set_sfx_volume(options.defaultSfxVolume());
  ui_viewport->set_blip_volume(options.defaultBlipVolume());

  ui_casing->setChecked(options.casingEnabled());
  ui_showname_enable->setChecked(options.shownamesEnabled());

  connect(ui_ic_chat, &AOChat::messageSent, this, &Courtroom::onICMessageSend);
  connect(ui_server_chat, &AOServerChat::messageSent, this, &Courtroom::onOOCSend);
  connect(ui_ms_chat, &AOServerChat::messageSent, this, &Courtroom::onGlobalChatSend);

  connect(client.get(), &Client::icReceived, this, &Courtroom::onICMessage);
  connect(client.get(), &Client::connectionLost, this, &Courtroom::onDisconnect);
  connect(client.get(), &Client::trackChanged, this, &Courtroom::onTrackChange);
  connect(client.get(), &Client::modCalled, this, &Courtroom::onModCall);
  connect(client.get(), &Client::caseCalled, this, &Courtroom::onCaseCall);

  connect(client.get(), &Client::oocReceived, this, &Courtroom::onOOCMessage);
  connect(client.get(), &Client::characterChanged, this, &Courtroom::resetCourtroom);
  connect(client.get(), &Client::backgroundChanged, ui_viewport, &AOViewport::set_background);
  connect(client.get(), &Client::wtceReceived, ui_viewport, &AOViewport::wtce);
  connect(client.get(), &Client::healthChanged, ui_room_controls, &AORoomControls::setHealth);
  connect(client.get(), &Client::areasUpdated, this, &Courtroom::onAreaUpdate);
  connect(client.get(), &Client::evidenceChanged, this, &Courtroom::onEvidenceUpdate);

  connect(ui_mixer, &AOMixer::volumeChanged, this, &Courtroom::onMixerVolumeChange);
  connect(ui_room_controls, &AORoomControls::requestHealthChange, this,
          &Courtroom::onRequestHealthChange);
  connect(ui_room_controls, &AORoomControls::wtce, this, &Courtroom::onRequestWTCE);
  connect(ui_evidence, &AOEvidence::evidenceAdded, client.get(), &Client::addEvidence);
  connect(ui_evidence, &AOEvidence::evidenceEdited, client.get(), &Client::editEvidence);
  connect(ui_evidence, &AOEvidence::evidenceDeleted, client.get(), &Client::removeEvidence);
  connect(ui_room_chooser, &AORoomChooser::roomSelected, client.get(), &Client::joinRoom);

  // Note that autoconnect will only work for courtroom widgets. It will not work
  // for windows or other miscellaneous children that the courtroom happens to own.
  QMetaObject::connectSlotsByName(this);

  ui_room_chooser->setAreas(client->rooms());
  ui_music_list->setTracks(client->tracks().toVector());
  ui_evidence->setEvidenceList(client->evidence());

  QFileInfo layoutAutosaveFile(ao_app->get_base_path() + LAYOUT_AUTOSAVE_FILE);
  QFileInfo layoutFile(ao_app->get_theme_path("layout.dat"));
  if (layoutAutosaveFile.exists())
    loadLayout(layoutAutosaveFile.filePath());
  else if (layoutFile.exists())
    loadLayout(layoutFile.filePath());

  auto *autosaveTimer = new QTimer(this);
  autosaveTimer->setInterval(LAYOUT_AUTOSAVE_INTERVAL);
  autosaveTimer->setTimerType(Qt::VeryCoarseTimer);
  connect(autosaveTimer, &QTimer::timeout, this, &Courtroom::autosaveLayout);
  autosaveTimer->start();
}

template<typename T>
void Courtroom::registerWindow(T *&widget, const QString &name,
                               QDockWidget *&dockWidget, QAction *&toggleAction)
{
  widget = findChild<T *>(name);
  dockWidget = findChild<QDockWidget *>(name + "_dock");
  toggleAction = findChild<QAction *>("toggle_" + name);
  if (widget == nullptr)
  {
    dockWidget = new QDockWidget(this);
    widget = new T(dockWidget, ao_app);
    widget->setObjectName(name);
    dockWidget->setObjectName(name + "_dock");
    dockWidget->setWidget(widget);
    dockWidget->setWindowTitle(widget->windowTitle());
    windowWidget->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
    dockWidget->setFloating(true);
    dockWidget->setVisible(false);
  }
  else
  {
    toggleAction->setChecked(true);
  }
  connect(toggleAction, &QAction::toggled, this, [=](bool toggled) {
    dockWidget->setVisible(toggled);
  });
  connect(ui_window_menu, &QMenu::aboutToShow, this, [=] {
    toggleAction->setChecked(dockWidget->isVisible());
  });

  windows.insert(dockWidget);
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
      close();
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

void Courtroom::onICMessageSend()
{
  if (ui_viewport->is_busy() && !ui_ic_chat->interjectionSelected())
    return;

  chat_message_type message;
  ui_ic_chat->addMessageData(message);
  ui_evidence->addMessageData(message);

  client->sendIC(message).then([&] {
    ui_evidence->togglePresenting(false);
    ui_ic_chat->clearEntry();
  });
}

void Courtroom::onICMessage(const chat_message_type &message)
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

void Courtroom::onDisconnect(DisconnectReason code, const QString &message)
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

void Courtroom::onTrackChange(const QString &track, const QString &showname)
{
  // TODO: ignore track change if the character who changed it is on the mute map

  if (!showname.isEmpty())
  {
    ui_ic_chatlog->append_ic_text(showname, showname, track, true);
  }

  music_player->play(track);
}

void Courtroom::onModCall(const QString &message)
{
  ui_server_chat->append_text(message);
  if (ui_modcall_notify->isChecked())
  {
    modcall_player->play(ao_app->get_sfx("mod_call"));
    ao_app->alert(this);
  }
}

void Courtroom::onCaseCall(const QString &message,
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

void Courtroom::onOOCMessage(const QString &name, const QString &message)
{
  append_server_chatmessage(name, message, false);
}

void Courtroom::onAreaUpdate()
{
  ui_room_chooser->setAreas(this->client->rooms());
}

void Courtroom::onEvidenceUpdate()
{
  ui_evidence->setEvidenceList(this->client->evidence());
}

void Courtroom::onOOCSend(QString name, QString message)
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
    QStringList command = message.split(" ", Qt::SkipEmptyParts);

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

void Courtroom::onGlobalChatSend(QString name, QString message)
{
  append_ms_chatmessage("Client", "Not implemented yet.");
}

void Courtroom::onICPositionChange(QString pos)
{
  client->sendOOC(ui_server_chat->name(), QStringLiteral("/pos %1").arg(pos));
}

void Courtroom::onJukeboxTrackSelect(QString track)
{
  client->playTrack(track, ui_ic_chat->showname());
}

void Courtroom::on_mute_triggered()
{
  auto dialog = new AOMuteDialog(this);
  connect(dialog, &AOMuteDialog::mute, this, [&](int mute_cid, bool muted) {
    mute_map.insert(mute_cid, muted);
  });

  auto makeMuteMap = [this, dialog] {
    QVector<QPair<QString, bool>> characters;
    for (const char_type &character : client->characters()) {
      characters.push_back({ character.name, false });
    }

    for (const auto &cid : mute_map.keys()) {
      if (mute_map[cid])
        characters[cid].second = true;
    }

    dialog->setMuteMap(characters);
  };

  connect(dialog, &AOMuteDialog::cleared, this, [=] {
    mute_map.clear();
    makeMuteMap();
  });

  makeMuteMap();
  dialog->setModal(true);
  dialog->show();

  // Dialog will be freed by Qt on dialog close
}

void Courtroom::on_pair_triggered()
{
  // Break the abstraction layer to access intricacies of AO pairing
  auto *legacyClient = dynamic_cast<LegacyClient *>(client.get());
  if (!legacyClient) {
    QMessageBox::warning(this, tr("Pairing"), tr("Pairing is not supported on this server."));
    return;
  }
  auto dialog = new AOPairDialog(this);
  connect(dialog, &AOPairDialog::pairChanged, legacyClient, &LegacyClient::setPair);
  connect(dialog, &AOPairDialog::offsetChanged, legacyClient, &LegacyClient::setPairOffset);
  dialog->setCharacters(client->characters());
  dialog->setCurrentPair(legacyClient->currentPair());
  dialog->setPairOffset(legacyClient->currentPairOffset());
  dialog->setModal(true);
  dialog->show();

  // Dialog will be freed by Qt on dialog close
}

void Courtroom::onMixerVolumeChange(AUDIO_TYPE type, int volume)
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

void Courtroom::onRequestHealthChange(HEALTH_TYPE type, int value)
{
  client->sendHealth(type, value);
}

void Courtroom::onRequestWTCE(WTCE_TYPE type)
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
  close();
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

void Courtroom::initBASS()
{
  BASS_DEVICEINFO info;

  if (options.audioDevice() == "default")
  {
    qDebug() << "Initializing BASS";
    BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
    qDebug() << "Initialized BASS";
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

void Courtroom::on_load_layout_triggered() {
  QString filename = QFileDialog::getOpenFileName(this, tr("Load Layout File"),
                                                  ao_app->get_base_path(),
                                                  tr("Layout Data (*.dat)"));
  QFileInfo fileInfo(filename);
  if (filename.isEmpty() || !fileInfo.exists())
    return;

  if (!loadLayout(filename))
    QMessageBox::warning(this, tr("Layout Error"),
                         tr("There was an error loading the layout file. It may be corrupted."));
}

bool Courtroom::loadLayout(const QString &filename) {
  QSettings file(filename, QSettings::IniFormat);

  file.beginGroup("windows");
  QStringList windowEntries = file.childKeys();
  for (const QString &entry : windowEntries) {
    auto child = findChild<QWidget *>(entry);
    if (!child)
      continue;
    child->setVisible(file.value(entry).toBool());
  }
  file.endGroup();

  if (file.status() != QSettings::NoError) {
    return false;
  }

  auto state = file.value("state").toByteArray();
  if (!windowWidget->restoreState(state)) {
    return false;
  }

  return true;
}

void Courtroom::on_save_layout_triggered() {
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Layout File"),
                                                  ao_app->get_base_path(),
                                                  tr("Layout Data (*.dat)"));
  if (filename.isEmpty())
    return;

  if (!saveLayout(filename))
    QMessageBox::warning(this, tr("Layout Error"),
                         tr("There was an error writing to the layout file."));
}

bool Courtroom::saveLayout(const QString &filename) {
  QSettings file(filename, QSettings::IniFormat);
  for (const QWidget *widget : windows) {
    file.setValue("windows/" + widget->objectName(), widget->isVisible());
  }
  file.setValue("state", windowWidget->saveState());
  file.sync();

  if (file.status() != QSettings::NoError) {
    return false;
  }

  return true;
}

void Courtroom::autosaveLayout() {
  saveLayout(ao_app->get_base_path() + LAYOUT_AUTOSAVE_FILE);
}

void Courtroom::closeEvent(QCloseEvent *event) {
  QWidget::closeEvent(event);
  if (!event->isAccepted())
    return;

  autosaveLayout();
  disconnect(client.get(), &Client::connectionLost,
             this, &Courtroom::onDisconnect);
  ao_app->openLobby();
}

#elif (defined (LINUX) || defined (__linux__))
void Courtroom::load_bass_opus_plugin()
{
  BASS_PluginLoad("libbassopus.so", 0);
}
#else
#error This operating system is unsupported for bass plugins.
#endif
