#ifndef AODISCORD_H
#define AODISCORD_H
#pragma once

#include "include/interfaces/network/discord.hpp"

class AODiscord : public AttorneyOnline::Core::Discord {
public:
  explicit AODiscord(QObject *parent = nullptr);
  ~AODiscord() = default;

public slots:
  void state_lobby() override;
  void state_server(QString server_name, QString server_id) override;
  void state_character(QString character_name) override;
  void state_spectate() override;

private:
  const char *APPLICATION_ID = "399779271737868288";
  QString m_server_name;
  QString m_server_id;
  int64_t m_timestamp;
};

#endif
