#include "lobby.h"

#include "path_functions.h"
#include "text_file_functions.h"
#include "global_variables.h"
#include "debug_functions.h"
#include "aoapplication.h"
#include "networkmanager.h"

#include <QDebug>

Lobby::Lobby(AOApplication *parent) : QMainWindow()
{
  ao_app = parent;

  this->setWindowTitle("Attorney Online 2");
  this->resize(m_lobby_width, m_lobby_height);
  this->setFixedSize(m_lobby_width, m_lobby_height);

  ui_background = new AOImage(this);
  ui_public_servers = new AOButton(this);
  ui_favorites = new AOButton(this);
  ui_refresh = new AOButton(this);
  ui_add_to_fav = new AOButton(this);
  ui_connect = new AOButton(this);
  ui_about = new AOButton(this);
  ui_server_list = new QListWidget(this);
  ui_player_count = new QLabel(this);
  ui_description = new QPlainTextEdit(this);
  ui_chatbox = new QPlainTextEdit(this);
  ui_chatname = new QLineEdit(this);
  ui_chatmessage = new QLineEdit(this);

  connect(ui_public_servers, SIGNAL(clicked()), this, SLOT(on_public_servers_clicked()));
  connect(ui_favorites, SIGNAL(clicked()), this, SLOT(on_favorites_clicked()));
  connect(ui_refresh, SIGNAL(pressed()), this, SLOT(on_refresh_pressed()));
  connect(ui_refresh, SIGNAL(released()), this, SLOT(on_refresh_released()));
  connect(ui_add_to_fav, SIGNAL(pressed()), this, SLOT(on_add_to_fav_pressed()));
  connect(ui_add_to_fav, SIGNAL(released()), this, SLOT(on_add_to_fav_released()));
  connect(ui_connect, SIGNAL(pressed()), this, SLOT(on_connect_pressed()));
  connect(ui_connect, SIGNAL(released()), this, SLOT(on_connect_released()));
  connect(ui_about, SIGNAL(clicked()), this, SLOT(on_about_clicked()));
  connect(ui_server_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_server_list_clicked(QModelIndex)));
  connect(ui_chatmessage, SIGNAL(returnPressed()), this, SLOT(on_chatfield_return_pressed()));

  set_widgets();
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

  ui_about->set_image("about.png");
  ui_about->move(428, 1);
  ui_about->resize(88, 21);

  ui_server_list->move(20, 125);
  ui_server_list->resize(286, 240);
  ui_server_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "font: bold;");

  ui_player_count->move(336, 91);
  ui_player_count->resize(173, 16);
  ui_player_count->setText("Offline");
  ui_player_count->setStyleSheet("font: bold;"
                                 "color: white;"
                                 "qproperty-alignment: AlignCenter;");

  ui_description->move(337, 109);
  ui_description->resize(173, 245);
  ui_description->setReadOnly(true);
  ui_description->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "color: white;");

  ui_chatbox->move(2, 445);
  ui_chatbox->resize(515, 198);
  ui_chatbox->setReadOnly(true);
  ui_chatbox->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  ui_chatname->move(3, 646);
  ui_chatname->resize(85, 19);
  ui_chatname->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                             "selection-background-color: rgba(0, 0, 0, 0);");

  ui_chatmessage->move(93, 646);
  ui_chatmessage->resize(424, 19);
  ui_chatmessage->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "selection-background-color: rgba(0, 0, 0, 0);");

}

void Lobby::on_public_servers_clicked()
{
  ui_public_servers->set_image("publicservers_selected.png");
  ui_favorites->set_image("favorites.png");

  list_servers();

  public_servers_selected = true;
}

void Lobby::on_favorites_clicked()
{
  ui_favorites->set_image("favorites_selected.png");
  ui_public_servers->set_image("publicservers.png");

  ao_app->favorite_list = read_serverlist_txt();

  list_favorites();

  public_servers_selected = false;
}

void Lobby::on_refresh_pressed()
{
  ui_refresh->set_image("refresh_pressed.png");
}

void Lobby::on_refresh_released()
{
  ui_refresh->set_image("refresh.png");

  AOPacket *f_packet = new AOPacket("ALL#%");

  ao_app->send_ms_packet(f_packet);

  delete f_packet;
}

void Lobby::on_add_to_fav_pressed()
{
  ui_add_to_fav->set_image("addtofav_pressed.png");
}

void Lobby::on_add_to_fav_released()
{
  ui_add_to_fav->set_image("addtofav.png");

  //you cant add favorites from favorites m8
  if (!public_servers_selected)
    return;

  int n_server = ui_server_list->currentRow();

  if (n_server < 0 || n_server >= ao_app->server_list.size())
    return;

  server_type fav_server = ao_app->server_list.at(n_server);

  QString str_port = QString::number(fav_server.port);

  QString server_line = fav_server.ip + ":" + str_port + ":" + fav_server.name;

  write_to_serverlist_txt(server_line);
}

void Lobby::on_connect_pressed()
{
  ui_connect->set_image("connect_pressed.png");
}

void Lobby::on_connect_released()
{
  ui_connect->set_image("connect.png");

  //T0D0: call ao_app to initialize loading sequence
}

void Lobby::on_about_clicked()
{
  //T0D0: add something real here
  call_error("YEBOIIII");
}

void Lobby::on_server_list_clicked(QModelIndex p_model)
{
  server_type f_server;
  int n_server = p_model.row();

  if (n_server < 0)
    return;

  if (public_servers_selected)
  {
    if (n_server >= ao_app->server_list.size())
      return;

    f_server = ao_app->server_list.at(p_model.row());
  }
  else
  {
    if (n_server >= ao_app->favorite_list.size())
      return;

    f_server = ao_app->favorite_list.at(p_model.row());
  }

  ui_description->clear();
  ui_description->appendPlainText(f_server.desc);

  ui_description->moveCursor(QTextCursor::Start);
  ui_description->ensureCursorVisible();

  ui_player_count->setText("Offline");

  ao_app->net_manager->connect_to_server(f_server);
}

void Lobby::on_chatfield_return_pressed()
{
  QString f_header = "CT";
  QStringList f_contents{ui_chatname->text(), ui_chatmessage->text()};

  AOPacket *f_packet = new AOPacket(f_header, f_contents);

  ao_app->send_ms_packet(f_packet);

  ui_chatmessage->clear();

  delete f_packet;
}

void Lobby::list_servers()
{
  ui_server_list->clear();

  for (server_type i_server : ao_app->server_list)
  {
    ui_server_list->addItem(i_server.name);
  }
}

void Lobby::list_favorites()
{
  ui_server_list->clear();

  for (server_type i_server : ao_app->favorite_list)
  {
    ui_server_list->addItem(i_server.name);
  }
}

void Lobby::append_chatmessage(QString p_message_line)
{
  ui_chatbox->appendPlainText(p_message_line);
}

void Lobby::set_player_count(int players_online, int max_players)
{
  QString f_string = "Online: " + QString::number(players_online) + "/" + QString::number(max_players);
  ui_player_count->setText(f_string);
}

Lobby::~Lobby()
{
  delete ui_background;
  delete ui_public_servers;
  delete ui_favorites;
  delete ui_refresh;
  delete ui_add_to_fav;
  delete ui_connect;
  delete ui_about;
  delete ui_server_list;
  delete ui_player_count;
  delete ui_description;
  delete ui_chatbox;
  delete ui_chatname;
  delete ui_chatmessage;
}
