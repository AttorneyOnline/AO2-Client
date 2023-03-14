#ifndef DISCORD_H
#define DISCORD_H
#pragma once

#include <QObject>

namespace AttorneyOnline {
  namespace Core {
    class Discord : public QObject {
    public:
      Discord(QObject *parent = nullptr) : QObject(parent){};
      ~Discord() = default;

    public slots:
      virtual void state_lobby() = 0;
      virtual void state_server(QString server_name, QString server_id) = 0;
      virtual void state_character(QString character_name) = 0;
      virtual void state_spectate() = 0;
    };
  } // namespace Core
} // namespace AttorneyOnline

#endif // DISCORD_H
