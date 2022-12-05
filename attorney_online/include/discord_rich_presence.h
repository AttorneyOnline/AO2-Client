#ifndef DISCORD_RICH_PRESENCE_H
#define DISCORD_RICH_PRESENCE_H

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <cstring>
#include <ctime>
#include <string>

#include "discord_game_sdk.h"
#include "discord.h"

namespace AttorneyOnline {

class AORPC : public QObject {

    Q_OBJECT
  std::unique_ptr<discord::Core> core;

private:
  discord::ClientId client_id = 399779271737868288;
  std::string server_name, server_id;
  int64_t timestamp;
  QTimer rpc_beat;

  discord::Activity activity;

  const int RPC_HEARTBEAT = 1 * 1000;

public:
  AORPC();
  ~AORPC() = default;

  void publish_rpc();
  void state_lobby();
  void state_server(std::string name, std::string server_id);
  void state_character(std::string name);
  void state_spectate();
};

} // namespace AttorneyOnline
#endif // DISCORD_RICH_PRESENCE_H
