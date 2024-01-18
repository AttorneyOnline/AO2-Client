#include "include/widgets/player_presence.h"

PlayerItem::PlayerItem(QListWidget *parent) : QListWidgetItem(parent)
{
  setFlags(flags() & ~Qt::ItemIsSelectable);
}

int PlayerItem::id() { return m_id; }

QString PlayerItem::name() { return m_name; }

QString PlayerItem::character() { return m_character; }

bool PlayerItem::isSpecial() { return m_isSpecial; }

void PlayerItem::setID(int f_id) { m_id = f_id; }

void PlayerItem::setName(QString f_name) { m_name = f_name; }

void PlayerItem::setCharacter(QString f_character)
{
  m_character = f_character;
}

void PlayerItem::setIsSpecial(bool f_state)
{
  m_isSpecial = f_state;
  styleEntry();
}

void PlayerItem::styleEntry()
{
  QIcon l_icon(m_character);
  setIcon(l_icon);

  QString label;
  if (m_isSpecial) {
    label = label + "⭐";
  }
  label = label + QString("[%1]%2").arg(QString::number(m_id), m_name);
  setText(label);
}

PlayerMenu::PlayerMenu(QWidget *parent) : QListWidget(parent)
{
}

void PlayerMenu::addPlayer(int f_id, QString f_name, QString f_character,
                           bool f_isSpecial)
{
  PlayerItem *l_player = players.value(f_id);
  if (l_player != nullptr) {
    updatePlayer(l_player, f_name, f_character, f_isSpecial);
    return;
  }

  l_player = new PlayerItem(this);
  l_player->setID(f_id);
  l_player->setName(f_name);
  l_player->setCharacter(f_character);
  l_player->setIsSpecial(f_isSpecial);

  players.insert(f_id, l_player);
}

void PlayerMenu::updatePlayer(PlayerItem *f_player, QString f_name,
                              QString f_character, bool f_isSpecial)
{
  f_player->setName(f_name);
  f_player->setCharacter(f_character);
  f_player->setIsSpecial(f_isSpecial);
}

void PlayerMenu::removePlayer(int f_id)
{
  PlayerItem *f_player = players.value(f_id);
  if (f_player == nullptr) {
    qDebug() << "Attempted to remove non-existant player at" << f_id;
    return;
  }
  delete f_player;
  players.remove(f_id);
}

void PlayerMenu::resetList()
{
  for (PlayerItem *player : qAsConst(players)) {
    delete player;
  }
}
