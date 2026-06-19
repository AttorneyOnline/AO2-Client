#include "discord_rich_presence.h"

#ifdef AO_ENABLE_DISCORD_RPC
#include <discord_rpc.h>

#include <QCoreApplication>
#include <QLibrary>
#include <QStringList>
#endif

namespace AttorneyOnline
{
#if defined(AO_ENABLE_DISCORD_RPC) && !defined(ANDROID)
namespace
{
// Entry points resolved from the discord-rpc shared library at runtime. They
// stay null while the library is missing, which keeps every Discord call a safe
// no-op instead of a hard dependency that stops the client from booting.
decltype(&::Discord_Initialize) p_Discord_Initialize = nullptr;
decltype(&::Discord_Shutdown) p_Discord_Shutdown = nullptr;
decltype(&::Discord_UpdatePresence) p_Discord_UpdatePresence = nullptr;

// Loads discord-rpc and resolves the symbols we use. Returns true only when the
// library is present and exposes all of them; otherwise Discord Rich Presence is
// disabled and the client carries on as normal.
bool load_discord_library(QLibrary &library)
{
  const QString base = QCoreApplication::applicationDirPath();
  // discord-rpc ships next to the executable on Windows and in the Linux
  // tarball, while the AppImage keeps it one level up in lib/. QLibrary adds the
  // platform prefix/suffix (lib..., .dll/.so/.dylib) to each candidate, and the
  // bare name falls back to the system search paths. Listing explicit paths is
  // necessary because dlopen ignores the runpath patchelf sets on the binary.
  const QStringList candidates = {base + "/discord-rpc", base + "/../lib/discord-rpc", base + "/lib/discord-rpc", QStringLiteral("discord-rpc")};

  for (const QString &candidate : candidates)
  {
    library.setFileName(candidate);
    if (library.load())
    {
      break;
    }
  }

  if (!library.isLoaded())
  {
    qInfo() << "Discord RPC library not found; Discord Rich Presence disabled." << library.errorString();
    return false;
  }

  p_Discord_Initialize = reinterpret_cast<decltype(&::Discord_Initialize)>(library.resolve("Discord_Initialize"));
  p_Discord_Shutdown = reinterpret_cast<decltype(&::Discord_Shutdown)>(library.resolve("Discord_Shutdown"));
  p_Discord_UpdatePresence = reinterpret_cast<decltype(&::Discord_UpdatePresence)>(library.resolve("Discord_UpdatePresence"));

  if (!p_Discord_Initialize || !p_Discord_Shutdown || !p_Discord_UpdatePresence)
  {
    qWarning() << "Discord RPC library is missing expected symbols; Discord Rich Presence disabled.";
    p_Discord_Initialize = nullptr;
    p_Discord_Shutdown = nullptr;
    p_Discord_UpdatePresence = nullptr;
    return false;
  }

  return true;
}

// Loads the library on first use. The QLibrary is kept for the program's
// lifetime so the module and the resolved pointers stay valid until exit.
bool ensure_discord_loaded()
{
  static QLibrary library;
  static const bool loaded = load_discord_library(library);
  return loaded;
}
} // namespace

Discord::Discord()
{
  if (!ensure_discord_loaded())
  {
    return;
  }

  DiscordEventHandlers handlers;
  std::memset(&handlers, 0, sizeof(handlers));
  handlers = {};
  handlers.ready = [](const DiscordUser *user) {
    qInfo() << "Discord RPC ready";
  };
  handlers.disconnected = [](int errorCode, const char *message) {
    qInfo() << "Discord RPC disconnected! " << message << errorCode;
  };
  handlers.errored = [](int errorCode, const char *message) {
    qWarning() << "Discord RPC errored out! " << message << errorCode;
  };
  qInfo() << "Initializing Discord RPC";
  p_Discord_Initialize(APPLICATION_ID, &handlers, 1, nullptr);
}

Discord::~Discord()
{
  if (!ensure_discord_loaded())
  {
    return;
  }
  p_Discord_Shutdown();
}

void Discord::state_lobby()
{
  if (!ensure_discord_loaded())
  {
    return;
  }

  DiscordRichPresence presence;
  std::memset(&presence, 0, sizeof(presence));
  presence.largeImageKey = "ao2-logo";
  presence.largeImageText = "Objection!";
  presence.instance = 1;

  presence.state = "In Lobby";
  presence.details = "Idle";
  p_Discord_UpdatePresence(&presence);
}

void Discord::state_server(std::string name, std::string server_id)
{
  if (!ensure_discord_loaded())
  {
    return;
  }

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
  p_Discord_UpdatePresence(&presence);
}

void Discord::state_character(std::string name)
{
  if (!ensure_discord_loaded())
  {
    return;
  }

  auto name_internal = QString(name.c_str()).toLower().replace(' ', '_').toStdString();
  auto name_friendly = QString(name.c_str()).replace('_', ' ').toStdString();
  const std::string playing_as = "Playing as " + name_friendly;
  qDebug().nospace() << "Discord RPC: Setting character state (\"" << playing_as.c_str() << "\")";

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
  p_Discord_UpdatePresence(&presence);
}

void Discord::state_spectate()
{
  if (!ensure_discord_loaded())
  {
    return;
  }

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
  p_Discord_UpdatePresence(&presence);
}
#else
Discord::Discord()
{}

Discord::~Discord()
{}

void Discord::state_lobby()
{}

void Discord::state_server(std::string name, std::string server_id)
{
  Q_UNUSED(name);
  Q_UNUSED(server_id);
  qDebug() << "Discord RPC: Setting server state";
}

void Discord::state_character(std::string name)
{
  Q_UNUSED(name);
  qDebug() << "Discord RPC: Setting character state";
}

void Discord::state_spectate()
{
  qDebug() << "Discord RPC: Setting specator state";
}
#endif
} // namespace AttorneyOnline
