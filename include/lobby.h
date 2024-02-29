#ifndef LOBBY_H
#define LOBBY_H

class QLabel;
class QLineEdit;
class QPushButton;
class QTextBrowser;
class QTreeWidget;
class QTreeWidgetItem;

#include <QMainWindow>
#include "networkmanager.h"
#include "file_functions.h"

#ifdef ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

class AOApplication;

class Lobby : public QMainWindow {
  Q_OBJECT
public:
  Lobby(AOApplication *p_ao_app, NetworkManager *p_net_man = nullptr);
  ~Lobby();

  void set_player_count(int players_online, int max_players);
  void set_server_description(const QString &server_description);
  void list_servers();
  int get_selected_server();
  int pageSelected();

private:
  AOApplication *ao_app;
  NetworkManager *net_manager;

  const QString DEFAULT_UI = "lobby.ui";

  void list_favorites();
  void list_demos();
  void get_motd();
  void check_for_updates();
  void reset_selection();

  int last_index = -1;

  enum TabPage { SERVER, FAVORITES, DEMOS };

  // UI-file Lobby

  // Top Row
  QLabel *ui_game_version_lbl;
  QPushButton *ui_settings_button;
  QPushButton *ui_about_button;

  // Server, Favs and Demo lists
  QTabWidget *ui_connections_tabview;

  QTreeWidget *ui_serverlist_tree;
  QLineEdit *ui_serverlist_search;

  QTreeWidget *ui_favorites_tree;
  QLineEdit *ui_favorites_search;

  QTreeWidget *ui_demo_tree;
  QLineEdit *ui_demo_search;

  QPushButton *ui_add_to_favorite_button;
  QPushButton *ui_add_server_button;
  QPushButton *ui_remove_from_favorites_button;
  QPushButton *ui_edit_favorite_button;
  QPushButton *ui_direct_connect_button;
  QPushButton *ui_refresh_button;

  // Serverinfo / MOTD Horizontal Row
  QTextBrowser *ui_motd_text;

  QLabel *ui_server_player_count_lbl;
  QTextBrowser *ui_server_description_text;
  QPushButton *ui_connect_button;

  // Optional Widget
  QTextBrowser *ui_game_changelog_text;

  void loadUI();

  TabPage current_page = SERVER;

private slots:
  void on_tab_changed(int index);
  void on_refresh_released();
  void on_direct_connect_released();
  void on_add_to_fav_released();
  void on_add_server_to_fave_released();
  void on_edit_favorite_released();
  void on_remove_from_fav_released();
  void on_about_clicked();
  void on_server_list_clicked(QTreeWidgetItem *p_item, int column);
  void on_list_doubleclicked(QTreeWidgetItem *p_item, int column);
  void on_favorite_tree_clicked(QTreeWidgetItem *p_item, int column);
  void on_server_search_edited(QString p_text);
  void on_demo_clicked(QTreeWidgetItem *item, int column);
  void onReloadThemeRequested(); // Oh boy.
  void onSettingsRequested();
};

#endif // LOBBY_H
