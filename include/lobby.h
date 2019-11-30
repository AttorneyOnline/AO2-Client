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

  void showPublicServers();
  void showFavorites();
  void appendChat(QString f_name, QString f_message);
  void appendError(QString f_message);
  void showLoading();
  void hideLoading();
  server_type selectedServer();

  void set_loading_value(int p_value);

  bool publicServersSelected = true;

  ~Lobby();

private:
  AOApplication *ao_app;
  Options options;

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

  QLabel *ui_loading_text;
  QProgressBar *ui_progress_bar;
  QPushButton *ui_cancel;

  QWidget *ui_server_list_page;
  QWidget *ui_loading_page;

  QStackedWidget *ui_stacked_widget;

  QVector<server_type> servers;
  QVector<server_type> favorites;

  std::shared_ptr<AttorneyOnline::Client> client;

private slots:
  void on_public_servers_clicked();
  void on_favorites_clicked();

  void on_refresh_released();
  void on_add_to_fav_released();
  void on_connect_released();
  void on_about_clicked();
  void on_server_list_currentRowChanged(int n_server);
  void on_chat_messageSent(QString name, QString message);

  void onConnectProgress(int current, int max, const QString &message);
signals:
  void msRefreshRequested();
  void chatSent(const QString &name, const QString &message);
};

#endif // LOBBY_H
