#include "aoapplication.h"

#include "courtroom.h"
#include "lobby.h"
#include "networkmanager.h"
#include "options.h"
#include "widgets/aooptionsdialog.h"

#include <QMediaDevices>

static QtMessageHandler original_message_handler;
static AOApplication *message_handler_context;

void message_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  Q_EMIT message_handler_context->qt_log_message(type, context, msg);
  original_message_handler(type, context, msg);
}

AOApplication::AOApplication(QObject *parent)
    : QObject(parent)
{
  net_manager = new NetworkManager(this);

  discord = new AttorneyOnline::Discord();

  asset_lookup_cache.reserve(2048);

  message_handler_context = this;
  original_message_handler = qInstallMessageHandler(message_handler);
}

AOApplication::~AOApplication()
{
  destruct_lobby();
  destruct_courtroom();
  delete discord;
  qInstallMessageHandler(original_message_handler);
}

bool AOApplication::is_lobby_constructed()
{
  return w_lobby;
}

void AOApplication::construct_lobby()
{
  if (is_lobby_constructed())
  {
    qWarning() << "lobby was attempted constructed when it already exists";
    return;
  }

  w_lobby = new Lobby(this, net_manager);

  centerOrMoveWidgetOnPrimaryScreen(w_lobby);

  if (Options::getInstance().discordEnabled())
  {
    discord->state_lobby();
  }

  destruct_demo();

  w_lobby->show();
}

void AOApplication::destruct_lobby()
{
  if (!is_lobby_constructed())
  {
    qWarning() << "lobby was attempted destructed when it did not exist";
    return;
  }

  delete w_lobby;
  w_lobby = nullptr;
}

bool AOApplication::is_courtroom_constructed()
{
  return w_courtroom;
}

void AOApplication::construct_courtroom()
{
  if (is_courtroom_constructed())
  {
    qWarning() << "courtroom was attempted constructed when it already exists";
    return;
  }

  w_courtroom = new Courtroom(this);

  centerOrMoveWidgetOnPrimaryScreen(w_courtroom);

  if (demo_server)
  {
    QObject::connect(demo_server, &DemoServer::skip_timers, w_courtroom, &Courtroom::skip_clocks);
  }
  else
  {
    qWarning() << "demo server did not exist during courtroom construction";
  }
}

void AOApplication::destruct_courtroom()
{
  if (!is_courtroom_constructed())
  {
    qWarning() << "courtroom was attempted destructed when it did not exist";
    return;
  }

  delete w_courtroom;
  w_courtroom = nullptr;
}

bool AOApplication::is_demo_constructed()
{
  return demo_server;
}

void AOApplication::construct_demo()
{
  if (demo_server)
  {
    qWarning() << "DEMO server is already constructed, cannot construct again";
    return;
  }

  demo_server = new DemoServer(this);
}

void AOApplication::destruct_demo()
{
  if (!demo_server)
  {
    qWarning() << "DEMO server is not constructed, cannot destruct";
    return;
  }

  delete demo_server;
  demo_server = nullptr;
}

void AOApplication::reconstruct_demo()
{
  destruct_demo();
  construct_demo();
}

QString AOApplication::get_version_string()
{
  return QString::number(RELEASE) + "." + QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION);
}

QString AOApplication::find_image(QStringList p_list)
{
  QString image_path;
  for (const QString &path : p_list)
  {
    if (file_exists(path))
    {
      image_path = path;
      break;
    }
  }
  return image_path;
}

void AOApplication::server_disconnected()
{
  bool try_reconnect = false;
  if (is_courtroom_constructed())
  {
    try_reconnect = w_courtroom->isVisible();
    construct_lobby();
    destruct_courtroom();
  }
  Options::getInstance().setServerSubTheme(QString());

  if (try_reconnect && QMessageBox::question(nullptr,
                                             tr("Server Disconnected"),
                                             tr("Connection to the server has been lost. "
                                                "Do you want to reconnect?"),
                                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
  {
    net_manager->reconnect_to_last_server();
  }
}

void AOApplication::loading_cancelled()
{
  destruct_courtroom();
}

void AOApplication::call_settings_menu()
{
  AOOptionsDialog *l_dialog = new AOOptionsDialog(this);

  if (is_courtroom_constructed())
  {
    connect(l_dialog, &AOOptionsDialog::reloadThemeRequest, w_courtroom, &Courtroom::on_reload_theme_clicked);
  }

  if (is_lobby_constructed())
  {}
  l_dialog->exec();

  if (is_courtroom_constructed())
  {
    w_courtroom->playerList()->reloadPlayers();
  }

  delete l_dialog;
}

void AOApplication::server_connected()
{
  qInfo() << "Established connection to server.";

  destruct_courtroom();
  construct_courtroom();

  courtroom_loaded = false;
}

QAudioDevice AOApplication::currentAudioDevice() const
{
  QString pref = Options::getInstance().audioOutputDevice();
  if (pref != "default")
  {
    const QList<QAudioDevice> devices = QMediaDevices::audioOutputs();
    for (const QAudioDevice &dev : devices)
    {
      if (dev.description() == pref)
      {
        return dev;
      }
    }
  }
  return QMediaDevices::defaultAudioOutput();
}

bool AOApplication::pointExistsOnScreen(QPoint point)
{
  for (QScreen *screen : QApplication::screens())
  {
    if (screen->availableGeometry().contains(point))
    {
      return true;
    }
  }
  return false;
}

void AOApplication::centerOrMoveWidgetOnPrimaryScreen(QWidget *widget)
{
  auto point = Options::getInstance().windowPosition(widget->objectName());
  if (!Options::getInstance().restoreWindowPositionEnabled() || !point.has_value() || !pointExistsOnScreen(point.value()))
  {
    QRect geometry = QGuiApplication::primaryScreen()->geometry();
    int x = (geometry.width() - widget->width()) / 2;
    int y = (geometry.height() - widget->height()) / 2;
    widget->move(x, y);
  }
  else
  {
    widget->move(point->x(), point->y());
  }
}
