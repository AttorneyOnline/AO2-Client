#include "aoapplication.h"

#include "lobby.h"
#include "widgets/courtroom.h"
#include "networkmanager.h"
#include "debug_functions.h"

#include "widgets/aooptionsdialog.h"
#include "widgets/aocaseannouncerdialog.h"

AOApplication::AOApplication(int &argc, char **argv) : QApplication(argc, argv)
{
  // Create the QSettings class that points to the config.ini.
  configini = new QSettings(get_base_path() + "config.ini", QSettings::IniFormat);

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

  if (options.discordEnabled())
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

void AOApplication::reloadTheme()
{
  currentTheme = options.theme();
}

void AOApplication::add_favorite_server(int p_server)
{
  if (p_server < 0 || p_server >= server_list.size())
    return;

  options.addToFavoriteServers(server_list[p_server]);
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
      if (w_lobby != nullptr)
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

void AOApplication::call_settings_menu()
{
    AOOptionsDialog settings(nullptr, this);
    settings.exec();
}

void AOApplication::call_announce_menu(Courtroom *court)
{
    AOCaseAnnouncerDialog announcer(nullptr, this, court);
    announcer.exec();
}
