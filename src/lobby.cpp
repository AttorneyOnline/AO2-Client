#include "lobby.h"

#include "debug_functions.h"
#include "aoapplication.h"
#include "aosfxplayer.h"
#include "aouiloader.h"

#include <QVBoxLayout>
#include <aocharselect.h>

#include <network/legacyclient.h>

#include <widgets/courtroom.h>

using namespace AttorneyOnline;

Lobby::Lobby(AOApplication *p_ao_app) : QMainWindow(), ao_app(p_ao_app)
{
  AOUiLoader loader(this, ao_app);
  QFile uiFile(":/resource/ui/lobby.ui");
  uiFile.open(QFile::ReadOnly);
  QWidget *windowWidget = loader.load(&uiFile, this);
  QMetaObject::connectSlotsByName(this);

  QVBoxLayout *parentLayout = new QVBoxLayout;
  parentLayout->addWidget(windowWidget);
  setLayout(parentLayout);

  setWindowTitle("Attorney Online 2");
  setWindowIcon(QIcon(":/logo.png"));
  setFixedSize(windowWidget->size());

  FROM_UI(QPushButton, public_servers)
  FROM_UI(QPushButton, favorites)
  FROM_UI(QPushButton, refresh)
  FROM_UI(QPushButton, add_to_fav)
  FROM_UI(QPushButton, connect)
  FROM_UI(QLabel, version)
  FROM_UI(QPushButton, about)
  FROM_UI(QListWidget, server_list)
  FROM_UI(QLabel, player_count)
  FROM_UI(QTextBrowser, description)
  FROM_UI(AOServerChat, chat)
  FROM_UI(QLabel, loading_text)
  FROM_UI(QProgressBar, progress_bar)
  FROM_UI(QPushButton, cancel)

  FROM_UI(QWidget, server_list_page)
  FROM_UI(QWidget, loading_page)

  FROM_UI(QStackedWidget, stacked_widget)

  ui_chat->set_name(options.oocName());
  ui_version->setText("Version: " + ao_app->get_version_string());

  // connect(ui_cancel, SIGNAL(clicked()), ao_app, SLOT(loading_cancelled()));

  showPublicServers();
}

void Lobby::showLoading()
{
  ui_stacked_widget->setCurrentWidget(ui_loading_page);
}

void Lobby::hideLoading()
{
  ui_stacked_widget->setCurrentWidget(ui_server_list_page);
}

server_type Lobby::selectedServer()
{
  int row = ui_server_list->currentRow();

  Q_ASSERT(row >= 0);

  if (publicServersSelected)
    return servers[row];
  else
    return favorites[row];
}

void Lobby::set_loading_value(int p_value)
{
  ui_progress_bar->setValue(p_value);
}

void Lobby::on_public_servers_clicked()
{
  if (ui_favorites->isChecked())
    ui_favorites->setChecked(false);
  else
    ui_public_servers->setChecked(true);

  showPublicServers();

  publicServersSelected = true;
}

void Lobby::on_favorites_clicked()
{
  if (ui_public_servers->isChecked())
    ui_public_servers->setChecked(false);
  else
    ui_favorites->setChecked(true);

  showFavorites();

  publicServersSelected = false;
}

void Lobby::on_refresh_released()
{
  if (publicServersSelected)
    emit msRefreshRequested();
  else
    showFavorites();
}

void Lobby::on_add_to_fav_released()
{
  //you cant add favorites from favorites m8
  if (!publicServersSelected)
    return;

  options.addToFavoriteServers(selectedServer());
}

void Lobby::on_connect_released()
{
  const server_type server = selectedServer();
  client = std::make_shared<LegacyClient>(nullptr);
  showLoading();
  QObject::connect(client.get(), &Client::connectProgress,
                   this, &Lobby::onConnectProgress);
  client->connect(server.ip, static_cast<uint16_t>(server.port)).then([&] {
    auto courtroom = new Courtroom(ao_app, client);

    // XXX: is this necessary?
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - courtroom->width()) / 2;
    int y = (screenGeometry.height() - courtroom->height()) / 2;
    courtroom->move(x, y);

    courtroom->show();
    hide();

    courtroom->chooseCharacter();

    deleteLater();
  });
}

void Lobby::on_about_clicked()
{
  QString msg = tr("<h2>Attorney Online %1</h2>"
                   "The courtroom drama simulator"
                   "<p><b>Source code:</b> "
                   "<a href='https://github.com/AttorneyOnline/AO2-Client'>"
                   "https://github.com/AttorneyOnline/AO2-Client</a>"
                   "<p><b>Major development:</b><br>"
                   "OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter"
                   "<p><b>Special thanks:</b><br>"
                   "Remy, Iamgoofball, Hibiki, Qubrick (webAO), Ruekasu (UI design), "
                   "Draxirch (UI design), Unishred, Argoneus (tsuserver), Fiercy, "
                   "Noevain, Cronnicossy")
          .arg(ao_app->get_version_string());
  QMessageBox::about(this, "About", msg);
}

void Lobby::on_server_list_currentRowChanged(int row)
{
  server_type server;

  if (row < 0)
    return;

  if (publicServersSelected)
    server = servers[row];
  else
    server = favorites[row];

  ui_description->setText(server.desc);

  ui_description->moveCursor(QTextCursor::Start);
  ui_description->ensureCursorVisible();

  ui_player_count->setText("Offline");

  ui_connect->setEnabled(false);

  // Ignore callbacks for player count
  client->disconnect(this);

  client = std::make_shared<LegacyClient>(nullptr);
  client->connect(server.ip, static_cast<uint16_t>(server.port), true)
      .then([&] {
    qDebug() << "Client probe promise resolved";
    std::pair<int, int> players = client->playerCount();
    ui_player_count->setText(tr("Online: %1/%2")
                             .arg(players.first)
                             .arg(players.second));
    ui_connect->setEnabled(true);
  });
}

void Lobby::on_chat_messageSent(QString name, QString message)
{
  //no you can't send empty messages
  if (message.isEmpty())
    return;

  emit chatSent(name, message);
}

void Lobby::showPublicServers()
{
  publicServersSelected = true;

  ui_server_list->clear();

  // Implicit copy
  servers = ao_app->msConnection()->servers();

  for (const server_type &i_server : servers)
  {
    ui_server_list->addItem(i_server.name);
  }
}

void Lobby::showFavorites()
{
  publicServersSelected = false;

  ui_server_list->clear();

  // Implicit copy
  favorites = options.favoriteServers();

  for (const server_type &i_server : favorites)
  {
    ui_server_list->addItem(i_server.name);
  }
}

void Lobby::appendChat(QString f_name, QString f_message)
{
  ui_chat->append_chat_message(f_name, f_message, ao_app->get_color("ooc_default_color", "courtroom_design.ini").name());
}

void Lobby::appendError(QString f_message)
{
  ui_chat->append_error(f_message);
}

void Lobby::onConnectProgress(int current, int max, const QString &message)
{
  ui_loading_text->setText(message);
  ui_progress_bar->setValue(current);
  ui_progress_bar->setMaximum(max);
}

Lobby::~Lobby()
{

}
