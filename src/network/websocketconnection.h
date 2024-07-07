#pragma once

#include "aopacket.h"
#include "serverinfo.h"

#include <QObject>
#include <QWebSocket>

class AOApplication;

class WebSocketConnection : public QObject
{
  Q_OBJECT

public:
  explicit WebSocketConnection(AOApplication *ao_app, QObject *parent = nullptr);
  virtual ~WebSocketConnection();

  bool isConnected();

  void connectToServer(const ServerInfo &server);
  void disconnectFromServer();

  void sendPacket(AOPacket packet);

Q_SIGNALS:
  void connectedToServer();
  void disconnectedFromServer();
  void errorOccurred(QString error);

  void receivedPacket(AOPacket packet);

private:
  AOApplication *ao_app;

  QWebSocket *m_socket;
  QAbstractSocket::SocketState m_last_state;

private Q_SLOTS:
  void onError();
  void onStateChanged(QAbstractSocket::SocketState state);
  void onTextMessageReceived(QString message);
};
