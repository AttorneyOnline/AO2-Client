#ifndef COURTROOM_H
#define COURTROOM_H

#include "aoimage.h"
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

#include "network/client.h"

#include <QMainWindow>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QDockWidget>
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

#define REGISTER_WINDOW(type, name) \
  registerWindow<type>(ui_##name, #name, ui_##name##_dock, ui_toggle_##name);

using namespace AttorneyOnline;

class AOApplication;
class AOViewport;

class Courtroom : public QMainWindow
{
  Q_OBJECT
public:
  explicit Courtroom(AOApplication *p_ao_app, std::shared_ptr<Client> client);

  void chooseCharacter();

  //these are for OOC chat
  void append_ms_chatmessage(QString f_name, QString f_message);
  void append_server_chatmessage(QString p_name, QString p_message, bool special = false);

  void announce_case(QString title, bool def, bool pro, bool jud, bool jur, bool steno);

private:
  AOApplication *ao_app;
  Options options;

  std::shared_ptr<Client> client;

  QString previous_ic_message = "";

  // Whether or not the character selection screen will quit the server
  // when canceling.
  bool quitOnCancel = true;

  //char id, muted or not
  // XXX: needs rework
  QMap<int, bool> mute_map;

  // List of registered windows; used for saving/loading layout
  QSet<QWidget *> windows;
  const QString LAYOUT_AUTOSAVE_FILE = "last_layout.dat";
  const int LAYOUT_AUTOSAVE_INTERVAL = 30000;

  QMainWindow *windowWidget;

  AOMusicPlayer *music_player;
  AOSfxPlayer *modcall_player;

  AOViewport *ui_viewport;
  QDockWidget *ui_viewport_dock;

  AOICLog *ui_ic_chatlog;
  QDockWidget *ui_ic_chatlog_dock;
  QAction *ui_toggle_ic_chatlog;

  AORoomChooser *ui_room_chooser;
  QDockWidget *ui_room_chooser_dock;
  QAction *ui_toggle_room_chooser;

  AOJukebox *ui_music_list;
  QDockWidget *ui_music_list_dock;
  QAction *ui_toggle_music_list;

  AOMixer *ui_mixer;
  QDockWidget *ui_mixer_dock;
  QAction *ui_toggle_mixer;

  AOChat *ui_ic_chat;
  QDockWidget *ui_ic_chat_dock;

  AORoomControls *ui_room_controls;
  QDockWidget *ui_room_controls_dock;
  QAction *ui_toggle_room_controls;

  AOEvidence *ui_evidence;
  QDockWidget *ui_evidence_dock;
  QAction *ui_toggle_evidence;

  AOServerChat *ui_ms_chat;
  QDockWidget *ui_ms_chat_dock;
  QAction *ui_toggle_ms_chat;

  AOServerChat *ui_server_chat;
  QDockWidget *ui_server_chat_dock;
  QAction *ui_toggle_server_chat;

  QAction *ui_mute;
  QAction *ui_pair;
  QAction *ui_change_character;
  QAction *ui_reload_theme;
  QAction *ui_call_mod;
  QAction *ui_open_settings;
  QAction *ui_announce_case;

  QAction *ui_modcall_notify;
  QAction *ui_casing;
  QAction *ui_showname_enable;

  QMenu *ui_window_menu;
  QAction *ui_load_layout;
  QAction *ui_save_layout;

  template<typename T>
  void registerWindow(T *&widget, const QString &name,
                      QDockWidget *&dockWidget, QAction *&toggleAction);
  void initBASS();

private slots:
  void resetCourtroom();

  void onICMessageSend();
  void onICMessage(const chat_message_type &message);
  void onDisconnect(DisconnectReason code, const QString &message);
  void onTrackChange(const QString &track, const QString &showname);
  void onModCall(const QString &message);
  void onCaseCall(const QString &message,
                  const std::bitset<CASING_FLAGS_COUNT> casingFlags);
  void onOOCMessage(const QString &name, const QString &message);
  void onAreaUpdate();
  void onEvidenceUpdate();

  void onOOCSend(QString name, QString message);
  void onGlobalChatSend(QString name, QString message);
  void onICPositionChange(QString pos);
  void onJukeboxTrackSelect(QString track);

  void on_mute_triggered();
  void on_pair_triggered();

  void onMixerVolumeChange(AUDIO_TYPE type, int volume);

  void onRequestHealthChange(HEALTH_TYPE type, int value);
  void onRequestWTCE(WTCE_TYPE type);

  void on_change_character_triggered();
  void on_reload_theme_triggered();
  void on_call_mod_triggered();
  void on_open_settings_triggered();
  void on_announce_case_triggered();

  void on_showname_enable_triggered();

  void on_quit_triggered();

  void on_casing_triggered();

  void on_load_layout_triggered();
  void on_save_layout_triggered();

  void load_bass_opus_plugin();

  bool loadLayout(const QString &filename);
  bool saveLayout(const QString &filename);
  void autosaveLayout();

protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // COURTROOM_H
