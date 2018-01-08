#ifndef DISCORD_RICH_PRESENCE_H
#define DISCORD_RICH_PRESENCE_H

#include <discord-rpc.h>

namespace AttorneyOnline {

class Discord
{
private:
  const char* APPLICATION_ID = "399779271737868288";
public:
  Discord();
  ~Discord();

  void state_lobby();
  void state_server(const char* name, const char* server_id);
};

}
#endif // DISCORD_RICH_PRESENCE_H
