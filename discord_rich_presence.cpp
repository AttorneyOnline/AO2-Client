#include "discord_rich_presence.h"

#include <cstring>

#include <QDebug>

namespace AttorneyOnline {

Discord::Discord()
{
  DiscordEventHandlers handlers;
  std::memset(&handlers, 0, sizeof(handlers));
  handlers = {};
  handlers.ready = [] {
    qInfo() << "Discord RPC ready";
  };
  /*
  handlers.disconnected = [](int errorCode, const char* message) {
    qInfo() << "Discord RPC disconnected! " << message;
  };
  handlers.errored = [](int errorCode, const char* message) {
    qWarning() << "Discord RPC errored out! " << message;
  };
  */
  qInfo() << "Are things working out all right?";
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
  presence.state = "In Lobby";
  presence.details = "Idle";
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;
  Discord_UpdatePresence(&presence);
}

void Discord::state_server(const char* name, const char* server_id)
{
  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.state = "In a Server";
  presence.details = name;
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;
  presence.matchSecret = server_id;
  Discord_UpdatePresence(&presence);
}

}
