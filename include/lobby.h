#ifndef LOBBY_H
#define LOBBY_H

#include "aobutton.h"
#include "aoimage.h"
#include "aopacket.h"
#include "aotextarea.h"

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QTextBrowser>
#include <QTreeWidget>
#include <QDebug>
#include <QScrollBar>
#include <QHeaderView>

#ifdef ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

class AOApplication;

class Lobby : public QMainWindow {
  Q_OBJECT

public:
  Lobby(AOApplication *p_ao_app);

  void set_widgets();
  void list_servers();
  void list_favorites();
  void list_demos();
  void get_motd();
  void check_for_updates();
  void append_chatmessage(QString f_name, QString f_message);
  void append_error(QString f_message);
  void set_player_count(int players_online, int max_players);
  void set_server_description(const QString& server_description);
  void set_stylesheet(QWidget *widget);
  void set_stylesheets();
  void set_fonts();
  void set_font(QWidget *widget, QString p_identifier);
  void set_loading_text(QString p_text);
  void show_loading_overlay() { ui_loading_background->show(); }
  void hide_loading_overlay() { ui_loading_background->hide(); }
  QString get_chatlog();
  int get_selected_server();
  void enable_connect_button();
  void reset_selection();

  void set_loading_value(int p_value);

  bool public_servers_selected = true;
  bool doubleclicked = false;

  ~Lobby();

private:
  AOApplication *ao_app;

  AOButton *ui_remove_from_fav;

  AOImage *ui_loading_background;
  QTextEdit *ui_loading_text;
  QProgressBar *ui_progress_bar;
  AOButton *ui_cancel;

  int last_index = -1;

  void set_size_and_pos(QWidget *p_widget, QString p_identifier);

  //UI-file Lobby

  //Top Row
  QLabel* ui_game_version_lbl;
  QPushButton* ui_settings_button;
  QPushButton* ui_about_button;

  // Server, Favs and Demo lists
  QTabWidget* ui_connections_tabview;

  QTreeWidget* ui_serverlist_tree;
  QLineEdit* ui_serverlist_search;

  QTreeWidget* ui_favorites_tree;
  QLineEdit* ui_favorites_search;

  QTreeWidget* ui_demo_tree;
  QLineEdit* ui_demo_search;

  QPushButton* ui_add_to_favorite_button;
  QPushButton* ui_remove_from_favorites_button;
  QPushButton* ui_refresh_button;

  // Serverinfo / MOTD Horizontal Row
  QTextBrowser* ui_motd_text;

  QLabel* ui_server_player_count_lbl;
  QTextBrowser* ui_server_description_text;
  QPushButton* ui_connect_button;

private slots:
  void on_public_servers_clicked();
  void on_favorites_clicked();

  void on_refresh_pressed();
  void on_refresh_released();
  void on_add_to_fav_pressed();
  void on_add_to_fav_released();
  void on_remove_from_fav_pressed();
  void on_remove_from_fav_released();
  void on_connect_pressed();
  void on_connect_released();
  void on_about_clicked();
  void on_settings_clicked();
  void on_server_list_clicked(QTreeWidgetItem *p_item, int column);
  void on_server_list_doubleclicked(QTreeWidgetItem *p_item, int column);
  void on_favorite_list_context_menu_requested(const QPoint &point);
  void on_server_search_edited(QString p_text);
  void on_demo_doubleclicked(QTreeWidgetItem *item, int column);
};

#endif // LOBBY_H
