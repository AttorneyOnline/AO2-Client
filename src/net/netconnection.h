#pragma once

#include "aopacket.h"
#include "datatypes.h"

#include <QObject>

class NetConnection : public QObject
{
  Q_OBJECT

public:
  explicit NetConnection(QObject *parent = nullptr);

  virtual bool isConnected() = 0;

  virtual void connectToServer(ServerInfo &server) = 0;
  virtual void disconnectFromServer() = 0;

  virtual void sendPacket(AOPacket packet) = 0;

Q_SIGNALS:
  void connectedToServer();
  void disconnectedFromServer();
  void errorOccurred(QString error);

  void receivedPacket(AOPacket packet);
};
