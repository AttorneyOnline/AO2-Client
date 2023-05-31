#include "lobby.h"

#include "aoapplication.h"
#include "demoserver.h"
#include "networkmanager.h"
#include "widgets/add_server_dialog.h"
#include "widgets/direct_connect_dialog.h"
#include "widgets/edit_server_dialog.h"

#include <QImageReader>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>

#include <QUiLoader>

#define FROM_UI(type, name)                                                    \
  ;                                                                            \
  ui_##name = findChild<type *>(#name);

#define COMBO_RELOAD()                                                         \
  list_servers();                                                              \
  list_favorites();                                                            \
  list_demos();                                                                \
  get_motd();                                                                  \
  check_for_updates();                                                         \
  reset_selection();

Lobby::Lobby(AOApplication *p_ao_app, NetworkManager *p_net_manager)
    : QMainWindow()
{
  ao_app = p_ao_app;
  net_manager = p_net_manager;

  loadUI();
  COMBO_RELOAD()
}

void Lobby::on_tab_changed(int index)
{
  switch (index) {
  case SERVER:
    current_page = SERVER;
    ui_add_to_favorite_button->setVisible(true);
    ui_remove_from_favorites_button->setVisible(false);
    ui_add_server_button->setVisible(false);
    ui_edit_favorite_button->setVisible(false);
    ui_direct_connect_button->setVisible(true);
    reset_selection();
    break;
  case FAVORITES:
    current_page = FAVORITES;
    ui_add_to_favorite_button->setVisible(false);
    ui_remove_from_favorites_button->setVisible(true);
    ui_add_server_button->setVisible(true);
    ui_edit_favorite_button->setVisible(true);
    ui_direct_connect_button->setVisible(false);
    reset_selection();
    break;
  case DEMOS:
    current_page = DEMOS;
    ui_add_to_favorite_button->setVisible(false);
    ui_add_server_button->setVisible(false);
    ui_remove_from_favorites_button->setVisible(false);
    ui_edit_favorite_button->setVisible(false);
    ui_direct_connect_button->setVisible(false);
    reset_selection();
    break;
  default:
    break;
  }
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

int Lobby::pageSelected() { return current_page; }

void Lobby::reset_selection()
{
  last_index = -1;
  ui_server_player_count_lbl->setText(tr("Offline"));
  ui_server_description_text->clear();

  ui_edit_favorite_button->setEnabled(false);
  ui_remove_from_favorites_button->setEnabled(false);
  ui_connect_button->setEnabled(false);
}

void Lobby::loadUI()
{
  this->setWindowTitle(
      tr("Attorney Online %1").arg(ao_app->applicationVersion()));
  this->setWindowIcon(QIcon(":/logo.png"));
  this->setWindowFlags((this->windowFlags() | Qt::CustomizeWindowHint));

  QUiLoader l_loader(this);
  QFile l_uiFile(Options::getInstance().getUIAsset(DEFAULT_UI));
  if (!l_uiFile.open(QFile::ReadOnly)) {
    qCritical() << "Unable to open file " << l_uiFile.fileName();
    return;
  }

  l_loader.load(&l_uiFile, this);

  FROM_UI(QLabel, game_version_lbl);
  ui_game_version_lbl->setText(
      tr("Version: %1").arg(ao_app->get_version_string()));

  FROM_UI(QPushButton, settings_button);
  connect(ui_settings_button, &QPushButton::clicked, this,
          &Lobby::onSettingsRequested);

  FROM_UI(QPushButton, about_button);
  connect(ui_about_button, &QPushButton::clicked, this,
          &Lobby::on_about_clicked);

  // Serverlist elements
  FROM_UI(QTabWidget, connections_tabview);
  ui_connections_tabview->tabBar()->setExpanding(true);
  connect(ui_connections_tabview, &QTabWidget::currentChanged, this,
          &Lobby::on_tab_changed);

  FROM_UI(QTreeWidget, serverlist_tree);
  FROM_UI(QLineEdit, serverlist_search);
  connect(ui_serverlist_tree, &QTreeWidget::itemClicked, this,
          &Lobby::on_server_list_clicked);
  connect(ui_serverlist_tree, &QTreeWidget::itemDoubleClicked, this,
          &Lobby::on_list_doubleclicked);
  connect(ui_serverlist_search, &QLineEdit::textChanged, this,
          &Lobby::on_server_search_edited);

  FROM_UI(QTreeWidget, favorites_tree);
  connect(ui_favorites_tree, &QTreeWidget::itemClicked, this,
          &Lobby::on_favorite_tree_clicked);
  connect(ui_favorites_tree, &QTreeWidget::itemDoubleClicked, this,
          &Lobby::on_list_doubleclicked);

  FROM_UI(QTreeWidget, demo_tree);
  connect(ui_demo_tree, &QTreeWidget::itemClicked, this,
          &Lobby::on_demo_clicked);
  connect(ui_demo_tree, &QTreeWidget::itemDoubleClicked, this,
          &Lobby::on_list_doubleclicked);

  FROM_UI(QPushButton, refresh_button);
  connect(ui_refresh_button, &QPushButton::released, this,
          &Lobby::on_refresh_released);

  FROM_UI(QPushButton, direct_connect_button);
  connect(ui_direct_connect_button, &QPushButton::released, this,
          &Lobby::on_direct_connect_released);

  FROM_UI(QPushButton, add_to_favorite_button)
  connect(ui_add_to_favorite_button, &QPushButton::released, this,
          &Lobby::on_add_to_fav_released);

  FROM_UI(QPushButton, add_server_button)
  ui_add_server_button->setVisible(false);
  connect(ui_add_server_button, &QPushButton::released, this,
          &Lobby::on_add_server_to_fave_released);

  FROM_UI(QPushButton, edit_favorite_button)
  ui_edit_favorite_button->setVisible(false);
  connect(ui_edit_favorite_button, &QPushButton::released, this,
          &Lobby::on_edit_favorite_released)

      FROM_UI(QPushButton, remove_from_favorites_button)
          ui_remove_from_favorites_button->setVisible(false);
  connect(ui_remove_from_favorites_button, &QPushButton::released, this,
          &Lobby::on_remove_from_fav_released);

  FROM_UI(QLabel, server_player_count_lbl)
  FROM_UI(QTextBrowser, server_description_text)
  FROM_UI(QPushButton, connect_button);
  connect(ui_connect_button, &QPushButton::released, net_manager,
          &NetworkManager::join_to_server);
  connect(ui_connect_button, &QPushButton::released, this, [=] {
    ui_server_player_count_lbl->setText(tr("Joining Server..."));
  });
  connect(net_manager, &NetworkManager::server_connected, ui_connect_button,
          &QPushButton::setEnabled);

  FROM_UI(QTextBrowser, motd_text);
  FROM_UI(QTextBrowser, game_changelog_text)
  if (ui_game_changelog_text != nullptr) {
    QString l_changelog_text = "No changelog found.";
    QFile l_changelog(get_base_path() + "changelog.md");
    if (!l_changelog.open(QFile::ReadOnly)) {
      qDebug() << "Unable to locate changelog file. Does it even exist?";
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
      ui_game_changelog_text->setMarkdown(l_changelog_text);
#else
      ui_game_changelog_text->setPlainText(
          l_changelog_text); // imperfect solution, but implementing Markdown
                             // ourselves for this edge case is out of scope
#endif
      return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    ui_game_changelog_text->setMarkdown(l_changelog.readAll());
#else
    ui_game_changelog_text->setPlainText((l_changelog.readAll()));
#endif
    l_changelog.close();

    QTabWidget *l_tabbar = findChild<QTabWidget *>("motd_changelog_tab");
    if (l_tabbar != nullptr) {
      l_tabbar->tabBar()->setExpanding(true);
    }
  }
}

void Lobby::on_refresh_released()
{
  net_manager->get_server_list(std::bind(&Lobby::list_servers, this));
  get_motd();
  list_favorites();
}

void Lobby::on_direct_connect_released()
{
  DirectConnectDialog connect_dialog(net_manager);
  connect_dialog.exec();
}

void Lobby::on_add_to_fav_released()
{
  int selection = get_selected_server();
  if (selection > -1) {
    Options::getInstance().addFavorite(ao_app->get_server_list().at(selection));
    list_favorites();
  }
}

void Lobby::on_add_server_to_fave_released()
{
  AddServerDialog l_dialog;
  l_dialog.exec();
  list_favorites();
  reset_selection();
}

void Lobby::on_edit_favorite_released()
{
  EditServerDialog l_dialog(get_selected_server());
  l_dialog.exec();
  list_favorites();
  reset_selection();
}

void Lobby::on_remove_from_fav_released()
{
  int selection = get_selected_server();
  if (selection >= 0) {
    Options::getInstance().removeFavorite(selection);
    list_favorites();
  }
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
         "Cents02, windrammer, skyedeving, TrickyLeifa, Salanto, lambdcalculus"
         "<p><b>QA testing:</b><br>"
         "CaseyCazy, CedricDewitt, Chewable Tablets, CrazyJC, Fantos, "
         "Fury McFlurry, Geck, Gin-Gi, Jamania, Minx, Pandae, "
         "Robotic Overlord, Shadowlions (aka Shali), Sierra, SomeGuy, "
         "Veritas, Wiso"
         "<p><b>Translations:</b><br>"
         "k-emiko (Русский), Pyraq (Polski), scatterflower (日本語), vintprox "
         "(Русский), "
         "windrammer (Español, Português)"
         "<p><b>Special thanks:</b><br>"
         "Wiso, dyviacat (2.10 release); "
         "CrazyJC (2.8 release director) and MaximumVolty (2.8 release "
         "promotion); "
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

  if (n_server < 0) return;

  QVector<server_type> f_server_list = ao_app->get_server_list();

  if (n_server >= f_server_list.size()) return;

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
  ui_server_player_count_lbl->setText(tr("Joining Server..."));
  net_manager->join_to_server();
}

void Lobby::on_favorite_tree_clicked(QTreeWidgetItem *p_item, int column)
{
  column = 0;
  server_type f_server;
  int n_server = p_item->text(column).toInt();

  if (n_server == last_index) {
    return;
  }
  last_index = n_server;

  if (n_server < 0) return;

  ui_add_server_button->setEnabled(true);
  ui_edit_favorite_button->setEnabled(true);
  ui_remove_from_favorites_button->setEnabled(true);

  QVector<server_type> f_server_list = Options::getInstance().favorites();

  if (n_server >= f_server_list.size()) return;

  f_server = f_server_list.at(n_server);

  set_server_description(f_server.desc);
  ui_server_description_text->moveCursor(QTextCursor::Start);
  ui_server_description_text->ensureCursorVisible();
  ui_server_player_count_lbl->setText(tr("Connecting..."));

  ui_connect_button->setEnabled(false);

  net_manager->connect_to_server(f_server);
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
        ui_serverlist_search->text(), Qt::MatchContains | Qt::MatchRecursive,
        1);
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

  QString l_filepath = (QApplication::applicationDirPath() + "/logs/%1/%2")
                           .arg(item->data(0, Qt::DisplayRole).toString(),
                                item->data(1, Qt::DisplayRole).toString());
  ao_app->demo_server->start_server();
  server_type demo_server;
  demo_server.ip = "127.0.0.1";
  demo_server.port = ao_app->demo_server->port;
  ao_app->demo_server->set_demo_file(l_filepath);
  net_manager->connect_to_server(demo_server);
}

void Lobby::onReloadThemeRequested()
{
  // This is destructive to the active widget data.
  // Whatever, this is lobby. Nothing here is worth saving.
  delete centralWidget();
  loadUI();
  COMBO_RELOAD()
}

void Lobby::onSettingsRequested()
{
  AOOptionsDialog options(nullptr, ao_app);
  connect(&options, &AOOptionsDialog::reloadThemeRequest, this,
          &Lobby::onReloadThemeRequested);
  options.exec();
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
  ui_serverlist_tree->resizeColumnToContents(0);
}

void Lobby::list_favorites()
{
  ui_favorites_tree->setSortingEnabled(false);
  ui_favorites_tree->clear();

  int i = 0;
  for (const server_type &i_server : Options::getInstance().favorites()) {
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui_favorites_tree);
    treeItem->setData(0, Qt::DisplayRole, i);
    treeItem->setText(1, i_server.name);
    i++;
  }
  ui_favorites_tree->setSortingEnabled(true);
  ui_favorites_tree->sortItems(0, Qt::SortOrder::AscendingOrder);
  ui_favorites_tree->resizeColumnToContents(0);
}

void Lobby::list_demos()
{
  ui_demo_tree->setSortingEnabled(false);
  ui_demo_tree->clear();

  QMultiMap<QString, QString> m_demo_entries = ao_app->load_demo_logs_list();
  for (auto &l_key : m_demo_entries.uniqueKeys()) {
    for (const QString &l_entry : m_demo_entries.values(l_key)) {
      QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui_demo_tree);
      treeItem->setData(0, Qt::DisplayRole, l_key);
      treeItem->setData(1, Qt::DisplayRole, l_entry);
    }
  }
  ui_demo_tree->setSortingEnabled(true);
  ui_demo_tree->sortItems(0, Qt::SortOrder::AscendingOrder);
  ui_demo_tree->resizeColumnToContents(0);
}

void Lobby::get_motd()
{
  net_manager->request_document(MSDocumentType::Motd, [this](QString document) {
    if (document.isEmpty()) {
      document = tr("Couldn't get the message of the day.");
    }
    ui_motd_text->setHtml(document);
  });
}

void Lobby::check_for_updates()
{
  net_manager->request_document(
      MSDocumentType::ClientVersion, [this](QString version) {
        const QString current_version = ao_app->get_version_string();
        if (!version.isEmpty() && version != current_version) {
          ui_game_version_lbl->setText(
              tr("Version: %1 (!)").arg(current_version));
          ui_game_version_lbl->setToolTip(
              tr("New version available: %1").arg(version));
        }
      });
}

void Lobby::set_player_count(int players_online, int max_players)
{
  QString f_string =
      tr("Online: %1/%2")
          .arg(QString::number(players_online), QString::number(max_players));
  ui_server_player_count_lbl->setText(f_string);
}

void Lobby::set_server_description(const QString &server_description)
{
  ui_server_description_text->clear();
  QString result =
      server_description.toHtmlEscaped()
          .replace("\n", "<br>")
          .replace(QRegularExpression("\\b(https?://\\S+\\.\\S+)\\b"),
                   "<a href='\\1'>\\1</a>");
  ui_server_description_text->insertHtml(result);
}

Lobby::~Lobby() {}
