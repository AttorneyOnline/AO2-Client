#include "aoapplication.h"

#include "courtroom.h"
#include "debug_functions.h"
#include "lobby.h"
#include "networkmanager.h"

#include "aocaseannouncerdialog.h"
#include "aooptionsdialog.h"

AOApplication::AOApplication(int &argc, char **argv) : QApplication(argc, argv)
{
  // Create the QSettings class that points to the config.ini.
  configini =
      new QSettings(get_base_path() + "config.ini", QSettings::IniFormat);

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
  if (lobby_constructed) {
    qDebug() << "W: lobby was attempted constructed when it already exists";
    return;
  }

  w_lobby = new Lobby(this);
  lobby_constructed = true;

  QRect geometry = QGuiApplication::primaryScreen()->geometry();
  int x = (geometry.width() - w_lobby->width()) / 2;
  int y = (geometry.height() - w_lobby->height()) / 2;
  w_lobby->move(x, y);

  if (is_discord_enabled())
    discord->state_lobby();

  if (demo_server)
      demo_server->deleteLater();
  demo_server = new DemoServer();

  w_lobby->show();
}

void AOApplication::destruct_lobby()
{
  if (!lobby_constructed) {
    qDebug() << "W: lobby was attempted destructed when it did not exist";
    return;
  }

  delete w_lobby;
  w_lobby = nullptr;
  lobby_constructed = false;
}

void AOApplication::construct_courtroom()
{
  if (courtroom_constructed) {
    qDebug() << "W: courtroom was attempted constructed when it already exists";
    return;
  }

  w_courtroom = new Courtroom(this);
  courtroom_constructed = true;

  QRect geometry = QGuiApplication::primaryScreen()->geometry();
  int x = (geometry.width() - w_courtroom->width()) / 2;
  int y = (geometry.height() - w_courtroom->height()) / 2;
  w_courtroom->move(x, y);
}

void AOApplication::destruct_courtroom()
{
  if (!courtroom_constructed) {
    qDebug() << "W: courtroom was attempted destructed when it did not exist";
    return;
  }

  delete w_courtroom;
  w_courtroom = nullptr;
  courtroom_constructed = false;
}

QString AOApplication::get_version_string()
{
  return QString::number(RELEASE) + "." + QString::number(MAJOR_VERSION) + "." +
         QString::number(MINOR_VERSION);
}

void AOApplication::reload_theme() { current_theme = read_theme(); }

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
  if (courtroom_constructed) {
    call_notice(tr("Disconnected from server."));
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
  if (connected) {
    AOPacket *f_packet = new AOPacket("ALL#%");
    send_ms_packet(f_packet);
  }
  else {
    if (will_retry) {
      if (lobby_constructed)
        w_lobby->append_error(
            tr("Error connecting to master server. Will try again in %1 "
               "seconds.")
                .arg(QString::number(net_manager->ms_reconnect_delay)));
    }
    else {
      call_error(tr("There was an error connecting to the master server.\n"
                    "We deploy multiple master servers to mitigate any "
                    "possible downtime, "
                    "but the client appears to have exhausted all possible "
                    "methods of finding "
                    "and connecting to one.\n"
                    "Please check your Internet connection and firewall, and "
                    "please try again."));
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

// Callback for when BASS device is lost
void CALLBACK AOApplication::BASSreset(HSTREAM handle, DWORD channel,
                                       DWORD data, void *user)
{
  doBASSreset();
}

void AOApplication::doBASSreset()
{
  BASS_Free();
  BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
  load_bass_opus_plugin();
}

void AOApplication::initBASS()
{
  BASS_Free();
  // Change the default audio output device to be the one the user has given
  // in his config.ini file for now.
  unsigned int a = 0;
  BASS_DEVICEINFO info;

  if (get_audio_output_device() == "default") {
    BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
    load_bass_opus_plugin();
  }
  else {
    for (a = 0; BASS_GetDeviceInfo(a, &info); a++) {
      if (get_audio_output_device() == info.name) {
        BASS_SetDevice(a);
        BASS_Init(static_cast<int>(a), 48000, BASS_DEVICE_LATENCY, nullptr,
                  nullptr);
        load_bass_opus_plugin();
        qDebug() << info.name << "was set as the default audio output device.";
        return;
      }
    }
    BASS_Init(-1, 48000, BASS_DEVICE_LATENCY, nullptr, nullptr);
    load_bass_opus_plugin();
  }
}

#if (defined(_WIN32) || defined(_WIN64))
void AOApplication::load_bass_opus_plugin()
{
  BASS_PluginLoad("bassopus.dll", 0);
}
#elif (defined(LINUX) || defined(__linux__))
void AOApplication::load_bass_opus_plugin()
{
  BASS_PluginLoad("libbassopus.so", 0);
}
#elif defined __APPLE__
void AOApplication::load_bass_opus_plugin()
{
  BASS_PluginLoad("libbassopus.dylib", 0);
}
#else
#error This operating system is unsupported for BASS plugins.
#endif
