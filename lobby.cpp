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

  ui_public_servers->set_image("publicservers.png");
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

  //clear server list and show public servers
}

/*
void Lobby::on_refresh_pressed()
{
  ui->refresh->setStyleSheet(get_stylesheet_path("refresh_pressed.png"));
}

void Lobby::on_refresh_released()
{
  ui->refresh->setStyleSheet(get_stylesheet_path("refresh.png"));

  all_servers_requested();
}

void Lobby::on_addtofav_pressed()
{
  ui->addtofav->setStyleSheet(get_stylesheet_path("addtofav_pressed.png"));
}

void Lobby::on_addtofav_released()
{
  ui->addtofav->setStyleSheet(get_stylesheet_path("addtofav.png"));

  //T0D0, add selected element to serverlist.txt

}

void Lobby::on_connect_pressed()
{
  ui->connect->setStyleSheet(get_stylesheet_path("connect_pressed.png"));
}

void Lobby::on_connect_released()
{
  ui->connect->setStyleSheet(get_stylesheet_path("connect.png"));

  enter_server_requested();
}
*/

