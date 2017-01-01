#include <QDebug>

#include "path_functions.h"
#include "text_file_functions.h"
#include "global_variables.h"

#include "lobby.h"

Lobby::Lobby(QWidget *parent) : QMainWindow(parent)
{
  const int lobby_width = 517;
  const int lobby_height = 666;

  this->resize(lobby_width, lobby_height);

  ui_background = new AOImage(this, 0, 0, lobby_width, lobby_height);
}

void Lobby::set_theme_images()
{
  g_user_theme = get_user_theme();

  ui_background->set_image("lobbybackground.png");
  ui_public_servers->set_image("publicservers.png");
  ui_favorites->set_image("favorites.png");
}

Lobby::~Lobby()
{
  delete ui_background;
}
