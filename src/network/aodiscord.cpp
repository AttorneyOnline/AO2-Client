#include "include/network/aodiscord.hpp"
#include "external/discord_rpc.h"

#include <QDebug>
#include <QObject>
#include <cstring>

#if defined(DISCORD) && !defined(ANDROID)
AODiscord::AODiscord(QObject *parent) : Discord(parent)
{
  DiscordEventHandlers handlers;
  std::memset(&handlers, 0, sizeof(handlers));
  handlers.ready = [](const DiscordUser *connectedUser) {
    qInfo() << QString("Discord: connected to user %1#%2 - %3")
                   .arg(connectedUser->username, connectedUser->discriminator,
                        connectedUser->userId);
  };
  handlers.disconnected = [](int errorCode, const char *message) {
    qInfo() << "Discord RPC disconnected! " << message << errorCode;
  };
  handlers.errored = [](int errorCode, const char *message) {
    qWarning() << "Discord RPC errored out! " << message << errorCode;
  };
  qInfo() << "Initializing Discord RPC";
  Discord_Initialize(APPLICATION_ID, &handlers, 1, nullptr);
}

void AODiscord::state_lobby()
{
  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;

  presence.state = "In Lobby";
  presence.details = "Idle";
  Discord_UpdatePresence(&presence);
}

void AODiscord::state_server(QString server_name, QString server_id)
{
  qDebug() << "Discord RPC: Setting server state";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;

  auto l_timestamp = static_cast<int64_t>(std::time(nullptr));

  presence.state = "In a Server";
  presence.details = server_name.toStdString().c_str();
  presence.matchSecret = server_id.toStdString().c_str();
  presence.startTimestamp = m_timestamp;

  m_server_id = server_id;
  m_server_name = server_name;
  m_timestamp = l_timestamp;
  Discord_UpdatePresence(&presence);
}

void AODiscord::state_character(QString character_name)
{
  auto name_internal = QString(character_name.toStdString().c_str())
                           .toLower()
                           .replace(' ', '_')
                           .toStdString();
  auto name_friendly = QString(character_name.toStdString().c_str())
                           .replace('_', ' ')
                           .toStdString();
  const std::string playing_as = "Playing as " + name_friendly;
  qDebug().nospace() << "Discord RPC: Setting character state (\""
                     << playing_as.c_str() << "\")";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;
  presence.details = m_server_name.toStdString().c_str();
  presence.matchSecret = m_server_id.toStdString().c_str();
  presence.startTimestamp = m_timestamp;

  presence.state = playing_as.c_str();
  presence.smallImageKey = name_internal.c_str();
  Discord_UpdatePresence(&presence);
}

void AODiscord::state_spectate() {}
#else
void AODiscord::state_lobby()
{
  // NOOP
}

void AODiscord::state_server(QString server_name, QString server_id)
{
  Q_UNUSED(server_name);
  Q_UNUSED(server_id);
}

void AODiscord::state_character(QString character_name)
{
  Q_UNUSED(character_name);
}

void AODiscord::state_spectate()
{
  // NOOP
}

#endif
