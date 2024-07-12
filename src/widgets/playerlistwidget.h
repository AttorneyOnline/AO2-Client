#pragma once

#include "datatypes.h"

#include <QList>
#include <QListWidget>
#include <QMap>

class AOApplication;

class PlayerListWidget : public QListWidget
{
public:
  explicit PlayerListWidget(AOApplication *ao_app, QWidget *parent = nullptr);
  virtual ~PlayerListWidget();

  void registerPlayer(const PlayerRegister &update);
  void updatePlayer(const PlayerUpdate &update);

  void setAuthenticated(bool f_state);

private:
  AOApplication *ao_app;
  QMap<int, PlayerData> m_player_map;
  QMap<int, QListWidgetItem *> m_item_map;
  bool m_is_authenticated = false;

  void addPlayer(int playerId);
  void removePlayer(int playerId);
  void updatePlayer(int playerId, bool updateIcon);

  void filterPlayerList();

private Q_SLOTS:
  void onCustomContextMenuRequested(const QPoint &pos);
};
