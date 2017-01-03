#include <QDebug>

#include "path_functions.h"
#include "text_file_functions.h"
#include "global_variables.h"

#include "lobby.h"

Lobby::Lobby(QWidget *parent) : QMainWindow(parent)
{
  this->setWindowTitle("Attorney Online 2");
  this->resize(m_lobby_width, m_lobby_height);

  ui_background = new AOImage(this);
  ui_public_servers = new AOButton(this);
  ui_favorites = new AOButton(this);
  ui_refresh = new AOButton(this);
  ui_add_to_fav = new AOButton(this);
  ui_connect = new AOButton(this);

  connect(ui_public_servers, SIGNAL(clicked()), this, SLOT(on_public_servers_clicked()));
  connect(ui_favorites, SIGNAL(clicked()), this, SLOT(on_favorites_clicked()));

  connect(ui_refresh, SIGNAL(pressed()), this, SLOT(on_refresh_pressed()));
  connect(ui_refresh, SIGNAL(released()), this, SLOT(on_refresh_released()));
  connect(ui_add_to_fav, SIGNAL(pressed()), this, SLOT(on_add_to_fav_pressed()));
  connect(ui_add_to_fav, SIGNAL(released()), this, SLOT(on_add_to_fav_released()));
  connect(ui_connect, SIGNAL(pressed()), this, SLOT(on_connect_pressed()));
  connect(ui_connect, SIGNAL(released()), this, SLOT(on_connect_released()));

  set_widgets();
}

Lobby::~Lobby()
{
  delete ui_background;
}

//sets images, position and size
void Lobby::set_widgets()
{
  //global to efficiently set images on button presses
  g_user_theme = get_user_theme();

  ui_background->set_image("lobbybackground.png");
  ui_background->move(0, 0);
  ui_background->resize(m_lobby_width, m_lobby_height);

  ui_public_servers->set_image("publicservers_selected.png");
  ui_public_servers->move(46, 88);
  ui_public_servers->resize(114, 30);

  ui_favorites->set_image("favorites.png");
  ui_favorites->move(164, 88);
  ui_favorites->resize(114, 30);

  ui_refresh->set_image("refresh.png");
  ui_refresh->move(56, 381);
  ui_refresh->resize(132, 28);

  ui_add_to_fav->set_image("addtofav.png");
  ui_add_to_fav->move(194, 381);
  ui_add_to_fav->resize(132, 28);

  ui_connect->set_image("connect.png");
  ui_connect->move(332, 381);
  ui_connect->resize(132, 28);
}

void Lobby::on_public_servers_clicked()
{
  ui_public_servers->set_image("publicservers_selected.png");
  ui_favorites->set_image("favorites.png");

  //T0D0: clear server list and show public servers
}

void Lobby::on_favorites_clicked()
{
  ui_favorites->set_image("favorites_selected.png");
  ui_public_servers->set_image("publicservers.png");

  //T0D0: clear server list and show favorites from serverlist.txt
}

void Lobby::on_refresh_pressed()
{
  ui_refresh->set_image("refresh_pressed.png");
}

void Lobby::on_refresh_released()
{
  ui_refresh->set_image("refresh.png");

  //T0D0: clear serverlist, request new list from MS and show them
}

void Lobby::on_add_to_fav_pressed()
{
  ui_add_to_fav->set_image("addtofav_pressed.png");
}

void Lobby::on_add_to_fav_released()
{
  ui_add_to_fav->set_image("addtofav.png");

  //T0D0, add selected element to serverlist.txt
}

void Lobby::on_connect_pressed()
{
  ui_connect->set_image("connect_pressed.png");
}

void Lobby::on_connect_released()
{
  ui_connect->set_image("connect.png");

  //T0D0: connect to selected server(show loading overlay?)
}

