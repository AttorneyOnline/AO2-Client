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

#include "aoapplication.h"

#include <QTcpSocket>
#include <QDnsLookup>
#include <QTime>
#include <QTimer>
#include <cstring>

#include <network/client.h>
#include <network/masterserver.h>

class NetworkManager : public QObject
{
  Q_OBJECT

public:
  NetworkManager(AOApplication *parent);
  ~NetworkManager();

  AOApplication *ao_app;
  AttorneyOnline::MasterServer *ms_socket;
  AttorneyOnline::Client *server_socket;
  QTimer *ms_reconnect_timer;

#ifdef LOCAL_MS
  QString ms_nosrv_hostname = "localhost";
#else
  QString ms_nosrv_hostname = "master.aceattorneyonline.com";
#endif

  const int ms_port = 27016;
  const int timeout_milliseconds = 2000;

  const int ms_reconnect_delay_ms = 7000;

  void connect_to_master();
  void connect_to_master_nosrv();
  void connect_to_server(server_type p_server);

signals:
  void ms_connect_finished(bool success, bool will_retry);

private:
  void perform_srv_lookup();

private slots:
  void on_srv_lookup();
  void on_ms_nosrv_connect_success();
  void on_ms_socket_error(QAbstractSocket::SocketError error);
  void retry_ms_connect();
};

#endif // NETWORKMANAGER_H
