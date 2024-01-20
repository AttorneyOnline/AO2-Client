#ifndef PLAYER_PRESENCE_H
#define PLAYER_PRESENCE_H

#include <QDebug>
#include <QListWidget>
#include <QMap>
#include <QMenu>

class AOApplication;
class Courtroom;

class PlayerItem : public QListWidgetItem {
public:
  PlayerItem(QListWidget *parent = nullptr, AOApplication *p_ao_app = nullptr);

  int id();
  QString name();
  QString character();
  bool isSpecial();

  void setID(int f_id);
  void setName(QString f_name);
  void setCharacter(QString f_character);
  void setIsSpecial(bool f_state);

private:
  void styleEntry();

  AOApplication *ao_app;
  int m_id;
  QString m_name;
  QString m_character;
  bool m_isSpecial;
};

class PlayerContextMenu : public QMenu {
  Q_OBJECT
public:
  PlayerContextMenu(QWidget *parent, PlayerItem *f_player,
                    bool f_authentication_state);
  ~PlayerContextMenu() = default;

signals:
  void actionTriggered(QStringList args);

private slots:
  void onPairClicked();
  void onKickClicked();
  void onBanClicked();

private:
  PlayerItem *player;
};

class PlayerMenu : public QListWidget {
  Q_OBJECT
public:
  PlayerMenu(QWidget *parent = nullptr, AOApplication *p_ao_app = nullptr);
  ~PlayerMenu() = default;

public:
  void setAuthenticated(bool p_state);
  void addPlayer(QStringList f_content);
  void updatePlayer(PlayerItem *f_player, QString f_name, QString f_character,
                    bool f_isSpecial);
  void removePlayer(int f_id);
  void resetList();

signals:
  void actionTriggered(QStringList args);

private:
  QMap<int, PlayerItem *> players;
  AOApplication *ao_app;
  bool is_authenticated = false;

private slots:
  void onCustomContextMenuRequested(const QPoint &pos);
};

#endif // PLAYER_PRESENCE_H
