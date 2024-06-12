#include "playerlistwidget.h"
#include "aoapplication.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

PlayerListWidget::PlayerListWidget(AOApplication *ao_app, QWidget *parent)
    : QListWidget(parent)
    , ao_app(ao_app)
{}

void PlayerListWidget::yeet()
{
  clear();
  m_items.clear();
}

void PlayerListWidget::populateList(PlayerList list)
{
  for (const auto &player : list.list())
  {
    QIcon char_icon = QIcon(ao_app->get_image_suffix(ao_app->get_character_path(player.icon, "char_icon"), true));
    QListWidgetItem *l_item = new QListWidgetItem(char_icon, player.text, this, Qt::UserRole);
    l_item->setData(Qt::UserRole, player.uid);
    m_items.insert(player.uid, l_item);
  }
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
