#include "networkmanager.h"

#include "datatypes.h"
#include "debug_functions.h"
#include "lobby.h"

NetworkManager::NetworkManager(AOApplication *parent) : QObject(parent)
{
  ao_app = parent;

  ms_socket = new QTcpSocket(this);
  server_socket = new QTcpSocket(this);

  ms_reconnect_timer = new QTimer(this);
  ms_reconnect_timer->setSingleShot(true);
  QObject::connect(ms_reconnect_timer, SIGNAL(timeout()), this,
                   SLOT(retry_ms_connect()));

  QObject::connect(ms_socket, SIGNAL(readyRead()), this,
                   SLOT(handle_ms_packet()));
  QObject::connect(server_socket, SIGNAL(readyRead()), this,
                   SLOT(handle_server_packet()));
  QObject::connect(server_socket, SIGNAL(disconnected()), ao_app,
                   SLOT(server_disconnected()));

  QString master_config =
      ao_app->configini->value("master", "").value<QString>();
  if (master_config != "")
    ms_nosrv_hostname = master_config;
}

NetworkManager::~NetworkManager() {}

void NetworkManager::connect_to_master()
{
  ms_socket->close();
  ms_socket->abort();

#ifdef MS_FAILOVER_SUPPORTED
  perform_srv_lookup();
#else
  connect_to_master_nosrv();
#endif
}

void NetworkManager::connect_to_master_nosrv()
{
  QObject::connect(ms_socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
                   SLOT(on_ms_socket_error(QAbstractSocket::SocketError)));

  QObject::connect(ms_socket, SIGNAL(connected()), this,
                   SLOT(on_ms_nosrv_connect_success()));
  ms_socket->connectToHost(ms_nosrv_hostname, ms_port);
}

void NetworkManager::connect_to_server(server_type p_server)
{
  server_socket->close();
  server_socket->abort();

  server_socket->connectToHost(p_server.ip, p_server.port);
}

void NetworkManager::ship_ms_packet(QString p_packet)
{
  if (!ms_socket->isOpen()) {
    retry_ms_connect();
  }
  else {
    ms_socket->write(p_packet.toUtf8());
  }
}

void NetworkManager::ship_server_packet(QString p_packet)
{
  server_socket->write(p_packet.toUtf8());
}

void NetworkManager::handle_ms_packet()
{
  QByteArray buffer = ms_socket->readAll();
  QString in_data = QString::fromUtf8(buffer, buffer.size());

  if (!in_data.endsWith("%")) {
    ms_partial_packet = true;
    ms_temp_packet += in_data;
    return;
  }

  else {
    if (ms_partial_packet) {
      in_data = ms_temp_packet + in_data;
      ms_temp_packet = "";
      ms_partial_packet = false;
    }
  }

  QStringList packet_list =
      in_data.split("%", QString::SplitBehavior(QString::SkipEmptyParts));

  for (QString packet : packet_list) {
    AOPacket *f_packet = new AOPacket(packet);

    ao_app->ms_packet_received(f_packet);
  }
}

void NetworkManager::perform_srv_lookup()
{
#ifdef MS_FAILOVER_SUPPORTED
  ms_dns = new QDnsLookup(QDnsLookup::SRV, ms_srv_hostname, this);

  connect(ms_dns, SIGNAL(finished()), this, SLOT(on_srv_lookup()));
  ms_dns->lookup();
#endif
}

void NetworkManager::on_srv_lookup()
{
#ifdef MS_FAILOVER_SUPPORTED
  bool connected = false;
  if (ms_dns->error() != QDnsLookup::NoError) {
    qWarning("SRV lookup of the master server DNS failed.");
    ms_dns->deleteLater();
  }
  else {
    const auto srv_records = ms_dns->serviceRecords();

    for (const QDnsServiceRecord &record : srv_records) {
#ifdef DEBUG_NETWORK
      qDebug() << "Connecting to " << record.target() << ":" << record.port();
#endif
      ms_socket->connectToHost(record.target(), record.port());
      QTime timer;
      timer.start();
      do {
        ao_app->processEvents();
        if (ms_socket->state() == QAbstractSocket::ConnectedState) {
          connected = true;
          break;
        }
        else if (ms_socket->state() != QAbstractSocket::ConnectingState &&
                 ms_socket->state() != QAbstractSocket::HostLookupState &&
                 ms_socket->error() != -1) {
          qDebug() << ms_socket->error();
          qWarning() << "Error connecting to master server:"
                     << ms_socket->errorString();
          ms_socket->abort();
          ms_socket->close();
          break;
        }
      } while (timer.elapsed() <
               timeout_milliseconds); // Very expensive spin-wait loop - it will
                                      // bring CPU to 100%!
      if (connected) {
        // Connect a one-shot signal in case the master server disconnects
        // randomly
        QObject::connect(
            ms_socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(on_ms_socket_error(QAbstractSocket::SocketError)));
        break;
      }
      else {
        ms_socket->abort();
        ms_socket->close();
      }
    }
  }

  // Failover to non-SRV connection
  if (!connected)
    connect_to_master_nosrv();
  else
    emit ms_connect_finished(connected, false);
#endif
}

void NetworkManager::on_ms_nosrv_connect_success()
{
  emit ms_connect_finished(true, false);

  QObject::disconnect(ms_socket, SIGNAL(connected()), this,
                      SLOT(on_ms_nosrv_connect_success()));

  QObject::connect(ms_socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
                   SLOT(on_ms_socket_error(QAbstractSocket::SocketError)));
}

void NetworkManager::on_ms_socket_error(QAbstractSocket::SocketError error)
{
  qWarning() << "Master server socket error:" << ms_socket->errorString() << "("
             << error << ")";

  // Disconnect the one-shot signal - this way, failover connect attempts
  // don't trigger a full retry
  QObject::disconnect(ms_socket, SIGNAL(error(QAbstractSocket::SocketError)),
                      this,
                      SLOT(on_ms_socket_error(QAbstractSocket::SocketError)));

  emit ms_connect_finished(false, true);

  ms_reconnect_timer->start(ms_reconnect_delay * 1000);
}

void NetworkManager::retry_ms_connect()
{
  if (!ms_reconnect_timer->isActive() &&
      ms_socket->state() != QAbstractSocket::ConnectingState)
    connect_to_master();
}

void NetworkManager::handle_server_packet()
{
  QByteArray buffer = server_socket->readAll();
  QString in_data = QString::fromUtf8(buffer, buffer.size());

  if (!in_data.endsWith("%")) {
    partial_packet = true;
    temp_packet += in_data;
    return;
  }

  else {
    if (partial_packet) {
      in_data = temp_packet + in_data;
      temp_packet = "";
      partial_packet = false;
    }
  }

  QStringList packet_list =
      in_data.split("%", QString::SplitBehavior(QString::SkipEmptyParts));

  for (QString packet : packet_list) {
    AOPacket *f_packet = new AOPacket(packet);

    ao_app->server_packet_received(f_packet);
  }
}
