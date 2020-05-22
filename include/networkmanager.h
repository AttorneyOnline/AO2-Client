#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

// Qt for Android has stubbed QDnsLookup. This is not documented in any part of their wiki.
// This prevents SRV lookup/failover behavior from functioning.
// https://bugreports.qt.io/browse/QTBUG-56143
#ifndef ANDROID
#define MS_FAILOVER_SUPPORTED
#endif

//#define LOCAL_MS

#ifdef LOCAL_MS
#undef MS_FAILOVER_SUPPORTED
#endif

#include "aopacket.h"
#include "aoapplication.h"

#include <QTcpSocket>
#include <QDnsLookup>
#include <QTime>
#include <QTimer>
#include <cstring>

class NetworkManager : public QObject
{
  Q_OBJECT

public:
  NetworkManager(AOApplication *parent);
  ~NetworkManager();

  AOApplication *ao_app;
  QTcpSocket *ms_socket;
  QTcpSocket *server_socket;
  QDnsLookup *ms_dns;
  QTimer *ms_reconnect_timer;

  const QString ms_srv_hostname = "_aoms._tcp.aceattorneyonline.com";
#ifdef LOCAL_MS
  QString ms_nosrv_hostname = "localhost";
#else
  QString ms_nosrv_hostname = "master.aceattorneyonline.com";
#endif

  const quint16 ms_port = 27016;
  const int timeout_milliseconds = 2000;

  // in seconds
  const int ms_reconnect_delay = 7;

  bool ms_partial_packet = false;
  QString ms_temp_packet = "";

  bool partial_packet = false;
  QString temp_packet = "";

  unsigned int s_decryptor = 5;

  void connect_to_master();
  void connect_to_master_nosrv();
  void connect_to_server(server_type p_server);

public slots:
  void ship_ms_packet(QString p_packet);
  void ship_server_packet(QString p_packet);

signals:
  void ms_connect_finished(bool success, bool will_retry);

private:
  void perform_srv_lookup();

private slots:
  void on_srv_lookup();
  void handle_ms_packet();
  void handle_server_packet();
  void on_ms_nosrv_connect_success();
  void on_ms_socket_error(QAbstractSocket::SocketError error);
  void retry_ms_connect();
};

#endif // NETWORKMANAGER_H
