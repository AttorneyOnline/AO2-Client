#include "lobby.h"

#include "debug_functions.h"
#include "aoapplication.h"
#include "networkmanager.h"
#include "aosfxplayer.h"
#include "aouiloader.h"

#include <QVBoxLayout>

Lobby::Lobby(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

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

  ui_background = findChild<AOImage *>("background");
  ui_public_servers = findChild<AOButton *>("public_servers");
  ui_favorites = findChild<AOButton *>("favorites");
  ui_refresh = findChild<AOButton *>("refresh");
  ui_add_to_fav = findChild<AOButton *>("add_to_fav");
  ui_connect = findChild<AOButton *>("connect");
  ui_version = findChild<QLabel *>("version");
  ui_about = findChild<AOButton *>("about");
  ui_server_list = findChild<QListWidget *>("server_list");
  ui_player_count = findChild<QLabel *>("player_count");
  ui_description = findChild<AOTextArea *>("description");
  ui_chatbox = findChild<AOTextArea *>("chatbox");
  ui_chatname = findChild<QLineEdit *>("chatname");
  ui_chatmessage = findChild<QLineEdit *>("chatmessage");
  ui_loading_background = findChild<AOImage *>("loading_background");
  ui_loading_text = findChild<QTextEdit *>("loading_text");
  ui_progress_bar = findChild<QProgressBar *>("progress_bar");
  ui_cancel = findChild<AOButton *>("cancel");

  ui_server_list_page = findChild<QWidget *>("server_list_page");
  ui_loading_page = findChild<QWidget *>("loading_page");

  ui_stacked_widget = findChild<QStackedWidget *>("stacked_widget");

  ui_chatname->setText(ao_app->get_ooc_name());
  ui_version->setText("Version: " + ao_app->get_version_string());

  connect(ui_server_list, SIGNAL(clicked(QModelIndex)), this, SLOT(on_server_list_clicked(QModelIndex)));
  connect(ui_cancel, SIGNAL(clicked()), ao_app, SLOT(loading_cancelled()));

  ao_app->reload_theme();
  list_servers();
}

void Lobby::set_loading_text(QString p_text)
{
  ui_loading_text->clear();
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->append(p_text);
}

void Lobby::show_loading_overlay()
{
  ui_stacked_widget->setCurrentWidget(ui_loading_page);
}

void Lobby::hide_loading_overlay()
{
  ui_stacked_widget->setCurrentWidget(ui_server_list_page);
}

QString Lobby::get_chatlog()
{
  QString return_value = ui_chatbox->toPlainText();

  return return_value;
}

int Lobby::get_selected_server()
{
  return ui_server_list->currentRow();
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

  list_servers();

  public_servers_selected = true;
}

void Lobby::on_favorites_clicked()
{
  if (ui_public_servers->isChecked())
    ui_public_servers->setChecked(false);
  else
    ui_favorites->setChecked(true);

  ao_app->set_favorite_list();
  //ao_app->favorite_list = read_serverlist_txt();

  list_favorites();

  public_servers_selected = false;
}

void Lobby::on_refresh_released()
{
  AOPacket *f_packet = new AOPacket("ALL#%");

  ao_app->send_ms_packet(f_packet);
}

void Lobby::on_add_to_fav_released()
{
  //you cant add favorites from favorites m8
  if (!public_servers_selected)
    return;

  ao_app->add_favorite_server(ui_server_list->currentRow());
}

void Lobby::on_connect_released()
{
  AOPacket *f_packet;

  f_packet = new AOPacket("askchaa#%");

  ao_app->send_server_packet(f_packet);
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

void Lobby::on_server_list_currentRowChanged(int n_server)
{
  server_type f_server;

  if (n_server < 0)
    return;

  if (public_servers_selected)
  {
    QVector<server_type> f_server_list = ao_app->get_server_list();

    if (n_server >= f_server_list.size())
      return;

    f_server = f_server_list.at(n_server);
  }
  else
  {
    QVector<server_type> f_favorites_list = ao_app->get_favorite_list();

    if (n_server >= f_favorites_list.size())
      return;

    f_server = f_favorites_list.at(n_server);
  }

  ui_description->clear();
  ui_description->append(f_server.desc);

  ui_description->moveCursor(QTextCursor::Start);
  ui_description->ensureCursorVisible();

  ui_player_count->setText("Offline");

  ui_connect->setEnabled(false);

  ao_app->net_manager->connect_to_server(f_server);
}

void Lobby::on_chatfield_returnPressed()
{
  //no you can't send empty messages
  if (ui_chatname->text() == "" || ui_chatmessage->text() == "")
    return;

  QString f_header = "CT";
  QStringList f_contents{ui_chatname->text(), ui_chatmessage->text()};

  AOPacket *f_packet = new AOPacket(f_header, f_contents);

  ao_app->send_ms_packet(f_packet);

  ui_chatmessage->clear();
}

void Lobby::list_servers()
{
  public_servers_selected = true;

  ui_server_list->clear();

  for (server_type i_server : ao_app->get_server_list())
  {
    ui_server_list->addItem(i_server.name);
  }
}

void Lobby::list_favorites()
{
  ui_server_list->clear();

  for (server_type i_server : ao_app->get_favorite_list())
  {
    ui_server_list->addItem(i_server.name);
  }
}

void Lobby::append_chatmessage(QString f_name, QString f_message)
{
  ui_chatbox->append_chatmessage(f_name, f_message, ao_app->get_color("ooc_default_color", "courtroom_design.ini").name());
}

void Lobby::append_error(QString f_message)
{
  ui_chatbox->append_error(f_message);
}

void Lobby::set_player_count(int players_online, int max_players)
{
  QString f_string = "Online: " + QString::number(players_online) + "/" + QString::number(max_players);
  ui_player_count->setText(f_string);
}

void Lobby::enable_connect_button()
{
  ui_connect->setEnabled(true);
}

Lobby::~Lobby()
{

}
