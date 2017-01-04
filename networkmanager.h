#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "aopacket.h"
#include "aoapplication.h"

#include <QTcpSocket>

class NetworkManager : public QObject
{
  Q_OBJECT

public:
  NetworkManager(AOApplication *parent);
  ~NetworkManager();

  AOApplication *ao_app;
  QTcpSocket *ms_socket;
  QTcpSocket *server_socket;

  QString ms_hostname = "master.aceattorneyonline.com";
  int ms_port = 27016;

  bool partial_packet = false;
  QString temp_packet = "";

  void connect_to_master();

public slots:
  void send_ms_packet(AOPacket *p_packet);

  void send_server_packet(AOPacket *p_packet);

private slots:
  void handle_ms_packet();
  void handle_server_packet();
};

#endif // NETWORKMANAGER_H
