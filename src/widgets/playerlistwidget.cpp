#include "playerlistwidget.h"

#include "aoapplication.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QListWidgetItem>

PlayerList::PlayerList(const QJsonArray &array)
{
  for (int i = 0; i < array.size(); ++i)
  {
    QJsonObject player_json = array[i].toObject();
    PlayerData player;
    player.id = player_json["id"].toInt(-1);
    player.name = player_json["name"].toString();
    player.character = player_json["character"].toString();
    player.character_name = player_json["character_name"].toString();
    player.area_id = player_json["area_id"].toInt();
    player_list.append(player);
  }
}

PlayerListUpdate::PlayerListUpdate(const QJsonObject &object)
{
  id = object["id"].toInt(-1);
  type = UpdateType(object["type"].toInt());
}

PlayerUpdate::PlayerUpdate(const QJsonObject &object)
{
  id = object["id"].toInt(-1);
  type = DataType(object["type"].toInt());
  data = object["data"].toString();
}

PlayerListWidget::PlayerListWidget(AOApplication *ao_app, QWidget *parent)
    : QListWidget(parent)
    , ao_app(ao_app)
{}

PlayerListWidget::~PlayerListWidget()
{}

void PlayerListWidget::setPlayerList(const PlayerList &update)
{
  clear();
  m_player_map.clear();

  for (const PlayerData &i_player : update.player_list)
  {
    addPlayer(i_player.id);
    m_player_map[i_player.id] = i_player;
    updatePlayerItem(i_player.id, true);
  }

  filterPlayerList();
}

void PlayerListWidget::updatePlayerList(const PlayerListUpdate &update)
{
  switch (update.type)
  {
  default:
    Q_UNREACHABLE();
    break;

  case PlayerListUpdate::AddPlayerUpdate:
    addPlayer(update.id);
    break;

  case PlayerListUpdate::RemovePlayerUpdate:
    removePlayer(update.id);
    break;
  }
}

void PlayerListWidget::updatePlayer(const PlayerUpdate &update)
{
  PlayerData &player = m_player_map[update.id];

  bool update_icon = false;
  switch (update.type)
  {
  default:
    Q_UNREACHABLE();
    break;

  case PlayerUpdate::NameData:
    player.name = update.data;
    break;

  case PlayerUpdate::CharacterData:
    player.character = update.data;
    update_icon = true;
    break;

  case PlayerUpdate::CharacterNameData:
    player.character_name = update.data;
    break;

  case PlayerUpdate::AreaIdData:
    player.area_id = update.data.toInt();
    break;
  }
  updatePlayerItem(player.id, update_icon);

  filterPlayerList();
}

void PlayerListWidget::addPlayer(int playerId)
{
  m_player_map.insert(playerId, PlayerData{.id = playerId});
  QListWidgetItem *item = new QListWidgetItem(this);
  item->setData(Qt::UserRole, playerId);
  m_item_map.insert(playerId, item);
  updatePlayerItem(playerId, false);
}

void PlayerListWidget::removePlayer(int playerId)
{
  delete takeItem(row(m_item_map.take(playerId)));
  m_player_map.remove(playerId);
}

void PlayerListWidget::filterPlayerList()
{
  int area_id = m_player_map.value(ao_app->client_id).area_id;
  for (int i = 0; i < count(); ++i)
  {
    m_item_map[i]->setHidden(m_player_map[i].area_id != area_id);
  }
}

void PlayerListWidget::updatePlayerItem(int playerId, bool updateIcon)
{
  PlayerData &data = m_player_map[playerId];
  QListWidgetItem *item = m_item_map[playerId];
  item->setText(data.name.isEmpty() ? QObject::tr("Unnamed Player") : data.name);
  if (data.character.isEmpty())
  {
    item->setToolTip(QString());
    return;
  }

  QString tooltip = data.character;
  if (!data.character_name.isEmpty())
  {
    tooltip = QObject::tr("%1 aka %2").arg(data.character, data.character_name);
  }

  item->setToolTip(tooltip);

  if (updateIcon)
  {
    item->setIcon(QIcon(ao_app->get_image_suffix(ao_app->get_character_path(data.character, "char_icon"), true)));
  }
}
