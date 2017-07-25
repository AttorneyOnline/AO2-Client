#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "aopacket.h"
#include "aoapplication.h"

#include <QTcpSocket>
#include <QDnsLookup>
#include <QTime>

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

  QString ms_hostname = "_aoms._tcp.aceattorneyonline.com";
  int ms_port = 27016;
  const int timeout_milliseconds = 2000;

  bool ms_partial_packet = false;
  QString ms_temp_packet = "";

  bool partial_packet = false;
  QString temp_packet = "";

  unsigned int s_decryptor = 5;

  void connect_to_master();
  void connect_to_server(server_type p_server);

public slots:
  void ship_ms_packet(QString p_packet);
  void ship_server_packet(QString p_packet);

signals:
  void ms_connect_finished(bool success);

private:
  void perform_srv_lookup();

private slots:
  void on_srv_lookup();
  void handle_ms_packet();
  void handle_server_packet();
};

#endif // NETWORKMANAGER_H
