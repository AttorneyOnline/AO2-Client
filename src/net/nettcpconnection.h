#pragma once

#include "aopacket.h"
#include "datatypes.h"
#include "netconnection.h"

#include <QTcpSocket>

class NetTcpConnection : public NetConnection
{
public:
  NetTcpConnection(QObject *parent = nullptr);
  virtual ~NetTcpConnection();

  bool isConnected() override;

  void connectToServer(ServerInfo &server) override;
  void disconnectFromServer() override;

  void sendPacket(AOPacket packet) override;

private:
  QTcpSocket *m_socket;
  QAbstractSocket::SocketState m_last_state;
  QString m_cached_data;

private Q_SLOTS:
  void onErrorOccurred();
  void onStateChanged(QAbstractSocket::SocketState state);
  void onReadyRead();
};
