#ifndef DISCORD_RICH_PRESENCE_H
#define DISCORD_RICH_PRESENCE_H

#include <cstring>
#include <ctime>
#include <QDebug>
#include <string>
#include <discord-rpc.h>

namespace AttorneyOnline {

class Discord
{
private:
  const char* APPLICATION_ID = "399779271737868288";
  std::string server_name, server_id;
  int64_t timestamp;
public:
  Discord();
  ~Discord();

  void state_lobby();
  void state_server(std::string name, std::string server_id);
  void state_character(std::string name);
  void state_spectate();
};

}
#endif // DISCORD_RICH_PRESENCE_H
