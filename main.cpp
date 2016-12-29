#include <QApplication>

#include "lobby.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Lobby w;
  w.show();

  return a.exec();
}
