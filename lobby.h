#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QLabel>

class Lobby : public QMainWindow
{
  Q_OBJECT

public:
  Lobby(QWidget *parent = nullptr);

  void set_theme_images();

  ~Lobby();

private:
  QLabel *ui_background;
};

#endif // LOBBY_H
