#pragma once

#include "netconnection.h"

class NetworkManager;

#include <QWebSocket>

class NetWebSocketConnection : public NetConnection
{
public:
  NetWebSocketConnection(NetworkManager *networkManager);
  virtual ~NetWebSocketConnection();

  bool isConnected() override;

  void connectToServer(ServerInfo &server) override;
  void disconnectFromServer() override;

  void sendPacket(AOPacket packet) override;

private:
  NetworkManager *m_network_manager;

  QWebSocket *m_socket;
  QAbstractSocket::SocketState m_last_state;

private Q_SLOTS:
  void onError();
  void onStateChanged(QAbstractSocket::SocketState state);
  void onTextMessageReceived(QString message);
};
