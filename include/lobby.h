#ifndef LOBBY_H
#define LOBBY_H

#include "aobutton.h"
#include "aoimage.h"
#include "aopacket.h"
#include "aotextarea.h"

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QTextBrowser>
#include <QTreeWidget>
#include <QDebug>
#include <QScrollBar>
#include <QHeaderView>
#ifdef ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

class AOApplication;

class Lobby : public QMainWindow {
  Q_OBJECT

public:
  Lobby(AOApplication *p_ao_app);

  void set_widgets();
  void set_stylesheet(QWidget *widget);
  void set_stylesheets();
  void set_fonts();
  void set_font(QWidget *widget, QString p_identifier);
  void set_loading_text(QString p_text);
  void show_loading_overlay() { ui_loading_background->show(); }
  void hide_loading_overlay() { ui_loading_background->hide(); }

  void set_loading_value(int p_value);

  void begin_connection();

  ~Lobby();

private:
  AOApplication *ao_app;

  AOImage *ui_loading_background;
  QTextEdit *ui_loading_text;
  QProgressBar *ui_progress_bar;
  AOButton *ui_cancel;

  void set_size_and_pos(QWidget *p_widget, QString p_identifier);

private slots:
  void on_about_clicked();
};

#endif // LOBBY_H
