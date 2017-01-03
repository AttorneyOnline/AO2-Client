#ifndef AOAPPLICATION_H
#define AOAPPLICATION_H

#include <QApplication>
#include <QMainWindow>

#include "lobby.h"

class AOApplication : public QApplication
{
public:
  AOApplication(int &argc, char **argv);
  ~AOApplication();

  Lobby *w_lobby;
  QMainWindow *w_courtroom;

  bool lobby_constructed = false;
  bool courtroom_constructed = false;

  void construct_lobby();
  void destruct_lobby();

  void construct_courtroom();
  void destruct_courtroom();
};

#endif // AOAPPLICATION_H
