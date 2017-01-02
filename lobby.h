#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include "aoimage.h"
#include "aobutton.h"

class Lobby : public QMainWindow
{
  Q_OBJECT

public:
  Lobby(QWidget *parent = nullptr);
  ~Lobby();

  void set_widgets();

private:
  const int m_lobby_width = 517;
  const int m_lobby_height = 666;

  AOImage *ui_background;

  AOButton *ui_public_servers;
  AOButton *ui_favorites;

  AOButton *ui_refresh;
  AOButton *ui_add_to_fav;
  AOButton *ui_connect;

public slots:
  void on_public_servers_clicked();
};

#endif // LOBBY_H
