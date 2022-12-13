#ifndef DISCORD_RICH_PRESENCE_H
#define DISCORD_RICH_PRESENCE_H

#include <QCoreApplication>
#include <QDebug>
#include <cstring>
#include <ctime>
#include <string>

#include "discord.h"

namespace AttorneyOnline {

class AORPC {
  discord::Core *core;

private:
  const std::int64_t APPLICATION_ID = 399779271737868288;
  std::string server_name, server_id;
  int64_t timestamp;

public:
  AORPC();
  ~AORPC();

  void state_lobby();
  void state_server(std::string name, std::string server_id);
  void state_character(std::string name);
  void state_spectate();
};

} // namespace AttorneyOnline
#endif // DISCORD_RICH_PRESENCE_H
