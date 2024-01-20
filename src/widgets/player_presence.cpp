#include "include/widgets/player_presence.h"
#include "aoapplication.h"

PlayerItem::PlayerItem(QListWidget *parent, AOApplication *p_ao_app)
    : QListWidgetItem{parent}, ao_app{p_ao_app}
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
  QString l_icon_path = ao_app->get_image_suffix(
      ao_app->get_character_path(m_character, "char_icon"));
  QIcon l_icon(l_icon_path);
  setIcon(l_icon);

  QString label = "[%1]%2";
  if (m_isSpecial) {
    label = "⭐[%1]%2";
  }
  setText(label.arg(QString::number(m_id), m_name));
}

PlayerMenu::PlayerMenu(QWidget *parent, AOApplication *p_ao_app)
    : QListWidget{parent}, ao_app{p_ao_app}
{
  setIconSize(QSize(25, 25));
}

void PlayerMenu::addPlayer(QStringList f_content)
{
  int id = f_content[0].toInt();
  QString name = f_content[1];
  QString character = f_content[2];
  bool is_special = QVariant::fromValue(f_content[3]).toBool();

  PlayerItem *l_player = players.value(id);
  if (l_player != nullptr) {
    updatePlayer(l_player, name, character, is_special);
    return;
  }
  qDebug() << "Creating player entry for" << id;
  l_player = new PlayerItem(this, ao_app);
  l_player->setID(id);
  l_player->setName(name);
  l_player->setCharacter(character);
  l_player->setIsSpecial(is_special);

  players.insert(id, l_player);
}

void PlayerMenu::updatePlayer(PlayerItem *f_player, QString f_name,
                              QString f_character, bool f_isSpecial)
{
  qDebug() << "Updating player entry for" << f_player->id();
  f_player->setName(f_name);
  f_player->setCharacter(f_character);
  f_player->setIsSpecial(f_isSpecial);
}

void PlayerMenu::removePlayer(int f_id)
{
  PlayerItem *f_player = players.take(f_id);
  if (f_player == nullptr) {
    qDebug() << "Attempted to remove non-existant player at" << f_id;
    return;
  }
  delete f_player;
}

void PlayerMenu::resetList()
{
  qDebug() << "Clearing player menu after server request.";
  for (PlayerItem *player : qAsConst(players)) {
    delete player;
  }
  players.clear();
}
