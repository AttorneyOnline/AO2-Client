#include "networkmanager.h"

#include "datatypes.h"
#include "debug_functions.h"
#include "lobby.h"


NetworkManager::NetworkManager(AOApplication *parent) : QObject(parent)
{
  ao_app = parent;

  ms_socket = new QTcpSocket();
  server_socket = new QTcpSocket();

  QObject::connect(ms_socket, SIGNAL(readyRead()), this, SLOT(handle_ms_packet()));
  QObject::connect(server_socket, SIGNAL(readyRead()), this, SLOT(handle_server_packet()));
}

NetworkManager::~NetworkManager()
{
  delete ms_socket;
  delete server_socket;
}

void NetworkManager::connect_to_master()
{
  ms_socket->close();
  ms_socket->abort();

  ms_socket->connectToHost(ms_hostname, ms_port);
}

void NetworkManager::send_ms_packet(AOPacket *p_packet)
{
  QString f_packet = p_packet->to_string();

  ms_socket->write(f_packet.toLocal8Bit());
  qDebug() << "S(ms):" << f_packet;
}

void NetworkManager::send_server_packet(AOPacket *p_packet)
{
  QString f_packet = p_packet->to_string();

  ms_socket->write(f_packet.toLocal8Bit());
  qDebug() << "S(ms):" << f_packet;
}

void NetworkManager::handle_ms_packet()
{
  char buffer[16384] = {0};
  ms_socket->read(buffer, ms_socket->bytesAvailable());

  QString in_data = buffer;

  if (!in_data.endsWith("%"))
  {
    partial_packet = true;
    temp_packet += in_data;
    return;
  }

  else
  {
    if (partial_packet)
    {
      in_data = temp_packet + in_data;
      temp_packet = "";
      partial_packet = false;
    }
  }

  QStringList packet_list = in_data.split("%", QString::SplitBehavior(QString::SkipEmptyParts));

  for (QString packet : packet_list)
  {
    AOPacket *f_packet = new AOPacket(packet);

    ao_app->ms_packet_received(f_packet);

    delete f_packet;
  }
}

void NetworkManager::handle_server_packet()
{

}

