#pragma once

#include "datatypes.h"

#include <QList>
#include <QListWidget>
#include <QMap>
#include <QPointer>

class AOApplication;
class ModeratorDialog;

class PlayerListWidget : public QListWidget
{
  Q_OBJECT
public:
  explicit PlayerListWidget(AOApplication *ao_app, QWidget *parent = nullptr);
  virtual ~PlayerListWidget();

  void registerPlayer(const PlayerRegister &update);
  void updatePlayer(const PlayerUpdate &update);
  void reloadPlayers();

  void setAuthenticated(bool f_state);

private:
  AOApplication *ao_app;
  QMap<int, PlayerData> m_player_map;
  QMap<int, QListWidgetItem *> m_item_map;
  QPair<int, QPointer<ModeratorDialog>> active_moderator_menu;
  bool m_is_authenticated = false;

  void addPlayer(int playerId);
  void removePlayer(int playerId);
  void updatePlayer(int playerId, bool updateIcon);

  QString formatLabel(const PlayerData &data);

  void filterPlayerList();

Q_SIGNALS:
  void notify(const QString& messasge);

private Q_SLOTS:
  void onCustomContextMenuRequested(const QPoint &pos);
};
