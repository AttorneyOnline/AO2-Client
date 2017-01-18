#ifndef AOAPPLICATION_H
#define AOAPPLICATION_H

#include "aopacket.h"
#include "datatypes.h"

#include <QApplication>
#include <QVector>
#include <QFile>

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
  Courtroom *w_courtroom;

  bool lobby_constructed = false;
  bool courtroom_constructed = false;

  void construct_lobby();
  void destruct_lobby();

  void construct_courtroom();
  void destruct_courtroom();

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

  int get_release() {return RELEASE;}
  int get_major_version() {return MAJOR_VERSION;}
  int get_minor_version() {return MINOR_VERSION;}
  QString get_version_string();

  void set_favorite_list();
  QVector<server_type>& get_favorite_list() {return favorite_list;}
  void add_favorite_server(int p_server);

  void set_server_list();
  QVector<server_type>& get_server_list() {return server_list;}

  void set_user_theme();
  QString get_user_theme() {return user_theme;}

  //path functions
  QString get_base_path();
  QString get_theme_path();
  QString get_default_theme_path();
  QString get_character_path(QString p_character);
  QString get_demothings_path();

  //text file functions
  QString read_user_theme();
  void write_to_serverlist_txt(QString p_line);
  QVector<server_type> read_serverlist_txt();
  pos_size_type get_pos_and_size(QString p_identifier, QString p_design_path);

private:
  const int RELEASE = 2;
  const int MAJOR_VERSION = 1;
  const int MINOR_VERSION = 0;

  QString user_theme = "default";

  QVector<server_type> server_list;
  QVector<server_type> favorite_list;
};

#endif // AOAPPLICATION_H
