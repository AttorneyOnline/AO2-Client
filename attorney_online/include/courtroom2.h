#ifndef COURTROOM2_H
#define COURTROOM2_H

#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QPlainTextEdit>
#include <QSignalMapper>
#include <QSlider>
#include <QSpinBox>
#include <QTextBrowser>
#include <QVector>

#include <QAction>
#include <QBrush>
#include <QDebug>
#include <QFileDialog>
#include <QFont>
#include <QInputDialog>
#include <QRegExp>
#include <QScrollBar>
#include <QTextCharFormat>

#include "aochatqueue.h"
#include "network/client.h"
#include "options.h"

using namespace AttorneyOnline;

class AOApplication;
class AOViewport;

class Courtroom2 : public QMainWindow {
  Q_OBJECT
public:
  Courtroom2(std::shared_ptr<Client> client, AOApplication *p_ao_app = nullptr);
  ~Courtroom2();

  void setupCourtroom(QString p_server_name = "NoName");

private:
  AOApplication *ao_app;
  std::shared_ptr<Client> client;
  Options options;

  QString previous_ic_message = "";

  // Whether or not the character selection screen will quit the server
  // when canceling.
  bool quitOnCancel = true;

  // List of registered windows; used for saving/loading layout
  QSet<QWidget *> windows;
  const QString LAYOUT_AUTOSAVE_FILE = "autosave_layout.dat";
  const int LAYOUT_AUTOSAVE_INTERVAL = 30000;

  QMainWindow *windowWidget;
  AOViewport *viewport;

  // Registers dockedWidget to the QMenu Action for the widget to hide/show it.
  template <typename T>
  void registerWindow(T *&widget, const QString &name, QDockWidget *&dockWidget,
                      QAction *&toggleAction);

  void initBass();

  AOChatQueue m_chat_queue;

  // char id, muted or not
  //  XXX: needs rework
  QMap<int, bool> mute_map;

  QMenu *ui_window_menu;
  QAction *ui_load_layout;
  QAction *ui_save_layout;

  enum LogMode {
    IO_ONLY,
    DISPLAY_ONLY,
    DISPLAY_AND_IO,
    QUEUED,
  };

private slots:

  void onICMessage(DataTypes::MSPacket f_packet);
  void onICMessageDequeue();
};

#endif
