#include "include/widgets/player_presence.h"
#include "aoapplication.h"
#include "options.h"
#include "qnamespace.h"

#include <QInputDialog>
#include <optional>

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
  setContextMenuPolicy(Qt::CustomContextMenu);
  setIconSize(QSize(25, 25));
  connect(this, &PlayerMenu::customContextMenuRequested, this,
          &PlayerMenu::onCustomContextMenuRequested);
}

void PlayerMenu::setAuthenticated(bool p_state) { is_authenticated = p_state; }

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
  f_player->setName(f_name);
  f_player->setCharacter(f_character);
  f_player->setIsSpecial(f_isSpecial);
}

void PlayerMenu::removePlayer(int f_id)
{
  PlayerItem *f_player = players.take(f_id);
  if (f_player == nullptr) {
    return;
  }
  delete f_player;
}

void PlayerMenu::resetList()
{
  for (PlayerItem *player : qAsConst(players)) {
    delete player;
  }
  players.clear();
}

void PlayerMenu::onCustomContextMenuRequested(const QPoint &pos)
{
  QPoint l_pos = mapToGlobal(pos);
  PlayerItem *l_item = static_cast<PlayerItem *>(itemAt(pos));
  if (l_item == nullptr) {
    qDebug() << "Unable to obtain PlayerItem object pointer.";
    return;
  }
  PlayerContextMenu *l_context_menu =
      new PlayerContextMenu(this, l_item, is_authenticated);
  l_context_menu->setAttribute(Qt::WA_DeleteOnClose);
  connect(l_context_menu, &PlayerContextMenu::actionTriggered, this,
          &PlayerMenu::actionTriggered);
  l_context_menu->popup(l_pos);
}

PlayerContextMenu::PlayerContextMenu(QWidget *parent, PlayerItem *f_player,
                                     bool f_authentication_state)
    : QMenu{parent}, player{f_player}
{

  if (f_authentication_state) {
    addSeparator();

    QMenu *kick_menu = addMenu("Kick");
    const QStringList kick_presets = Options::getInstance().getKickPresets();
    for (const QString &arg_list : kick_presets) {
      QStringList config = arg_list.split(",", Qt::KeepEmptyParts);
      if (config.size() != 2) {
        continue;
      }

      const QString display_name = config[0];
      const QString message = config[1];

      kick_menu->addAction(display_name, this, [this, message](bool triggered) {
        QString l_message = message;
        if (l_message.trimmed().isEmpty()) {

          QString title = tr("Kick message required!");
          QString prompt = tr("Enter kick message:");
          QPair<bool, QString> l_text = getUserInput(title, prompt);

          if (!l_text.first) {
            return;
          }

          l_message = l_text.second;
        }
        QStringList args{"/kick", QString::number(player->id()), l_message};
        emit actionTriggered(args);
      });
    }

    QMenu *ban_menu = addMenu("Ban");
    const QStringList ban_presets = Options::getInstance().getBanPresets();
    for (const QString &preset : ban_presets) {
      QStringList presets = preset.split(",", Qt::KeepEmptyParts);
      if (presets.size() != 3) {
        continue;
      }

      QString display_name = presets[0];
      QString duration = presets[1];
      QString message = presets[2];

      ban_menu->addAction(
          display_name, this, [this, duration, message](bool triggered) {
            QString l_message = message;

            QString title = tr("Ban message required!");
            QString prompt = tr("Enter ban message:");
            if (l_message.trimmed().isEmpty()) {
              QPair<bool, QString> l_text = getUserInput(title, prompt);

              if (!l_text.first) {
                return;
              }
              l_message = l_text.second;
            }
            QStringList args{"/ban", QString::number(player->id()), duration,
                             l_message};
            emit actionTriggered(args);
          });
    }

    addAction("Mute", this, [this](bool triggered) {
      Q_UNUSED(triggered);
      const QStringList args{"/mute", QString::number(player->id())};
      emit actionTriggered(args);
    });

    addAction("Unmute", this, [this](bool triggered) {
      Q_UNUSED(triggered);
      const QStringList args{"/unmute", QString::number(player->id())};
      emit actionTriggered(args);
    });
  }
}

QPair<bool, QString> PlayerContextMenu::getUserInput(QString title,
                                                     QString prompt)
{
  bool ok;
  QString l_text =
      QInputDialog::getText(nullptr, title, prompt, QLineEdit::Normal, "", &ok);
  if (!ok) {
    return {false, ""};
  }
  return {true, l_text};
}
