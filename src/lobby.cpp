#include "lobby.h"

#include "aoapplication.h"
#include "aosfxplayer.h"
#include "debug_functions.h"
#include "networkmanager.h"

#include <QImageReader>

Lobby::Lobby(AOApplication *p_ao_app) : QMainWindow()
{
  ao_app = p_ao_app;

  this->setWindowTitle(tr("Attorney Online 2"));
  this->setWindowIcon(QIcon(":/logo.png"));
  this->setWindowFlags( (this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  ui_background = new AOImage(this, ao_app);
  ui_public_servers = new AOButton(this, ao_app);
  ui_favorites = new AOButton(this, ao_app);
  ui_refresh = new AOButton(this, ao_app);
  ui_add_to_fav = new AOButton(this, ao_app);
  ui_connect = new AOButton(this, ao_app);
  ui_version = new QLabel(this);
  ui_about = new AOButton(this, ao_app);
  ui_settings = new AOButton(this, ao_app);

  ui_server_list = new QTreeWidget(this);
  ui_server_list->setHeaderLabels({"#", "Name"}); //, "Players"});
  ui_server_list->hideColumn(0);

  ui_server_search = new QLineEdit(this);
  ui_server_search->setFrame(false);
  ui_server_search->setPlaceholderText(tr("Search"));

  ui_player_count = new QLabel(this);
  ui_description = new AOTextArea(this);
  ui_description->setOpenExternalLinks(true);
  ui_chatbox = new AOTextArea(this);
  ui_chatbox->setOpenExternalLinks(true);
  ui_chatname = new QLineEdit(this);
  ui_chatname->setPlaceholderText(tr("Name"));
  ui_chatname->setText(ao_app->get_ooc_name());
  ui_chatmessage = new QLineEdit(this);
  ui_loading_background = new AOImage(this, ao_app);
  ui_loading_text = new QTextEdit(ui_loading_background);
  ui_progress_bar = new QProgressBar(ui_loading_background);
  ui_progress_bar->setMinimum(0);
  ui_progress_bar->setMaximum(100);
  ui_progress_bar->setStyleSheet("QProgressBar{ color: white; }");
  ui_cancel = new AOButton(ui_loading_background, ao_app);

  connect(ui_public_servers, SIGNAL(clicked()), this,
          SLOT(on_public_servers_clicked()));
  connect(ui_favorites, SIGNAL(clicked()), this, SLOT(on_favorites_clicked()));
  connect(ui_refresh, SIGNAL(pressed()), this, SLOT(on_refresh_pressed()));
  connect(ui_refresh, SIGNAL(released()), this, SLOT(on_refresh_released()));
  connect(ui_add_to_fav, SIGNAL(pressed()), this,
          SLOT(on_add_to_fav_pressed()));
  connect(ui_add_to_fav, SIGNAL(released()), this,
          SLOT(on_add_to_fav_released()));
  connect(ui_connect, SIGNAL(pressed()), this, SLOT(on_connect_pressed()));
  connect(ui_connect, SIGNAL(released()), this, SLOT(on_connect_released()));
  connect(ui_about, SIGNAL(clicked()), this, SLOT(on_about_clicked()));
  connect(ui_settings, SIGNAL(clicked()), this, SLOT(on_settings_clicked()));
  connect(ui_server_list, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this,
          SLOT(on_server_list_clicked(QTreeWidgetItem *, int)));
  connect(ui_server_list, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
          this, SLOT(on_server_list_doubleclicked(QTreeWidgetItem *, int)));
  connect(ui_server_search, SIGNAL(textChanged(QString)), this,
          SLOT(on_server_search_edited(QString)));
  connect(ui_chatmessage, SIGNAL(returnPressed()), this,
          SLOT(on_chatfield_return_pressed()));
  connect(ui_cancel, SIGNAL(clicked()), ao_app, SLOT(loading_cancelled()));

  ui_connect->setEnabled(false);

  list_servers();

  set_widgets();
}

// sets images, position and size
void Lobby::set_widgets()
{
  ao_app->reload_theme();

  QString filename = "lobby_design.ini";

  pos_size_type f_lobby = ao_app->get_element_dimensions("lobby", filename);

  if (f_lobby.width < 0 || f_lobby.height < 0) {
    qDebug() << "W: did not find lobby width or height in " << filename;

    // Most common symptom of bad config files and missing assets.
    call_notice(
        tr("It doesn't look like your client is set up correctly.\n"
           "Did you download all resources correctly from tiny.cc/getao, "
           "including the large 'base' folder?"));

    this->setFixedSize(517, 666);
  }
  else {
    this->setFixedSize(f_lobby.width, f_lobby.height);
  }

  set_size_and_pos(ui_background, "lobby");
  ui_background->set_image("lobbybackground");

  set_size_and_pos(ui_public_servers, "public_servers");
  ui_public_servers->set_image("publicservers_selected");

  set_size_and_pos(ui_favorites, "favorites");
  ui_favorites->set_image("favorites");

  set_size_and_pos(ui_refresh, "refresh");
  ui_refresh->set_image("refresh");

  set_size_and_pos(ui_add_to_fav, "add_to_fav");
  ui_add_to_fav->set_image("addtofav");

  set_size_and_pos(ui_connect, "connect");
  ui_connect->set_image("connect");

  set_size_and_pos(ui_version, "version");
  ui_version->setText(tr("Version: %1").arg(ao_app->get_version_string()));

  set_size_and_pos(ui_about, "about");
  ui_about->set_image("about");

  set_size_and_pos(ui_settings, "settings");
  ui_settings->setText(tr("Settings"));
  ui_settings->set_image("settings");
  ui_settings->setToolTip(
      tr("Allows you to change various aspects of the client."));

  set_size_and_pos(ui_server_list, "server_list");
  ui_server_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "font: bold;");

  set_size_and_pos(ui_server_search, "server_search");
  ui_server_search->setStyleSheet("background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_player_count, "player_count");
  ui_player_count->setText(tr("Offline"));
  ui_player_count->setStyleSheet("font: bold;"
                                 "color: white;"
                                 "qproperty-alignment: AlignCenter;");

  set_size_and_pos(ui_description, "description");
  ui_description->setReadOnly(true);
  ui_description->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "color: white;");

  set_size_and_pos(ui_chatbox, "chatbox");
  ui_chatbox->setReadOnly(true);
  ui_chatbox->setStyleSheet(
      "QTextBrowser{background-color: rgba(0, 0, 0, 0);}");

  set_size_and_pos(ui_chatname, "chatname");
  ui_chatname->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                             "selection-background-color: rgba(0, 0, 0, 0);");

  set_size_and_pos(ui_chatmessage, "chatmessage");
  ui_chatmessage->setStyleSheet(
      "background-color: rgba(0, 0, 0, 0);"
      "selection-background-color: rgba(0, 0, 0, 0);");

  ui_loading_background->resize(this->width(), this->height());
  ui_loading_background->set_image("loadingbackground");

  set_size_and_pos(ui_loading_text, "loading_label");
  ui_loading_text->setFont(QFont("Arial", 20, QFont::Bold));
  ui_loading_text->setReadOnly(true);
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->setFrameStyle(QFrame::NoFrame);
  ui_loading_text->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: rgba(255, 128, 0, 255);");
  ui_loading_text->append(tr("Loading"));

  set_size_and_pos(ui_progress_bar, "progress_bar");
  set_size_and_pos(ui_cancel, "cancel");
  ui_cancel->setText(tr("Cancel"));

  ui_loading_background->hide();

  set_fonts();
  set_stylesheets();
}

void Lobby::set_size_and_pos(QWidget *p_widget, QString p_identifier)
{
  QString filename = "lobby_design.ini";

  pos_size_type design_ini_result =
      ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0) {
    qDebug() << "W: could not find " << p_identifier << " in " << filename;
    p_widget->hide();
  }
  else {
    p_widget->move(design_ini_result.x, design_ini_result.y);
    p_widget->resize(design_ini_result.width, design_ini_result.height);
  }
}

void Lobby::set_fonts()
{
  set_font(ui_player_count, "player_count");
  set_font(ui_description, "description");
  set_font(ui_chatbox, "chatbox");
  set_font(ui_chatname, "chatname");
  set_font(ui_chatmessage, "chatmessage");
  set_font(ui_loading_text, "loading_text");
  set_font(ui_server_list, "server_list");
}

void Lobby::set_stylesheet(QWidget *widget, QString target_tag)
{
  QString f_file = "lobby_stylesheets.css";
  QString style_sheet_string =
      ao_app->get_tagged_stylesheet(target_tag, f_file);
  if (style_sheet_string != "")
    widget->setStyleSheet(style_sheet_string);
}

void Lobby::set_stylesheets()
{
  set_stylesheet(ui_player_count, "[PLAYER COUNT]");
  set_stylesheet(ui_description, "[DESCRIPTION]");
  set_stylesheet(ui_chatbox, "[CHAT BOX]");
  set_stylesheet(ui_chatname, "[CHAT NAME]");
  set_stylesheet(ui_chatmessage, "[CHAT MESSAGE]");
  set_stylesheet(ui_loading_text, "[LOADING TEXT]");
  set_stylesheet(ui_server_list, "[SERVER LIST]");
}

void Lobby::set_font(QWidget *widget, QString p_identifier)
{
  QString design_file = "lobby_fonts.ini";
  int f_weight = ao_app->get_font_size(p_identifier, design_file);
  QString class_name = widget->metaObject()->className();
  QString font_name =
      ao_app->get_font_name(p_identifier + "_font", design_file);
  QFont font(font_name, f_weight);
  bool use = ao_app->get_font_size("use_custom_fonts", design_file) == 1;
  if (use) {
    bool bold = ao_app->get_font_size(p_identifier + "_bold", design_file) ==
                1; // is the font bold or not?
    font.setBold(bold);
    widget->setFont(font);
    QColor f_color = ao_app->get_color(p_identifier + "_color", design_file);
    bool center =
        ao_app->get_font_size(p_identifier + "_center", design_file) ==
        1; // should it be centered?
    QString is_center = "";
    if (center)
      is_center = "qproperty-alignment: AlignCenter;";
    QString style_sheet_string =
        class_name + " { background-color: rgba(0, 0, 0, 0);\n" +
        "color: rgba(" + QString::number(f_color.red()) + ", " +
        QString::number(f_color.green()) + ", " +
        QString::number(f_color.blue()) + ", 255);\n" + is_center + "}";
    widget->setStyleSheet(style_sheet_string);
  }
  return;
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
  return ui_server_list->currentItem()->text(0).toInt();
}

void Lobby::set_loading_value(int p_value)
{
  ui_progress_bar->setValue(p_value);
}

void Lobby::on_public_servers_clicked()
{
  ui_public_servers->set_image("publicservers_selected");
  ui_favorites->set_image("favorites");

  list_servers();

  public_servers_selected = true;
}

void Lobby::on_favorites_clicked()
{
  ui_favorites->set_image("favorites_selected");
  ui_public_servers->set_image("publicservers");

  ao_app->set_favorite_list();
  // ao_app->favorite_list = read_serverlist_txt();

  list_favorites();

  public_servers_selected = false;
}

void Lobby::on_refresh_pressed() { ui_refresh->set_image("refresh_pressed"); }

void Lobby::on_refresh_released()
{
  ui_refresh->set_image("refresh");

  AOPacket *f_packet = new AOPacket("ALL#%");

  ao_app->send_ms_packet(f_packet);
}

void Lobby::on_add_to_fav_pressed()
{
  ui_add_to_fav->set_image("addtofav_pressed");
}

void Lobby::on_add_to_fav_released()
{
  ui_add_to_fav->set_image("addtofav");

  // you cant add favorites from favorites m8
  if (!public_servers_selected)
    return;

  ao_app->add_favorite_server(get_selected_server());
}

void Lobby::on_connect_pressed() { ui_connect->set_image("connect_pressed"); }

void Lobby::on_connect_released()
{
  ui_connect->set_image("connect");

  AOPacket *f_packet;

  f_packet = new AOPacket("askchaa#%");

  ao_app->send_server_packet(f_packet);
}

void Lobby::on_about_clicked()
{
  const bool hasApng = QImageReader::supportedImageFormats().contains("apng");

  QString msg =
      tr("<h2>Attorney Online %1</h2>"
         "The courtroom drama simulator"
         "<p><b>Source code:</b> "
         "<a href='https://github.com/AttorneyOnline/AO2-Client'>"
         "https://github.com/AttorneyOnline/AO2-Client</a>"
         "<p><b>Major development:</b><br>"
         "OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter, "
         "Crystalwarrior, Iamgoofball"
         "<p><b>Client development:</b><br>"
         "Cents02, in1tiate, raidensnake, windrammer"
         "<p><b>QA testing:</b><br>"
         "CaseyCazy, CedricDewitt, Chewable Tablets, CrazyJC, Fantos, "
         "Fury McFlurry, Geck, Gin-Gi, Jamania, Minx, Pandae, "
         "Robotic Overlord, Shadowlions (aka Shali), Sierra, SomeGuy, "
         "Veritas, Wiso"
         "<p><b>Special thanks:</b><br>"
         "CrazyJC (2.8 release director) and MaximumVolty (2.8 release promotion); "
         "Remy, Hibiki, court-records.net (sprites); Qubrick (webAO); "
         "Rue (website); Draxirch (UI design); "
         "Lewdton and Argoneus (tsuserver); "
         "Fiercy, Noevain, Cronnicossy, and FanatSors (AO1); "
         "server hosts, game masters, case makers, content creators, "
         "and the whole AO2 community!"
         "<p>The Attorney Online networked visual novel project "
         "is copyright (c) 2016-2020 Attorney Online developers. Open-source "
         "licenses apply. All other assets are the property of their "
         "respective owners."
         "<p>Running on Qt version %2 with the BASS audio engine.<br>"
         "APNG plugin loaded: %3"
         "<p>Built on %4")
      .arg(ao_app->get_version_string())
      .arg(QLatin1String(QT_VERSION_STR))
      .arg(hasApng ? tr("Yes") : tr("No"))
      .arg(QLatin1String(__DATE__));
  QMessageBox::about(this, tr("About"), msg);
}

void Lobby::on_settings_clicked() { ao_app->call_settings_menu(); }

// clicked on an item in the serverlist
void Lobby::on_server_list_clicked(QTreeWidgetItem *p_item, int column)
{
  column = 0;
  if (p_item->text(column).toInt() != last_index || !public_servers_selected) {
    server_type f_server;
    int n_server = p_item->text(column).toInt();
    last_index = n_server;

    if (n_server < 0)
      return;

    if (public_servers_selected) {
      QVector<server_type> f_server_list = ao_app->get_server_list();

      if (n_server >= f_server_list.size())
        return;

      f_server = f_server_list.at(n_server);
    }
    else {
      if (n_server >= ao_app->get_favorite_list().size())
        return;

      f_server = ao_app->get_favorite_list().at(n_server);
    }

    ui_description->clear();
    ui_description->append_linked(f_server.desc);

    ui_description->moveCursor(QTextCursor::Start);
    ui_description->ensureCursorVisible();

    ui_player_count->setText(tr("Offline"));

    ui_connect->setEnabled(false);

    ao_app->net_manager->connect_to_server(f_server);
  }
}

// doubleclicked on an item in the serverlist so we'll connect right away
void Lobby::on_server_list_doubleclicked(QTreeWidgetItem *p_item, int column)
{
  doubleclicked = true;
  on_server_list_clicked(p_item, column);
  //on_connect_released();
}

void Lobby::on_server_search_edited(QString p_text)
{
  // Iterate through all QTreeWidgetItem items
  QTreeWidgetItemIterator it(ui_server_list);
  while (*it) {
    (*it)->setHidden(p_text != "");
    ++it;
  }

  if (p_text != "") {
    // Search in metadata
    QList<QTreeWidgetItem *> clist = ui_server_list->findItems(
        ui_server_search->text(), Qt::MatchContains | Qt::MatchRecursive, 1);
    foreach (QTreeWidgetItem *item, clist) {
      if (item->parent() != nullptr) // So the category shows up too
        item->parent()->setHidden(false);
      item->setHidden(false);
    }
  }
}

void Lobby::on_chatfield_return_pressed()
{
  // no you can't send empty messages
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
  ui_favorites->set_image("favorites");
  ui_public_servers->set_image("publicservers_selected");

  ui_server_list->setSortingEnabled(false);
  ui_server_list->clear();

  ui_server_search->setText("");

  int i = 0;
  for (server_type i_server : ao_app->get_server_list()) {
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui_server_list);
    treeItem->setData(0, Qt::DisplayRole, i);
    treeItem->setText(1, i_server.name);
    i++;
  }
  ui_server_list->setSortingEnabled(true);
  ui_server_list->sortItems(0, Qt::SortOrder::AscendingOrder);
}

void Lobby::list_favorites()
{
  ui_server_list->setSortingEnabled(false);
  ui_server_list->clear();

  int i = 0;
  for (server_type i_server : ao_app->get_favorite_list()) {
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui_server_list);
    treeItem->setData(0, Qt::DisplayRole, i);
    treeItem->setText(1, i_server.name);
    //    treeItem->setText(2, "-");
    i++;
  }
  ui_server_list->setSortingEnabled(true);
}

void Lobby::append_chatmessage(QString f_name, QString f_message)
{
  ui_chatbox->append_chatmessage(
      f_name, f_message,
      ao_app->get_color("ooc_default_color", "courtroom_design.ini").name());
}

void Lobby::append_error(QString f_message)
{
  ui_chatbox->append_error(f_message);
}

void Lobby::set_player_count(int players_online, int max_players)
{
  QString f_string = tr("Online: %1/%2")
                         .arg(QString::number(players_online))
                         .arg(QString::number(max_players));
  ui_player_count->setText(f_string);
}

void Lobby::enable_connect_button() { ui_connect->setEnabled(true); }

Lobby::~Lobby() {}
