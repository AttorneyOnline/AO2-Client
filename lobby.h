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
#include <QProgressBar>

class AOApplication;

class Lobby : public QMainWindow
{
  Q_OBJECT

public:
  Lobby(AOApplication *p_ao_app);

  void set_widgets();
  void list_servers();
  void list_favorites();
  void append_chatmessage(QString f_message);
  void set_player_count(int players_online, int max_players);
  void set_loading_text(QString p_text);
  void show_loading_overlay(){ui_loading_background->show();}
  void hide_loading_overlay(){ui_loading_background->hide();}
  QString get_chatlog();
  int get_selected_server();

  void set_loading_value(int p_value);

  bool public_servers_selected = true;

  ~Lobby();

private:
  AOApplication *ao_app;

  const int m_lobby_width = 517;
  const int m_lobby_height = 666;

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

  AOImage *ui_loading_background;
  QTextEdit *ui_loading_text;
  QProgressBar *ui_progress_bar;
  AOButton *ui_cancel;

  void set_size_and_pos(QWidget *p_widget, QString p_identifier);

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
