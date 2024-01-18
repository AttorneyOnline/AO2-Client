#ifndef PLAYER_PRESENCE_H
#define PLAYER_PRESENCE_H

#include <QDebug>
#include <QListWidget>
#include <QMap>

class PlayerItem : public QListWidgetItem {
public:
  PlayerItem(QListWidget *parent);

  void setID(int f_id);
  void setName(QString f_name);
  void setCharacter(QString f_character);
  void setIsSpecial(bool f_state);

private:
  void styleEntry();

  int id;
  QString name;
  QString character;
  bool isSpecial;
};

class PlayerMenu : QListWidget {
  Q_OBJECT

public:
  PlayerMenu(QWidget *parent = nullptr);
  ~PlayerMenu() = default;

public slots:
  void addPlayer(int f_id, QString f_name, QString f_character,
                 bool f_isSpecial);
  void updatePlayer(PlayerItem *f_player, QString f_name, QString f_character,
                    bool f_isSpecial);
  void removePlayer(int f_id);
  void resetList();

private:
  QMap<int, PlayerItem *> players;
};

#endif // PLAYER_PRESENCE_H
