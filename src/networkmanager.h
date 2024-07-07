#pragma once

#include "aoapplication.h"
#include "aopacket.h"
#include "network/websocketconnection.h"

#include <QDnsLookup>
#include <QNetworkAccessManager>
#include <QTime>
#include <QTimer>
#include <QtWebSockets/QWebSocket>

#include <cstring>

enum MSDocumentType
{
  PrivacyPolicy,
  Motd,
  ClientVersion
};

class NetworkManager : public QObject
{
  Q_OBJECT

public:
  explicit NetworkManager(AOApplication *parent);

  void connect_to_server(ServerInfo p_server);
  void disconnect_from_server();

  QString get_user_agent() const;

public Q_SLOTS:
  void get_server_list();
  void ship_server_packet(AOPacket packet);
  void join_to_server();
  void handle_server_packet(AOPacket packet);

  void request_document(MSDocumentType document_type, const std::function<void(QString)> &cb);
  void send_heartbeat();

Q_SIGNALS:
  void server_connected(bool state);

private Q_SLOTS:
  void ms_request_finished(QNetworkReply *reply);

private:
  AOApplication *ao_app;
  QNetworkAccessManager *http;

  WebSocketConnection *m_connection = nullptr;

  QTimer *heartbeat_timer;

  const QString DEFAULT_MS_BASEURL = "http://servers.aceattorneyonline.com";
  QString ms_baseurl = DEFAULT_MS_BASEURL;

  const int heartbeat_interval = 60 * 5 * 1000;

  unsigned int s_decryptor = 5;
};
