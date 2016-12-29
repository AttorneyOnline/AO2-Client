#include <QDebug>

#include "path_functions.h"

#include "lobby.h"

Lobby::Lobby(QWidget *parent) : QMainWindow(parent)
{
  this->resize(517, 666);
  ui_background = new QLabel(this);
}

void Lobby::set_theme_images()
{
  QString theme_path = get_theme_path();

  ui_background->move(0, 0);
  ui_background->resize(517, 666);
  ui_background->setPixmap(theme_path + "lobbybackground.png");
}

Lobby::~Lobby()
{

}
