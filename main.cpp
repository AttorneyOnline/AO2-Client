#include <QDebug>

#include "aoapplication.h"

#include "lobby.h"

int main(int argc, char *argv[])
{
  AOApplication main_app(argc, argv);
  main_app.construct_lobby();
  main_app.w_lobby->show();

  return main_app.exec();
}
