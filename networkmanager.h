#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpSocket>

class NetworkManager
{
public:
  NetworkManager();
  ~NetworkManager();

  QTcpSocket *ms_socket;
  QTcpSocket *server_socket;
};

#endif // NETWORKMANAGER_H
