#pragma once

#include <QString>

class ServerInfo
{
public:
  QString name;
  QString description;
  QString address;
  quint16 port = 0;
  QString protocol = "ws";

  QString toString() const;
};
