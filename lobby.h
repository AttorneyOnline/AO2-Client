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

  void set_theme_images();

  ~Lobby();

private:
  AOImage *ui_background;
  AOButton *ui_public_servers;
  AOButton *ui_favorites;
};

#endif // LOBBY_H
