#ifndef AOAPPLICATION_H
#define AOAPPLICATION_H

#include "aopacket.h"
#include "datatypes.h"

#include <QApplication>
#include <QVector>

class NetworkManager;
class Lobby;
class Courtroom;

class AOApplication : public QApplication
{
  Q_OBJECT

public:
  AOApplication(int &argc, char **argv);
  ~AOApplication();

  NetworkManager *net_manager;
  Lobby *w_lobby;
  //T0D0: change to custom class "Courtroom"
  QMainWindow *w_courtroom;

  bool lobby_constructed = false;
  bool courtroom_constructed = false;

  void construct_lobby();
  void destruct_lobby();

  void construct_courtroom();
  void destruct_courtroom();

  QVector<server_type> server_list;
  QVector<server_type> favorite_list;

  void ms_packet_received(AOPacket *p_packet);
  void server_packet_received(AOPacket *p_packet);

  void send_ms_packet(AOPacket *p_packet);
  void send_server_packet(AOPacket *p_packet);

  /////////////////server metadata//////////////////

  unsigned int s_decryptor = 5;
  bool encryption_needed = true;
  bool ao2_features = false;
  //player number, it's hardly used but might be needed for some old servers
  bool s_pv = 0;

  //////////////////////////////////////////////////
};

#endif // AOAPPLICATION_H
