#include "lobby.h"

#include "debug_functions.h"
#include "aoapplication.h"
#include "networkmanager.h"
#include "aosfxplayer.h"

#include <QDebug>
#include <QScrollBar>

Lobby::Lobby(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

  this->setWindowTitle("Attorney Online 2");
  this->setFixedSize(m_lobby_width, m_lobby_height);

  ui_background = new AOImage(this, ao_app);
  ui_public_servers = new AOButton(this, ao_app);
  ui_favorites = new AOButton(this, ao_app);
  ui_refresh = new AOButton(this, ao_app);
  ui_add_to_fav = new AOButton(this, ao_app);
  ui_connect = new AOButton(this, ao_app);
  ui_version = new QLabel(this);
  ui_about = new AOButton(this, ao_app);
  ui_server_list = new QListWidget(this);
  ui_player_count = new QLabel(this);
  ui_description = new QPlainTextEdit(this);
  ui_chatbox = new QTextBrowser(this);
  ui_chatname = new QLineEdit(this);
  ui_chatname->setPlaceholderText("Name");
  ui_chatmessage = new QLineEdit(this);
  ui_loading_background = new AOImage(this, ao_app);
  ui_loading_text = new QTextEdit(ui_loading_background);
  ui_progress_bar = new QProgressBar(ui_loading_background);
  ui_progress_bar->setMinimum(0);
  ui_progress_bar->setMaximum(100);
  ui_progress_bar->setStyleSheet("QProgressBar{ color: white; }");
  ui_cancel = new AOButton(ui_loading_background, ao_app);

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
  connect(ui_cancel, SIGNAL(clicked()), ao_app, SLOT(loading_cancelled()));

  set_widgets();
}

//sets images, position and size
void Lobby::set_widgets()
{
  ao_app->set_user_theme();

  ui_background->resize(m_lobby_width, m_lobby_height);
  ui_background->set_image("lobbybackground.png");

  set_size_and_pos(ui_public_servers, "public_servers");
  ui_public_servers->set_image("publicservers_selected.png");

  set_size_and_pos(ui_favorites, "favorites");
  ui_favorites->set_image("favorites.png");

  set_size_and_pos(ui_refresh, "refresh");
  ui_refresh->set_image("refresh.png");

  set_size_and_pos(ui_add_to_fav, "add_to_fav");
  ui_add_to_fav->set_image("addtofav.png");

  set_size_and_pos(ui_connect, "connect");
  ui_connect->set_image("connect.png");

  set_size_and_pos(ui_version, "version");
  ui_version->setText("Version: " + ao_app->get_version_string());

  set_size_and_pos(ui_about, "about");
  ui_about->set_image("about.png");

  set_size_and_pos(ui_server_list, "server_list");
  ui_server_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                  "font: bold;");

  set_size_and_pos(ui_player_count, "player_count");
  ui_player_count->setText("Offline");
  ui_player_count->setStyleSheet("font: bold;"
                                 "color: white;"
                                 "qproperty-alignment: AlignCenter;");

  set_size_and_pos(ui_description, "description");
  ui_description->setReadOnly(true);
  ui_description->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "color: white;");

  set_size_and_pos(ui_chatbox, "chatbox");
  ui_chatbox->setReadOnly(true);
  ui_chatbox->setStyleSheet("QTextBrowser{background-color: rgba(0, 0, 0, 0);}");

  set_size_and_pos(ui_chatname, "chatname");
  ui_chatname->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                             "selection-background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_chatmessage, "chatmessage");
  ui_chatmessage->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "selection-background-color: rgba(0, 0, 0, 0);");

  ui_loading_background->resize(m_lobby_width, m_lobby_height);
  ui_loading_background->set_image("loadingbackground.png");


  set_size_and_pos(ui_loading_text, "loading_label");
  ui_loading_text->setFont(QFont("Arial", 20, QFont::Bold));
  ui_loading_text->setReadOnly(true);
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->setFrameStyle(QFrame::NoFrame);
  ui_loading_text->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: rgba(255, 128, 0, 255);");
  ui_loading_text->append("Loading");

  set_size_and_pos(ui_progress_bar, "progress_bar");
  set_size_and_pos(ui_cancel, "cancel");
  ui_cancel->setText("Cancel");

  ui_loading_background->hide();

}

void Lobby::set_size_and_pos(QWidget *p_widget, QString p_identifier)
{
  QString filename = "lobby_design.ini";

  pos_size_type design_ini_result = ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0)
  {
    qDebug() << "W: could not find " << p_identifier << " in " << filename;
    p_widget->hide();
  }
  else
  {
    p_widget->move(design_ini_result.x, design_ini_result.y);
    p_widget->resize(design_ini_result.width, design_ini_result.height);
  }
}

void Lobby::set_loading_text(QString p_text)
{
  ui_loading_text->clear();
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->append(p_text);
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
  ui_public_servers->set_image("publicservers_selected.png");
  ui_favorites->set_image("favorites.png");

  list_servers();

  public_servers_selected = true;
}

void Lobby::on_favorites_clicked()
{
  ui_favorites->set_image("favorites_selected.png");
  ui_public_servers->set_image("publicservers.png");

  ao_app->set_favorite_list();
  //ao_app->favorite_list = read_serverlist_txt();

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
}

void Lobby::on_add_to_fav_pressed()
{
  ui_add_to_fav->set_image("addtofav_pressed.png");
}

void Lobby::on_add_to_fav_released()
{
  ao_app->server_packet_received(new AOPacket("AO2CHECK#2.1.1#%"));

  ui_add_to_fav->set_image("addtofav.png");

  //you cant add favorites from favorites m8
  if (!public_servers_selected)
    return;

  ao_app->add_favorite_server(ui_server_list->currentRow());
}

void Lobby::on_connect_pressed()
{
  ui_connect->set_image("connect_pressed.png");
}

void Lobby::on_connect_released()
{
  ui_connect->set_image("connect.png");

  AOPacket *f_packet = new AOPacket("askchaa#%");
  ao_app->send_server_packet(f_packet);
}

void Lobby::on_about_clicked()
{
  call_notice("Attorney Online 2 is built using Qt 5.7\n\n"
              "Lead development:\n"
              "OmniTroid\n\n"
              "Supporting development:\n"
              "stonedDiscord\n"
              "Fiercy\n\n"
              "UI design:\n"
              "Ruekasu\n"
              "Draxirch\n\n"
              "Special thanks:\n"
              "Unishred\n"
              "Argoneus\n"
              "Noevain\n"
              "Cronnicossy");
}

void Lobby::on_server_list_clicked(QModelIndex p_model)
{
  server_type f_server;
  int n_server = p_model.row();

  if (n_server < 0)
    return;

  if (public_servers_selected)
  {
    QVector<server_type> f_server_list = ao_app->get_server_list();

    if (n_server >= f_server_list.size())
      return;

    f_server = f_server_list.at(p_model.row());
  }
  else
  {
    if (n_server >= ao_app->get_favorite_list().size())
      return;

    f_server = ao_app->get_favorite_list().at(p_model.row());
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
  ui_favorites->set_image("favorites.png");
  ui_public_servers->set_image("publicservers_selected.png");

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

void Lobby::append_chatmessage(QString f_message)
{
  const QTextCursor old_cursor = ui_chatbox->textCursor();
  const int old_scrollbar_value = ui_chatbox->verticalScrollBar()->value();
  const bool is_scrolled_down = old_scrollbar_value == ui_chatbox->verticalScrollBar()->maximum();

  ui_chatbox->moveCursor(QTextCursor::End);

  QStringList word_list = f_message.split(" ");

  for (QString i_word : word_list)
  {
    if (i_word.startsWith("http"))
      ui_chatbox->insertHtml("<a href=\"" + i_word + "\">" + i_word + "</a> ");
    else
      ui_chatbox->insertPlainText(i_word + " ");
  }

  //ui_ms_chatlog->append(f_message);
  ui_chatbox->insertPlainText("\n");

  if (old_cursor.hasSelection() || !is_scrolled_down)
  {
      // The user has selected text or scrolled away from the bottom: maintain position.
      ui_chatbox->setTextCursor(old_cursor);
      ui_chatbox->verticalScrollBar()->setValue(old_scrollbar_value);
  }
  else
  {
      // The user hasn't selected any text and the scrollbar is at the bottom: scroll to the bottom.
      ui_chatbox->moveCursor(QTextCursor::End);
      ui_chatbox->verticalScrollBar()->setValue(ui_chatbox->verticalScrollBar()->maximum());
  }
}

void Lobby::set_player_count(int players_online, int max_players)
{
  QString f_string = "Online: " + QString::number(players_online) + "/" + QString::number(max_players);
  ui_player_count->setText(f_string);
}

Lobby::~Lobby()
{

}
