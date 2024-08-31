#include "playerlistwidget.h"

#include "aoapplication.h"
#include "moderation_functions.h"
#include "widgets/moderator_dialog.h"

#include <QListWidgetItem>
#include <QMenu>

PlayerListWidget::PlayerListWidget(AOApplication *ao_app, QWidget *parent)
    : QListWidget(parent)
    , ao_app(ao_app)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &PlayerListWidget::customContextMenuRequested, this, &PlayerListWidget::onCustomContextMenuRequested);
}

PlayerListWidget::~PlayerListWidget()
{}

void PlayerListWidget::registerPlayer(const PlayerRegister &update)
{
  switch (update.type)
  {
  default:
    Q_UNREACHABLE();
    break;

  case PlayerRegister::ADD_PLAYER:
    addPlayer(update.id);
    break;

  case PlayerRegister::REMOVE_PLAYER:
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

  case PlayerUpdate::NAME:
    player.name = update.data;
    break;

  case PlayerUpdate::CHARACTER:
    player.character = update.data;
    update_icon = true;
    break;

  case PlayerUpdate::CHARACTER_NAME:
    player.character_name = update.data;
    break;

  case PlayerUpdate::AREA_ID:
    player.area_id = update.data.toInt();
    break;
  }
  updatePlayer(player.id, update_icon);

  filterPlayerList();
}

void PlayerListWidget::reloadPlayers()
{
  for (const PlayerData &player : qAsConst(m_player_map))
  {
    updatePlayer(player.id, false);
  }
}

void PlayerListWidget::setAuthenticated(bool f_state)
{
  m_is_authenticated = f_state;
  for (const PlayerData &data : qAsConst(m_player_map))
  {
    updatePlayer(data.id, false);
    filterPlayerList();
  }
}

void PlayerListWidget::onCustomContextMenuRequested(const QPoint &pos)
{
  QListWidgetItem *item = itemAt(pos);
  if (item == nullptr)
  {
    return;
  }
  int id = item->data(Qt::UserRole).toInt();
  QString name = item->text();

  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  QAction *report_player_action = menu->addAction("Report Player");
  connect(report_player_action, &QAction::triggered, this, [this, id, name] {
    auto maybe_reason = call_moderator_support(name);
    if (maybe_reason.has_value())
    {
      ao_app->send_server_packet(AOPacket("ZZ", {maybe_reason.value(), QString::number(id)}));
    }
  });

  if (m_is_authenticated)
  {
    QAction *kick_player_action = menu->addAction("Kick");
    connect(kick_player_action, &QAction::triggered, this, [this, id, name] {
      ModeratorDialog *dialog = new ModeratorDialog(id, false, ao_app);
      dialog->setWindowTitle(tr("Kick %1").arg(name));
      connect(this, &PlayerListWidget::destroyed, dialog, &ModeratorDialog::deleteLater);
      dialog->show();
    });

    QAction *ban_player_action = menu->addAction("Ban");
    connect(ban_player_action, &QAction::triggered, this, [this, id, name] {
      ModeratorDialog *dialog = new ModeratorDialog(id, true, ao_app);
      dialog->setWindowTitle(tr("Ban %1").arg(name));
      connect(this, &PlayerListWidget::destroyed, dialog, &ModeratorDialog::deleteLater);
      dialog->show();
    });
  }

  menu->popup(mapToGlobal(pos));
}

void PlayerListWidget::addPlayer(int playerId)
{
  m_player_map.insert(playerId, PlayerData{.id = playerId});
  QListWidgetItem *item = new QListWidgetItem(this);
  item->setData(Qt::UserRole, playerId);
  m_item_map.insert(playerId, item);
  updatePlayer(playerId, false);
}

void PlayerListWidget::removePlayer(int playerId)
{
  delete takeItem(row(m_item_map.take(playerId)));
  m_player_map.remove(playerId);
}

void PlayerListWidget::filterPlayerList()
{
  int area_id = m_player_map.value(ao_app->client_id).area_id;
  for (QListWidgetItem *item : qAsConst(m_item_map))
  {
    if (!item)
    {
      qWarning() << "Trying to filter item that does not exist. This indicates either a broken server-implementation or a bad demo file.";
      break;
    }
    item->setHidden(m_player_map[item->data(Qt::UserRole).toInt()].area_id != area_id && !m_is_authenticated);
  }
}

void PlayerListWidget::updatePlayer(int playerId, bool updateIcon)
{
  PlayerData &data = m_player_map[playerId];
  QListWidgetItem *item = m_item_map[playerId];

  if (!item)
  {
    qWarning() << "No player at ID" << playerId << ". This might indicate a broker server implementation or a bad demo file.";
    return;
  }

  item->setText(formatLabel(data));
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

QString PlayerListWidget::formatLabel(const PlayerData &data)
{
  QString format = Options::getInstance().playerlistFormatString();
  return format.replace("{id}", QString::number(data.id)).replace("{character}", data.character).replace("{displayname}", data.character_name.isEmpty() ? "No Data" : data.character_name).replace("{username}", m_is_authenticated ? data.name : "").simplified();
}
