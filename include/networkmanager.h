#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "aoapplication.h"
#include "aopacket.h"

#include <QDnsLookup>
#include <QNetworkAccessManager>
#include <QtWebSockets/QWebSocket>
#include <QTime>
#include <QTimer>

#include <cstring>

enum MSDocumentType {
  PrivacyPolicy,
  Motd,
  ClientVersion
};

class NetworkManager : public QObject {
  Q_OBJECT

private:
  AOApplication *ao_app;
  QNetworkAccessManager *http;

  union {
    QWebSocket *ws;
    QTcpSocket *tcp;
  } server_socket;
  connection_type active_connection_type;
  bool connected = false;

  QTimer *heartbeat_timer;

  const QString DEFAULT_MS_BASEURL = "http://servers.aceattorneyonline.com";
  QString ms_baseurl = DEFAULT_MS_BASEURL;

  const int heartbeat_interval = 60 * 5 * 1000;

  bool partial_packet = false;
  QString temp_packet = "";

  unsigned int s_decryptor = 5;

public:
  explicit NetworkManager(AOApplication *parent);
  ~NetworkManager() = default;

  void connect_to_server(server_type p_server);
  void disconnect_from_server();

signals:
  void server_connected(bool state);

public slots:
  void get_server_list(const std::function<void()> &cb);
  void ship_server_packet(QString p_packet);
  void join_to_server();
  void handle_server_packet(const QString& p_data);

  void request_document(MSDocumentType document_type,
                        const std::function<void(QString)> &cb);
  void send_heartbeat();
private slots:
  void ms_request_finished(QNetworkReply *reply,
                           const std::function<void()> &cb);

private:
  QString get_user_agent() const {
    return QStringLiteral("AttorneyOnline/%1 (Desktop)")
        .arg(ao_app->get_version_string());
  }
};

#endif // NETWORKMANAGER_H
