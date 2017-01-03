#ifndef AOAPPLICATION_H
#define AOAPPLICATION_H

#include <QApplication>
#include <QMainWindow>

class NetworkManager;
class Lobby;

class AOApplication : public QApplication
{
public:
  AOApplication(int &argc, char **argv);
  ~AOApplication();

  NetworkManager *net_manager;
  Lobby *w_lobby;
  //T0D0: change to custom class "Courtroom"
  QMainWindow *w_courtroom;

  bool lobby_constructed = false;
  bool courtroom_constructed = false;

  void construct_lobby();
  void destruct_lobby();

  void construct_courtroom();
  void destruct_courtroom();
};

#endif // AOAPPLICATION_H
