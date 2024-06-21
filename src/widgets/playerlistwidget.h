#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QListWidget>
#include <QMap>

class AOApplication;

class PlayerData
{
public:
  int id = -1;
  QString name;
  QString character;
  QString character_name;
  int area_id = 0;
};

class PlayerList
{
public:
  QList<PlayerData> player_list;

  PlayerList(const QJsonArray &array);
};

class PlayerListUpdate
{
public:
  enum UpdateType
  {
    AddPlayerUpdate,
    RemovePlayerUpdate,
  };

  int id;
  UpdateType type;

  PlayerListUpdate(const QJsonObject &object);
};

class PlayerUpdate
{
public:
  enum DataType
  {
    NameData,
    CharacterData,
    CharacterNameData,
    AreaIdData,
  };

  int id;
  DataType type;
  QString data;

  PlayerUpdate(const QJsonObject &object);
};

class PlayerListWidget : public QListWidget
{
public:
  explicit PlayerListWidget(AOApplication *ao_app, QWidget *parent = nullptr);
  virtual ~PlayerListWidget();

  void setPlayerList(const PlayerList &update);
  void updatePlayerList(const PlayerListUpdate &update);
  void updatePlayer(const PlayerUpdate &update);

private:
  AOApplication *ao_app;
  QMap<int, PlayerData> m_player_map;
  QMap<int, QListWidgetItem *> m_item_map;

  void addPlayer(int playerId);
  void removePlayer(int playerId);
  void updatePlayerItem(int playerId, bool updateIcon);

  void filterPlayerList();
};
