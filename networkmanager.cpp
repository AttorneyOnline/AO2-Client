#include "networkmanager.h"

NetworkManager::NetworkManager()
{
  ms_socket = new QTcpSocket();
  server_socket = new QTcpSocket();
}

NetworkManager::~NetworkManager()
{

}

