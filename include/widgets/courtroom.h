#ifndef COURTROOM_H
#define COURTROOM_H

#include "aoimage.h"
#include "aobutton.h"
#include "aopacket.h"
#include "aoscene.h"
#include "aomovie.h"
#include "aocharmovie.h"
#include "aomusicplayer.h"
#include "aosfxplayer.h"
#include "aoblipplayer.h"
#include "aolineedit.h"
#include "aotextedit.h"
#include "datatypes.h"
#include "aoapplication.h"
#include "lobby.h"
#include "file_functions.h"
#include "datatypes.h"
#include "debug_functions.h"
#include "aoiclog.h"
#include "aojukebox.h"
#include "aorooms.h"
#include "aomixer.h"
#include "aochat.h"
#include "aoroomcontrols.h"
#include "aoevidence.h"

#include <QMainWindow>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QVector>
#include <QCloseEvent>
#include <QSignalMapper>
#include <QMap>
#include <QTextBrowser>
#include <QSpinBox>

#include <QDebug>
#include <QScrollBar>
#include <QRegExp>
#include <QBrush>
#include <QTextCharFormat>
#include <QFont>
#include <QInputDialog>
#include <QFileDialog>
#include <QAction>

class AOApplication;
class AOViewport;

// This is for inline message-colouring.
enum INLINE_COLOURS {
    INLINE_BLUE,
    INLINE_GREEN,
    INLINE_ORANGE,
    INLINE_GREY
};

class Courtroom : public QMainWindow
{
  Q_OBJECT
public:
  explicit Courtroom(AOApplication *p_ao_app);

  void append_char(char_type p_char) { char_list.append(p_char); }
  void append_evidence(evi_type p_evi) { evidence_list.append(p_evi); }
  void set_music(QVector<track_type> &f_music) { ui_music_list->set_tracks(f_music); }
  void set_areas(QVector<area_type> &f_areas) { area_list = f_areas; ui_area_list->set_areas(f_areas); }

  void arup_append(int players, QString status, QString cm, QString locked);
  void arup_modify(int type, int place, QString value);

  //sets status as taken on character with cid n_char and places proper shading on charselect
  void set_taken(int n_char, bool p_taken);

  //sets the evidence list member variable to argument
  void set_evidence_list(QVector<evi_type> &p_evi_list);

  //called when a DONE#% from the server was received
  void done_received();

  //send a message that the player is banned and quits the server
  void set_ban(int p_cid);

  //cid = character id, returns the cid of the currently selected character
  int get_cid() { return m_cid; }
  QString get_current_char() { return current_char; }

  //properly sets up some varibles: resets user state
  void enter_courtroom(int p_cid);

  void list_areas();

  //these are for OOC chat
  void append_ms_chatmessage(QString f_name, QString f_message);
  void append_server_chatmessage(QString p_name, QString p_message, QString p_colour);

  void handle_chatmessage(QStringList *p_contents);
  void handle_background(QString background);
  void handle_wtce(QString wtce, int variant);

  //prints who played the song to IC chat and plays said song(if found on local filesystem)
  //takes in a list where the first element is the song name and the second is the char id of who played it
  void handle_song(QStringList *p_contents);

  //sets the hp bar of defense(p_bar 1) or pro(p_bar 2)
  //state is an number between 0 and 10 inclusive
  void set_hp_bar(HEALTH_TYPE p_bar, int p_state);

  void announce_case(QString title, bool def, bool pro, bool jud, bool jur, bool steno);

  void check_connection_received();

  ~Courtroom();

  static constexpr int chatmessage_size = 23;
private:
  AOApplication *ao_app;

  QVector<char_type> char_list;
  QVector<evi_type> evidence_list;
  QVector<area_type> area_list;

  //triggers ping_server() every 60 seconds
  QTimer *keepalive_timer;

  QString previous_ic_message = "";

  //char id, muted or not
  QMap<int, bool> mute_map;

  //character id, which index of the char_list the player is
  int m_cid = -1;
  //cid and this may differ in cases of ini-editing
  QString current_char = "";

  int objection_state = 0;
  int realization_state = 0;
  int text_color = 0;
  bool is_presenting_evidence = false;

  int defense_bar_state = 0;
  int prosecution_bar_state = 0;

  int current_emote = 0;
  QVector<evi_type> local_evidence_list;

  AOMusicPlayer *music_player;
  AOSfxPlayer *modcall_player;

  AOViewport *ui_viewport;
  AOICLog *ui_ic_chatlog;
  AORoomChooser *ui_area_list;
  AOJukebox *ui_music_list;
  AOMixer *ui_mixer;
  AOChat *ui_ic_chat;
  AORoomControls *ui_room_controls;
  AOEvidence *ui_evidence;

  AOServerChat *ui_ms_chat;
  AOServerChat *ui_server_chat;

  QAction *ui_change_character;
  QAction *ui_reload_theme;
  QAction *ui_call_mod;
  QAction *ui_settings;
  QAction *ui_announce_case;

  QAction *ui_modcall_notify;
  QAction *ui_casing;
  QAction *ui_showname_enable;

  bool chooseCharacter();
public slots:
  void mod_called(QString p_ip);

  void case_called(QString msg, std::bitset<CASING_FLAGS_COUNT> flags);

private slots:
  void on_ic_chat_messageSent();

  void on_ooc_return_pressed(QString name, QString message);
  void on_ms_return_pressed(QString name, QString message);
  void on_icChat_positionChanged(QString pos);
  void on_jukebox_trackSelected(QString track);
  void on_rooms_roomSelected(QString room);

  void on_mute_triggered();
  void on_pair_triggered();

  void on_text_color_changed(int p_color);

  void on_music_slider_moved(int p_value);
  void on_sfx_slider_moved(int p_value);
  void on_blip_slider_moved(int p_value);

  void on_roomControls_wtce(WTCE_TYPE type);

  void on_change_character_triggered();
  void on_reload_theme_triggered();
  void on_call_mod_triggered();
  void on_settings_triggered();
  void on_announce_case_triggered();

  void on_showname_enable_triggered();

  void on_quit_triggered();

  void on_casing_triggered();

  void ping_server();

  void load_bass_opus_plugin();
};

#endif // COURTROOM_H
