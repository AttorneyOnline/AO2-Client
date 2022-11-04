#include "lobby.h"

#include "aoapplication.h"
#include "aosfxplayer.h"
#include "debug_functions.h"
#include "demoserver.h"
#include "networkmanager.h"

#include <QAction>
#include <QImageReader>
#include <QMenu>
#include <QUiLoader>

#define FROM_UI(type, name)                                                    \
  ;                                                                            \
  ui_##name = findChild<type *>(#name);

Lobby::Lobby(AOApplication *p_ao_app, NetworkManager *p_net_manager) : QMainWindow()
{
  ao_app = p_ao_app;
  net_manager = p_net_manager;

  this->setWindowTitle(tr("Attorney Online %1").arg(ao_app->applicationVersion()));
  this->setWindowIcon(QIcon(":/logo.png"));
  this->setWindowFlags( (this->windowFlags() | Qt::CustomizeWindowHint));

  QUiLoader l_loader(this);
  QFile l_uiFile(":/resource/ui/lobby.ui");
  if (!l_uiFile.open(QFile::ReadOnly)) {
    qCritical() << "Unable to open file " << l_uiFile.fileName();
    return;
  }

  l_loader.load(&l_uiFile, this);

  FROM_UI(QLabel,game_version_lbl);
  ui_game_version_lbl->setText(tr("Version: %1").arg(ao_app->get_version_string()));

  FROM_UI(QPushButton,settings_button);
  connect(ui_settings_button, &QPushButton::clicked, this, &Lobby::on_settings_clicked);

  FROM_UI(QPushButton,about_button);
  connect(ui_about_button, &QPushButton::clicked, this, &Lobby::on_about_clicked);

  //Serverlist elements
  FROM_UI(QTabWidget,connections_tabview);
  ui_connections_tabview->tabBar()->setExpanding(true);
  connect(ui_connections_tabview, &QTabWidget::currentChanged, this, &Lobby::on_tab_changed);

  FROM_UI(QTreeWidget,serverlist_tree);
  FROM_UI(QLineEdit, serverlist_search);
  connect(ui_serverlist_tree, &QTreeWidget::itemClicked, this,
          &Lobby::on_server_list_clicked);
  connect(ui_serverlist_tree, &QTreeWidget::itemDoubleClicked,
          this, &Lobby::on_list_doubleclicked);
  connect(ui_serverlist_search, &QLineEdit::textChanged, this,
          &Lobby::on_server_search_edited);

  FROM_UI(QTreeWidget, favorites_tree);
  connect(ui_favorites_tree, &QTreeWidget::customContextMenuRequested, this,
          &Lobby::on_favorite_list_context_menu_requested);

  FROM_UI(QLineEdit, favorites_search);

  FROM_UI(QTreeWidget, demo_tree);
  connect(ui_demo_tree, &QTreeWidget::itemClicked, this, &Lobby::on_demo_clicked);

  FROM_UI(QLineEdit, demo_search);

  FROM_UI(QPushButton, refresh_button);
  connect(ui_refresh_button, &QPushButton::pressed, this, &Lobby::on_refresh_pressed);
  connect(ui_refresh_button, &QPushButton::released, this, &Lobby::on_refresh_released);

  FROM_UI(QPushButton, add_to_favorite_button)
  connect(ui_add_to_favorite_button, &QPushButton::pressed, this, &Lobby::on_add_to_fav_pressed);
  connect(ui_add_to_favorite_button, &QPushButton::released, this, &Lobby::on_add_to_fav_released);


  FROM_UI(QPushButton, remove_from_favorites_button)
  ui_remove_from_favorites_button->setVisible(false);
  connect(ui_remove_from_favorites_button, &QPushButton::pressed, this, &Lobby::on_remove_from_fav_pressed);
  connect(ui_remove_from_favorites_button, &QPushButton::released, this, &Lobby::on_remove_from_fav_released);

  FROM_UI(QLabel, server_player_count_lbl)
  FROM_UI(QTextBrowser, server_description_text)
  FROM_UI(QPushButton, connect_button);
  connect(ui_connect_button, &QPushButton::pressed, this, &Lobby::on_connect_pressed);
  connect(ui_connect_button, &QPushButton::released, this, &Lobby::on_connect_released);

  FROM_UI(QTextBrowser, motd_text);

  ui_loading_background = new AOImage(this, ao_app);
  ui_loading_background->setObjectName("ui_loading_background");
  ui_loading_text = new QTextEdit(ui_loading_background);
  ui_loading_text->setObjectName("ui_loading_text");
  ui_progress_bar = new QProgressBar(ui_loading_background);
  ui_progress_bar->setMinimum(0);
  ui_progress_bar->setMaximum(100);
  ui_progress_bar->setObjectName("ui_progress_bar");
  ui_cancel = new AOButton(ui_loading_background, ao_app);
  ui_cancel->setObjectName("ui_cancel");
  connect(ui_cancel, &AOButton::clicked, ao_app, &AOApplication::loading_cancelled);

  list_servers();
  list_favorites();
  list_demos();
  get_motd();
  check_for_updates();

  set_widgets();
}

// sets images, position and size
void Lobby::set_widgets()
{
  ui_loading_background->resize(this->width(), this->height());
  ui_loading_background->set_image("loadingbackground");

  set_size_and_pos(ui_loading_text, "loading_label");
  ui_loading_text->setFont(QFont("Arial", 20, QFont::Bold));
  ui_loading_text->setReadOnly(true);
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->setFrameStyle(QFrame::NoFrame);
  ui_loading_text->append(tr("Loading"));

  set_size_and_pos(ui_progress_bar, "progress_bar");
  set_size_and_pos(ui_cancel, "cancel");
  ui_cancel->setText(tr("Cancel"));

  ui_loading_background->hide();
}

void Lobby::set_size_and_pos(QWidget *p_widget, QString p_identifier)
{
  QString filename = "lobby_design.ini";

  pos_size_type design_ini_result =
      ao_app->get_element_dimensions(p_identifier, filename);

  if (design_ini_result.width < 0 || design_ini_result.height < 0) {
    qWarning() << "could not find" << p_identifier << "in" << filename;
    p_widget->hide();
  }
  else {
    p_widget->move(design_ini_result.x, design_ini_result.y);
    p_widget->resize(design_ini_result.width, design_ini_result.height);
  }
}

void Lobby::on_tab_changed(int index)
{
    switch (index) { //Implicit conversion cause FUCK ALL OF YOU.
    case SERVER:
        ui_add_to_favorite_button->setVisible(true);
        ui_remove_from_favorites_button->setVisible(false);
        break;
    case FAVORITES:
        ui_add_to_favorite_button->setVisible(false);
        ui_remove_from_favorites_button->setVisible(true);
        break;
    case DEMOS :
        ui_add_to_favorite_button->setVisible(false);
        ui_remove_from_favorites_button->setVisible(false);
        break;
    default:
        break;
    }
}

void Lobby::set_loading_text(QString p_text)
{
  ui_loading_text->clear();
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->append(p_text);
}

int Lobby::get_selected_server()
{
    switch (ui_connections_tabview->currentIndex()) {
    case SERVER:
        if (auto item = ui_serverlist_tree->currentItem()) {
          return item->text(0).toInt();
        }
        break;
    case FAVORITES:
        if (auto item = ui_favorites_tree->currentItem()) {
          return item->text(0).toInt();
        }
        break;
    default:
        break;
    }
  return -1;
}

void Lobby::set_loading_value(int p_value)
{
  ui_progress_bar->setValue(p_value);
}

void Lobby::reset_selection()
{
  last_index = -1;
  ui_serverlist_tree->clearSelection();
  ui_server_player_count_lbl->setText(tr("Offline"));
  ui_server_description_text->clear();

  ui_connect_button->setEnabled(false);
}

void Lobby::on_refresh_pressed() { /**ui_refresh->set_image("refresh_pressed");*/ }

void Lobby::on_refresh_released()
{
    net_manager->get_server_list(std::bind(&Lobby::list_servers, this));
    get_motd();
    ao_app->load_favorite_list();
    list_favorites();
}

void Lobby::on_add_to_fav_pressed()
{
}

void Lobby::on_add_to_fav_released()
{
  int selection = get_selected_server();
  if (selection > -1) {
    ao_app->add_favorite_server(selection);
  }
}

void Lobby::on_remove_from_fav_pressed()
{
  //ui_remove_from_favorites_button->set_image("removefromfav_pressed");
}

void Lobby::on_remove_from_fav_released()
{
  int selection = get_selected_server();
  if (selection >= 0) {
    ao_app->remove_favorite_server(selection);
    list_favorites();
  }
}

void Lobby::on_connect_pressed() { }

void Lobby::on_connect_released()
{

  AOPacket *f_packet;

  f_packet = new AOPacket("askchaa");

  ao_app->send_server_packet(f_packet);
}

void Lobby::on_about_clicked()
{
  const bool hasApng = QImageReader::supportedImageFormats().contains("apng");
  QString msg =
      tr("<h2>Attorney Online %1</h2>"
         "The courtroom drama simulator."
         "<p><b>Source code:</b> "
         "<a href='https://github.com/AttorneyOnline/AO2-Client'>"
         "https://github.com/AttorneyOnline/AO2-Client</a>"
         "<p><b>Major development:</b><br>"
         "OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter, "
         "Crystalwarrior, Iamgoofball, in1tiate"
         "<p><b>Client development:</b><br>"
         "Cents02, windrammer, skyedeving, TrickyLeifa, Salanto"
         "<p><b>QA testing:</b><br>"
         "CaseyCazy, CedricDewitt, Chewable Tablets, CrazyJC, Fantos, "
         "Fury McFlurry, Geck, Gin-Gi, Jamania, Minx, Pandae, "
         "Robotic Overlord, Shadowlions (aka Shali), Sierra, SomeGuy, "
         "Veritas, Wiso"
         "<p><b>Translations:</b><br>"
         "k-emiko (Русский), Pyraq (Polski), scatterflower (日本語), vintprox (Русский), "
         "windrammer (Español, Português)"
         "<p><b>Special thanks:</b><br>"
         "Wiso, dyviacat (2.10 release); "
         "CrazyJC (2.8 release director) and MaximumVolty (2.8 release promotion); "
         "Remy, Hibiki, court-records.net (sprites); Qubrick (webAO); "
         "Rue (website); Draxirch (UI design); "
         "Lewdton and Argoneus (tsuserver); "
         "Fiercy, Noevain, Cronnicossy, and FanatSors (AO1); "
         "server hosts, game masters, case makers, content creators, "
         "and the whole AO2 community!"
         "<p>The Attorney Online networked visual novel project "
         "is copyright (c) 2016-2022 Attorney Online developers. Open-source "
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
  server_type f_server;
  int n_server = p_item->text(column).toInt();

  if (n_server == last_index) {
      return;
  }
  last_index = n_server;

  if (n_server < 0)
    return;

  QVector<server_type> f_server_list = ao_app->get_server_list();

  if (n_server >= f_server_list.size())
    return;

  f_server = f_server_list.at(n_server);

  set_server_description(f_server.desc);

  ui_server_description_text->moveCursor(QTextCursor::Start);
  ui_server_description_text->ensureCursorVisible();
  ui_server_player_count_lbl->setText(tr("Connecting..."));

  ui_connect_button->setEnabled(false);

  net_manager->connect_to_server(f_server);
}

// doubleclicked on an item in the serverlist so we'll connect right away
void Lobby::on_list_doubleclicked(QTreeWidgetItem *p_item, int column)
{
   Q_UNUSED(p_item)
   Q_UNUSED(column)
   net_manager->join_to_server();
}

void Lobby::on_favorite_list_context_menu_requested(const QPoint &point)
{
  auto *item = ui_favorites_tree->itemAt(point);
  if (item == nullptr) {
    qInfo() << "no favorite server item; skipping context menu";
    return;
  }
  const int server_index = item->data(0, Qt::DisplayRole).toInt();

  auto *menu = new QMenu(this);
  menu->addAction(tr("Remove"), ao_app, [this,server_index](){
    ao_app->remove_favorite_server(server_index);
    list_favorites();
  });
  menu->popup(ui_favorites_tree->mapToGlobal(point));
}

void Lobby::on_server_search_edited(QString p_text)
{
  // Iterate through all QTreeWidgetItem items
  QTreeWidgetItemIterator it(ui_serverlist_tree);
  while (*it) {
    (*it)->setHidden(p_text != "");
    ++it;
  }

  if (p_text != "") {
    // Search in metadata
    QList<QTreeWidgetItem *> clist = ui_serverlist_tree->findItems(
        ui_serverlist_search->text(), Qt::MatchContains | Qt::MatchRecursive, 1);
    foreach (QTreeWidgetItem *item, clist) {
      if (item->parent() != nullptr) // So the category shows up too
        item->parent()->setHidden(false);
      item->setHidden(false);
    }
  }
}

void Lobby::on_demo_clicked(QTreeWidgetItem *item, int column)
{
  Q_UNUSED(column)

  if (item == nullptr) {
    return;
  }

  QString l_filepath = (QApplication::applicationDirPath()+"/logs/%1/%2").arg(
              item->data(0,Qt::DisplayRole).toString(), item->data(1, Qt::DisplayRole).toString());
  ao_app->demo_server->start_server();
  server_type demo_server;
  demo_server.ip = "127.0.0.1";
  demo_server.port = ao_app->demo_server->port;
  ao_app->demo_server->set_demo_file(l_filepath);
  net_manager->connect_to_server(demo_server);
}

void Lobby::list_servers()
{
  ui_serverlist_tree->setSortingEnabled(false);
  ui_serverlist_tree->clear();

  ui_serverlist_search->setText("");

  int i = 0;
  for (const server_type &i_server : qAsConst(ao_app->get_server_list())) {
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui_serverlist_tree);
    treeItem->setData(0, Qt::DisplayRole, i);
    treeItem->setText(1, i_server.name);
    i++;
  }
  ui_serverlist_tree->setSortingEnabled(true);
  ui_serverlist_tree->sortItems(0, Qt::SortOrder::AscendingOrder);
}

void Lobby::list_favorites()
{
  ui_favorites_tree->setSortingEnabled(false);
  ui_favorites_tree->clear();

  int i = 0;
  for (const server_type &i_server : qAsConst(ao_app->get_favorite_list())) {
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui_favorites_tree);
    treeItem->setData(0, Qt::DisplayRole, i);
    treeItem->setText(1, i_server.name);
    i++;
  }
  ui_favorites_tree->setSortingEnabled(true);
  ui_favorites_tree->sortItems(0, Qt::SortOrder::AscendingOrder);
}

void Lobby::list_demos()
{
  ui_demo_tree->setSortingEnabled(false);
  ui_demo_tree->clear();

  QMultiMap<QString,QString> m_demo_entries = ao_app->load_demo_logs_list();
  for (auto &l_key : m_demo_entries.uniqueKeys()) {
    for (const QString &l_entry : m_demo_entries.values(l_key)) {
        QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui_demo_tree);
        treeItem->setData(0, Qt::DisplayRole, l_key);
        treeItem->setData(1, Qt::DisplayRole, l_entry);
    }
  }
  ui_demo_tree->setSortingEnabled(true);
  ui_demo_tree->sortItems(0, Qt::SortOrder::AscendingOrder);
}

void Lobby::get_motd()
{
  net_manager->request_document(MSDocumentType::Motd,
                                        [this](QString document) {
    if (document.isEmpty()) {
      document = tr("Couldn't get the message of the day.");
    }
    ui_motd_text->setHtml(document);
  });
}

void Lobby::check_for_updates()
{
  net_manager->request_document(MSDocumentType::ClientVersion,
                                        [this](QString version) {
    const QString current_version = ao_app->get_version_string();
    if (!version.isEmpty() && version != current_version) {
      ui_game_version_lbl->setText(tr("Version: %1 (!)").arg(current_version));
      ui_game_version_lbl->setToolTip(tr("New version available: %1").arg(version));
    }
  });
}

void Lobby::set_player_count(int players_online, int max_players)
{
  QString f_string = tr("Online: %1/%2").arg(
              QString::number(players_online),
              QString::number(max_players));
  ui_server_player_count_lbl->setText(f_string);
}

void Lobby::set_server_description(const QString& server_description)
{
    ui_server_description_text->clear();
    QString result = server_description.toHtmlEscaped()
                         .replace("\n", "<br>")
                         .replace(QRegExp("\\b(https?://\\S+\\.\\S+)\\b"), "<a href='\\1'>\\1</a>");
    ui_server_description_text->insertHtml(result);
}

void Lobby::enable_connect_button() { ui_connect_button->setEnabled(true); }

Lobby::~Lobby() {}
