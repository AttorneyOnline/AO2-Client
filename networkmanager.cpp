#include "aoapplication.h"

#include "networkmanager.h"


NetworkManager::NetworkManager(AOApplication *parent)
{
  ao_app = parent;

  ms_socket = new QTcpSocket();
  server_socket = new QTcpSocket();
}

NetworkManager::~NetworkManager()
{
  delete ms_socket;
  delete server_socket;
}

