#include "playerlistwidget.h"
#include "aoapplication.h"
#include "qjsonobject.h"
#include "qnamespace.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <optional>

PlayerListWidget::PlayerListWidget(AOApplication *ao_app, QWidget *parent)
    : QListWidget(parent)
    , ao_app(ao_app)
{}

void PlayerListWidget::populateList(PlayerList list)
{
  reset();
  for (const auto &player : list.list())
  {
    addPlayerItem(player.uid, player.icon, player.text);
  }
}

void PlayerListWidget::updatePlayerList(PlayerListUpdate update)
{
  switch (update.type())
  {
  case PlayerListUpdate::ADD:
  {
    addPlayerItem(update.uid(), update.icon(), update.text());
  }
  break;
  case PlayerListUpdate::REMOVE:
  {
    if (m_items.contains(update.uid()))
    {
      delete m_items.value(update.uid());
      m_items.remove(update.uid());
    }
    else
    {
      qWarning() << "Unable to remove nonexistant entry of uid" << update.uid();
    }
  }

  break;
  }
}

void PlayerListWidget::updatePlayerEntry(PlayerListEntryUpdate update)
{
  auto l_item = getItemPointerById(update.uid());
  if (!l_item)
  {
    qWarning() << "Unable to edit nonexistant entry of uid" << update.uid();
    return;
  }

  switch (update.updateType())
  {
  case PlayerListEntryUpdate::UPDATEICON:
  {
    QIcon char_icon = QIcon(ao_app->get_image_suffix(ao_app->get_character_path(update.data(), "char_icon"), true));
    l_item.value()->setIcon(char_icon);
  }
  break;

  case PlayerListEntryUpdate::UPDATETEXT:
    l_item.value()->setText(update.data());
    break;
  }
}

void PlayerListWidget::reset()
{
  clear();
  m_items.clear();
}

void PlayerListWidget::addPlayerItem(int uid, QString icon, QString text)
{
  QIcon char_icon = QIcon(ao_app->get_image_suffix(ao_app->get_character_path(icon, "char_icon"), true));
  auto l_item = new QListWidgetItem(char_icon, text, this);
  l_item->setData(Qt::UserRole + 1, uid);
  m_items.insert(uid, l_item);
}

std::optional<QListWidgetItem *> PlayerListWidget::getItemPointerById(int uid)
{
  if (!m_items.contains(uid))
  {
    return std::nullopt;
  }
  return m_items.value(uid);
}

PlayerList::PlayerList(QByteArray f_data)
{
  QJsonParseError error;
  QJsonDocument l_json = QJsonDocument::fromJson(f_data, &error);

  if (error.error != QJsonParseError::NoError)
  {
    qWarning() << "Invalid or malformed initial playerlist received." << Qt::endl << "ErrorString" << error.errorString();
    return;
  }

  for (const auto &item : l_json.array())
  {
    QJsonObject player_data = item.toObject();
    PlayerInfo player;

    player.uid = player_data["uid"].toInt();
    player.icon = player_data["icon"].toString();
    player.text = player_data["text"].toString();

    m_initial_list.append(player);
  }
}

PlayerListUpdate::PlayerListUpdate(QByteArray f_data)
{
  QJsonParseError error;
  QJsonDocument l_json = QJsonDocument::fromJson(f_data, &error);

  if (error.error != QJsonParseError::NoError)
  {
    qWarning() << "Invalid or malformed initial playerlist received." << Qt::endl << "ErrorString" << error.errorString();
    return;
  }

  QJsonObject data = l_json.object();
  m_uid = data["uid"].toInt();
  m_icon = data["icon"].toString();
  m_text = data["text"].toString();
  m_update_type = data["type"].toVariant().value<UpdateType>();
}

PlayerListEntryUpdate::PlayerListEntryUpdate(QByteArray f_data)
{
  QJsonParseError error;
  QJsonDocument l_json = QJsonDocument::fromJson(f_data, &error);

  if (error.error != QJsonParseError::NoError)
  {
    qWarning() << "Invalid or malformed initial playerlist received." << Qt::endl << "ErrorString" << error.errorString();
    return;
  }

  QJsonObject data = l_json.object();
  m_uid = data["uid"].toInt();
  m_data = data["data"].toString();
  m_update_type = data["type"].toVariant().value<UpdateType>();
}
