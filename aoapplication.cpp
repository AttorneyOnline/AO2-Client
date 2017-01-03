#include <QDebug>

#include "aoapplication.h"

AOApplication::AOApplication(int &argc, char **argv) : QApplication(argc, argv)
{

}

AOApplication::~AOApplication()
{

}

void AOApplication::construct_lobby()
{
  if (lobby_constructed)
  {
    qDebug() << "W: lobby was attempted constructed when it already exists";
    return;
  }

  w_lobby = new Lobby();
  lobby_constructed = true;
}

void AOApplication::destruct_lobby()
{
  if(!lobby_constructed)
  {
    qDebug() << "W: lobby was attempted destructed when it did not exist";
    return;
  }

  delete w_lobby;
  lobby_constructed = false;
}

void AOApplication::construct_courtroom()
{
  if (courtroom_constructed)
  {
    qDebug() << "W: courtroom was attempted constructed when it already exists";
    return;
  }

  w_courtroom = new QMainWindow();
  courtroom_constructed = true;
}

void AOApplication::destruct_courtroom()
{
  if (!courtroom_constructed)
  {
    qDebug() << "W: courtroom was attempted destructed when it did not exist";
    return;
  }

  delete w_courtroom;
  courtroom_constructed = false;
}


