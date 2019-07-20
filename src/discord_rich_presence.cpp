#include "discord_rich_presence.h"

namespace AttorneyOnline {

#ifdef DISCORD
Discord::Discord()
{
  DiscordEventHandlers handlers;
  std::memset(&handlers, 0, sizeof(handlers));
  handlers = {};
  handlers.ready = [] {
    qInfo() << "Discord RPC ready";
  };
  handlers.disconnected = [](int errorCode, const char* message) {
    qInfo() << "Discord RPC disconnected! " << message << errorCode;
  };
  handlers.errored = [](int errorCode, const char* message) {
    qWarning() << "Discord RPC errored out! " << message << errorCode;
  };
  qInfo() << "Initializing Discord RPC";
  Discord_Initialize(APPLICATION_ID, &handlers, 1, nullptr);
}

Discord::~Discord()
{
  Discord_Shutdown();
}

void Discord::state_lobby()
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

void Discord::state_server(std::string name, std::string server_id)
{
  qDebug() << "Discord RPC: Setting server state";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;

  auto timestamp = static_cast<int64_t>(std::time(nullptr));

  presence.state = "In a Server";
  presence.details = name.c_str();
  presence.matchSecret = server_id.c_str();
  presence.startTimestamp = this->timestamp;

  this->server_id = server_id;
  this->server_name = name;
  this->timestamp = timestamp;
  Discord_UpdatePresence(&presence);
}

void Discord::state_character(std::string name)
{
  auto name_internal = QString(name.c_str()).toLower().replace(' ', '_').toStdString();
  auto name_friendly = QString(name.c_str()).replace('_', ' ').toStdString();
  const std::string playing_as = "Playing as " + name_friendly;
  qDebug() << "Discord RPC: Setting character state (" << playing_as.c_str() << ")";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;
  presence.details = this->server_name.c_str();
  presence.matchSecret = this->server_id.c_str();
  presence.startTimestamp = this->timestamp;

  presence.state = playing_as.c_str();
  presence.smallImageKey = name_internal.c_str();
  // presence.smallImageText = name_internal.c_str();
  Discord_UpdatePresence(&presence);
}

void Discord::state_spectate()
{
  qDebug() << "Discord RPC: Setting specator state";

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;
  presence.details = this->server_name.c_str();
  presence.matchSecret = this->server_id.c_str();
  presence.startTimestamp = this->timestamp;

  presence.state = "Spectating";
  Discord_UpdatePresence(&presence);
}
#else
Discord::Discord()
{

}

Discord::~Discord()
{

}

void Discord::state_lobby()
{

}

void Discord::state_server(std::string name, std::string server_id)
{
  qDebug() << "Discord RPC: Setting server state";
}

void Discord::state_character(std::string name)
{
  qDebug() << "Discord RPC: Setting character state";
}

void Discord::state_spectate()
{
  qDebug() << "Discord RPC: Setting specator state";

}
#endif
}
