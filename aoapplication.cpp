#include "aoapplication.h"

#include "lobby.h"
#include "courtroom.h"
#include "networkmanager.h"
#include "debug_functions.h"

AOApplication::AOApplication(int &argc, char **argv) : QApplication(argc, argv)
{
  net_manager = new NetworkManager(this);
  discord = new AttorneyOnline::Discord();
  QObject::connect(net_manager, SIGNAL(ms_connect_finished(bool, bool)),
                   SLOT(ms_connect_finished(bool, bool)));
}

AOApplication::~AOApplication()
{
  destruct_lobby();
  destruct_courtroom();
  delete discord;
}

void AOApplication::construct_lobby()
{
  if (lobby_constructed)
  {
    qDebug() << "W: lobby was attempted constructed when it already exists";
    return;
  }

  w_lobby = new Lobby(this);
  lobby_constructed = true;

  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  int x = (screenGeometry.width()-w_lobby->width()) / 2;
  int y = (screenGeometry.height()-w_lobby->height()) / 2;
  w_lobby->move(x, y);

  if(is_discord_enabled())
    discord->state_lobby();

  w_lobby->show();
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

  w_courtroom = new Courtroom(this);
  courtroom_constructed = true;

  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  int x = (screenGeometry.width()-w_courtroom->width()) / 2;
  int y = (screenGeometry.height()-w_courtroom->height()) / 2;
  w_courtroom->move(x, y);
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

QString AOApplication::get_version_string()
{
  return
  QString::number(RELEASE) + "." +
  QString::number(MAJOR_VERSION) + "." +
  QString::number(MINOR_VERSION);
}

void AOApplication::reload_theme()
{
  current_theme = read_theme();
}

void AOApplication::set_favorite_list()
{
  favorite_list = read_serverlist_txt();
}

QString AOApplication::get_current_char()
{
  if (courtroom_constructed)
    return w_courtroom->get_current_char();
  else
    return "";
}

void AOApplication::add_favorite_server(int p_server)
{
  if (p_server < 0 || p_server >= server_list.size())
    return;

  server_type fav_server = server_list.at(p_server);

  QString str_port = QString::number(fav_server.port);

  QString server_line = fav_server.ip + ":" + str_port + ":" + fav_server.name;

  write_to_serverlist_txt(server_line);
}

void AOApplication::server_disconnected()
{
  if (courtroom_constructed)
  {
    call_notice("Disconnected from server.");
    construct_lobby();
    destruct_courtroom();
  }
}

void AOApplication::loading_cancelled()
{
  destruct_courtroom();

  w_lobby->hide_loading_overlay();
}

void AOApplication::ms_connect_finished(bool connected, bool will_retry)
{
  if (connected)
  {
    AOPacket *f_packet = new AOPacket("ALL#%");
    send_ms_packet(f_packet);
  }
  else
  {
    if (will_retry)
    {
      w_lobby->append_error("Error connecting to master server. Will try again in "
                          + QString::number(net_manager->ms_reconnect_delay_ms / 1000.f) + " seconds.");
    }
    else
    {
      call_error("There was an error connecting to the master server.\n"
                 "We deploy multiple master servers to mitigate any possible downtime, "
                 "but the client appears to have exhausted all possible methods of finding "
                 "and connecting to one.\n"
                 "Please check your Internet connection and firewall, and please try again.");
    }
  }
}
