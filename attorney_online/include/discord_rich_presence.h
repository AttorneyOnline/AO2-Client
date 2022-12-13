#ifndef DISCORD_RICH_PRESENCE_H
#define DISCORD_RICH_PRESENCE_H

#include <QCoreApplication>
#include <QDebug>
#include <cstring>
#include <ctime>
#include <string>

#if defined(DISCORD) && !defined(ANDROID)
#include <discord_rpc.h>
#endif

namespace AttorneyOnline {

class Discord {
  Q_DECLARE_TR_FUNCTIONS(Discord)

private:
  const char *APPLICATION_ID = "399779271737868288";
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

} // namespace AttorneyOnline
#endif // DISCORD_RICH_PRESENCE_H
