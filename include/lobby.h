#ifndef LOBBY_H
#define LOBBY_H

#include "widgets/aoserverchat.h"

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QProgressBar>
#include <QTextBrowser>

#include <QDebug>
#include <QPushButton>
#include <QScrollBar>
#include <QStackedWidget>

class AOApplication;

class Lobby : public QMainWindow
{
  Q_OBJECT

public:
  Lobby(AOApplication *p_ao_app);

  void list_servers();
  void list_favorites();
  void append_chatmessage(QString f_name, QString f_message);
  void append_error(QString f_message);
  void set_player_count(int players_online, int max_players);
  void set_loading_text(QString p_text);
  void show_loading_overlay();
  void hide_loading_overlay();
  QString get_chatlog();
  int get_selected_server();
  void enable_connect_button();

  void set_loading_value(int p_value);

  bool public_servers_selected = true;

  ~Lobby();

private:
  AOApplication *ao_app;

  QImage *ui_background;

  QPushButton *ui_public_servers;
  QPushButton *ui_favorites;

  QPushButton *ui_refresh;
  QPushButton *ui_add_to_fav;
  QPushButton *ui_connect;

  QLabel *ui_version;
  QPushButton *ui_about;

  QListWidget *ui_server_list;

  QLabel *ui_player_count;
  QTextBrowser *ui_description;

  AOServerChat *ui_chat;

  QImage *ui_loading_background;
  QTextEdit *ui_loading_text;
  QProgressBar *ui_progress_bar;
  QPushButton *ui_cancel;

  QWidget *ui_server_list_page;
  QWidget *ui_loading_page;

  QStackedWidget *ui_stacked_widget;

private slots:
  void on_public_servers_clicked();
  void on_favorites_clicked();

  void on_refresh_released();
  void on_add_to_fav_released();
  void on_connect_released();
  void on_about_clicked();
  void on_server_list_currentRowChanged(int n_server);
  void on_chat_messageSent(QString name, QString message);
};

#endif // LOBBY_H
