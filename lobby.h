#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QListWidget>

#include "aoimage.h"
#include "aobutton.h"

class AOApplication;

class Lobby : public QMainWindow
{
  Q_OBJECT

public:
  Lobby(AOApplication *parent);
  ~Lobby();

  void set_widgets();

private:
  const int m_lobby_width = 517;
  const int m_lobby_height = 666;

  AOApplication *m_parent;

  AOImage *ui_background;

  AOButton *ui_public_servers;
  AOButton *ui_favorites;

  AOButton *ui_refresh;
  AOButton *ui_add_to_fav;
  AOButton *ui_connect;

  AOButton *ui_about;

  QListWidget *ui_server_list;
//  QListWidget

public slots:
  void on_public_servers_clicked();
  void on_favorites_clicked();

  void on_refresh_pressed();
  void on_refresh_released();
  void on_add_to_fav_pressed();
  void on_add_to_fav_released();
  void on_connect_pressed();
  void on_connect_released();

  void on_about_clicked();
};

#endif // LOBBY_H
