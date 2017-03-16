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

private slots:
  void handle_ms_packet();
  void handle_server_packet();
};

#endif // NETWORKMANAGER_H
