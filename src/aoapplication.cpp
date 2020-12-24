#include "aoapplication.h"

#include <QFile>
#include <QSettings>

#include <QDebug>
#include <QScreen>

#include <QStandardPaths>

#include <QStringList>

#include "lobby.h"
#include "widgets/courtroom.h"
#include "debug_functions.h"

#include "widgets/aooptionsdialog.h"
#include "widgets/aocaseannouncerdialog.h"

#include <network/legacyclient.h>
#include <network/legacyms.h>

using namespace AttorneyOnline;

AOApplication::AOApplication(int &argc, char **argv) : QApplication(argc, argv)
{
  // Create the QSettings class that points to the config.ini.
  configini = new QSettings(get_base_path() + "config.ini", QSettings::IniFormat);
  ms = std::make_shared<LegacyMasterServer>(this);
  discord = std::make_shared<Discord>();
}

void AOApplication::openLobby()
{
  auto lobby = new Lobby(this);
  lobby->setAttribute(Qt::WA_DeleteOnClose);

  connect(ms.get(), &MasterServer::serversChanged, lobby, &Lobby::showPublicServers);
  connect(ms.get(), &MasterServer::chatReceived, lobby, &Lobby::appendChat);
  connect(ms.get(), &MasterServer::connectionLost, lobby, [&] {
    lobby->appendError("Connection to master server lost. Retrying.");
    QtPromise::resolve().delay(MS_RETRY_MILLISECS).then([&] {
      connectToMaster();
    });
  });
  connect(lobby, &Lobby::chatSent, ms.get(), &MasterServer::sendChat);
  connect(lobby, &Lobby::msRefreshRequested, this, [&] {
    if (!ms->isConnected())
      this->connectToMaster();
    else
      ms->requestServerList();
  });

  // XXX: is this needed?
  QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
  int x = (screenGeometry.width() - lobby->width()) / 2;
  int y = (screenGeometry.height() - lobby->height()) / 2;
  lobby->move(x, y);

  if (options.discordEnabled())
    discord->state_lobby();

  lobby->show();
}

QString AOApplication::get_version_string()
{
  return QStringLiteral("%1.%2.%3")
      .arg(RELEASE)
      .arg(MAJOR_VERSION)
      .arg(MINOR_VERSION);
}

void AOApplication::reloadTheme()
{
  currentTheme = options.theme();
}

void AOApplication::connectToMaster()
{
  if (ms->isConnecting() || ms->isConnected())
    return;

  qInfo() << "Connecting to master server...";
  ms->connect(options.msAddress(), options.msPort()).then([&] {
    qInfo() << "Connected to master server.";
    msRetryCount = 0;
    ms->requestServerList();
  }).fail([&] {
    qWarning() << "Connection to master server failed. Retrying...";
    msRetryCount++;
    QtPromise::resolve().delay(MS_RETRY_MILLISECS * msRetryCount).then([&] {
      connectToMaster();
    });
  });
}
