#include <QApplication>

#include "lobby.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  Lobby w;
  w.set_theme_images();
  w.show();

  return a.exec();
}
