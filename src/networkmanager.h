#pragma once

#include "aoapplication.h"
#include "aopacket.h"

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

public Q_SLOTS:
  void get_server_list();
  void ship_server_packet(AOPacket p_packet);
  void join_to_server();
  void handle_server_packet(const QString &p_data);

  void request_document(MSDocumentType document_type, const std::function<void(QString)> &cb);
  void send_heartbeat();

Q_SIGNALS:
  void server_connected(bool state);

private Q_SLOTS:
  void ms_request_finished(QNetworkReply *reply);

private:
  AOApplication *ao_app;
  QNetworkAccessManager *http;

  union
  {
    QWebSocket *ws;
    QTcpSocket *tcp;
  } server_socket;
  ServerConnectionType active_connection_type;
  bool connected = false;

  QTimer *heartbeat_timer;

  const QString DEFAULT_MS_BASEURL = "http://servers.aceattorneyonline.com";
  QString ms_baseurl = DEFAULT_MS_BASEURL;

  const int heartbeat_interval = 60 * 5 * 1000;

  bool partial_packet = false;
  QString temp_packet;

  unsigned int s_decryptor = 5;

  QString get_user_agent() const { return QStringLiteral("AttorneyOnline/%1 (Desktop)").arg(ao_app->get_version_string()); }
};
