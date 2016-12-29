#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>

class Lobby : public QMainWindow
{
  Q_OBJECT

public:
  Lobby(QWidget *parent = nullptr);
  ~Lobby();
};

#endif // LOBBY_H
