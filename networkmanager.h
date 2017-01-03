#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpSocket>

class AOApplication;

class NetworkManager
{
public:
  NetworkManager(AOApplication *parent);
  ~NetworkManager();

  AOApplication *ao_app;
  QTcpSocket *ms_socket;
  QTcpSocket *server_socket;
};

#endif // NETWORKMANAGER_H
