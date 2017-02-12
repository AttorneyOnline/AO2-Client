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
  //T0D0: make this obsolete and break into smaller bools
  bool ao2_features = false;

  bool yellow_text_enabled = false;
  bool prezoom_enabled = false;
  bool flipping_enabled = false;
  bool custom_objection_enabled = false;

  ///////////////loading info///////////////////

  //player number, it's hardly used but might be needed for some old servers
  int s_pv = 0;

  int char_list_size = 0;
  int loaded_chars = 0;
  int evidence_list_size = 0;
  int loaded_evidence = 0;
  int music_list_size = 0;
  int loaded_music = 0;

  //////////////////versioning///////////////

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

  QString get_current_char();

  //implementation in path_functions.cpp
  QString get_base_path();
  QString get_theme_path();
  QString get_default_theme_path();
  QString get_character_path(QString p_character);
  QString get_demothings_path();
  QString get_sounds_path();
  QString get_music_path(QString p_song);
  QString get_background_path();
  QString get_default_background_path();

  //implementation in text_file_functions.cpp
  QString read_user_theme();
  void write_to_serverlist_txt(QString p_line);
  QVector<server_type> read_serverlist_txt();
  pos_size_type get_pos_and_size(QString p_identifier, QString p_design_path);
  QString read_char_ini(QString p_char, QString p_search_line, QString target_tag, QString terminator_tag);
  QString get_char_side(QString p_char);
  QString get_showname(QString p_char);
  QString get_chat(QString p_char);
  int get_preanim_duration(QString p_char, QString p_emote);
  int get_text_delay(QString p_char, QString p_emote);
  QString get_char_name(QString p_char);
  int get_emote_number(QString p_char);
  QString get_emote(QString p_char, int p_emote);
  QString get_pre_emote(QString p_char, int p_emote);
  QString get_sfx_name(QString p_char, int p_emote);
  int get_sfx_delay(QString p_char, int p_emote);
  int get_emote_mod(QString p_char, int p_emote);
  QString get_gender(QString p_char);

private:
  const int RELEASE = 2;
  const int MAJOR_VERSION = 1;
  const int MINOR_VERSION = 0;

  QString user_theme = "default";

  QVector<server_type> server_list;
  QVector<server_type> favorite_list;

public slots:
  void loading_cancelled();
};

#endif // AOAPPLICATION_H
