#ifndef AOAPPLICATION_H
#define AOAPPLICATION_H

#include "aopacket.h"
#include "datatypes.h"
#include "discord_rich_presence.h"

#include <QApplication>
#include <QVector>
#include <QFile>
#include <QSettings>

#include <QDebug>
#include <QRect>
#include <QDesktopWidget>

#include <QCryptographicHash>

#include <QDir>
#include <QStandardPaths>

#include <QTextStream>
#include <QStringList>
#include <QColor>

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
  AttorneyOnline::Discord *discord;

  bool lobby_constructed = false;
  bool courtroom_constructed = false;

  void construct_lobby();
  void destruct_lobby();

  void construct_courtroom();
  void destruct_courtroom();

  void send_ms_packet(AOPacket *p_packet);
  void send_server_packet(AOPacket *p_packet, bool encoded = true);

  void call_settings_menu();
  void call_announce_menu(Courtroom *court);

  /////////////////server metadata//////////////////

  unsigned int s_decryptor = 5;
  bool encryption_needed = true;

  bool yellow_text_enabled = false;
  bool prezoom_enabled = false;
  bool flipping_enabled = false;
  bool custom_objection_enabled = false;
  bool improved_loading_enabled = false;
  bool desk_mod_enabled = false;
  bool evidence_enabled = false;
  bool cccc_ic_support_enabled = false;
  bool arup_enabled = false;
  bool casing_alerts_enabled = false;
  bool modcall_reason_enabled = false;

  ///////////////loading info///////////////////

  //player number, it's hardly used but might be needed for some old servers
  int s_pv = 0;

  QString server_software = "";

  int char_list_size = 0;
  int loaded_chars = 0;
  int generated_chars = 0;
  int evidence_list_size = 0;
  int loaded_evidence = 0;
  int music_list_size = 0;
  int loaded_music = 0;

  bool courtroom_loaded = false;

  //////////////////versioning///////////////

  constexpr int get_release() const { return RELEASE; }
  constexpr int get_major_version() const { return MAJOR_VERSION; }
  constexpr int get_minor_version() const { return MINOR_VERSION; }
  QString get_version_string();

  ///////////////////////////////////////////

  void set_favorite_list();
  QVector<server_type>& get_favorite_list() {return favorite_list;}
  void add_favorite_server(int p_server);

  void set_server_list();
  QVector<server_type>& get_server_list() {return server_list;}

  //Returns the character the player has currently selected
  QString get_current_char();

private:
  const int RELEASE = 2;
  const int MAJOR_VERSION = 6;
  const int MINOR_VERSION = 0;

  QString current_theme = "default";

  QVector<server_type> server_list;
  QVector<server_type> favorite_list;

private slots:
  void ms_connect_finished(bool connected, bool will_retry);

public slots:
  void server_disconnected();
  void loading_cancelled();

  void ms_packet_received(AOPacket *p_packet);
  void server_packet_received(AOPacket *p_packet);

  void force_process_events();
};

#endif // AOAPPLICATION_H
