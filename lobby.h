#ifndef LOBBY_H
#define LOBBY_H

#include "aoimage.h"
#include "aobutton.h"
#include "aopacket.h"

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QLineEdit>

class AOApplication;

class Lobby : public QMainWindow
{
  Q_OBJECT

public:
  Lobby(AOApplication *parent);
  ~Lobby();

  void set_widgets();
  void list_servers();
  void list_favorites();
  void append_chatmessage(QString p_message_line);
  void set_player_count(int players_online, int max_players);

private:
  AOApplication *ao_app;

  AOImage *ui_background;

  AOButton *ui_public_servers;
  AOButton *ui_favorites;

  AOButton *ui_refresh;
  AOButton *ui_add_to_fav;
  AOButton *ui_connect;

  AOButton *ui_about;

  QListWidget *ui_server_list;

  QLabel *ui_player_count;
  QPlainTextEdit *ui_description;

  QPlainTextEdit *ui_chatbox;

  QLineEdit *ui_chatname;
  QLineEdit *ui_chatmessage;

  const int m_lobby_width = 517;
  const int m_lobby_height = 666;

  bool public_servers_selected = true;

private slots:
  void on_public_servers_clicked();
  void on_favorites_clicked();

  void on_refresh_pressed();
  void on_refresh_released();
  void on_add_to_fav_pressed();
  void on_add_to_fav_released();
  void on_connect_pressed();
  void on_connect_released();
  void on_about_clicked();
  void on_server_list_clicked(QModelIndex p_model);
  void on_chatfield_return_pressed();
};

#endif // LOBBY_H
