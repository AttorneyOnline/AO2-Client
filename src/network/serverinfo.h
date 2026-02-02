#pragma once

#include <QString>

class ServerInfo
{
public:
  QString name;
  QString description;
  QString address;
  quint16 port = 0;
  bool legacy = false;
  bool ssl = false;

  QString toString() const;
};
