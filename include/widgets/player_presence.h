#ifndef PLAYER_PRESENCE_H
#define PLAYER_PRESENCE_H

#include <QDebug>
#include <QListWidget>
#include <QMap>

class AOApplication;

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

class PlayerMenu : public QListWidget {

public:
  PlayerMenu(QWidget *parent = nullptr, AOApplication *p_ao_app = nullptr);
  ~PlayerMenu() = default;

public:
  void addPlayer(int f_id, QString f_name, QString f_character,
                 bool f_isSpecial);
  void updatePlayer(PlayerItem *f_player, QString f_name, QString f_character,
                    bool f_isSpecial);
  void removePlayer(int f_id);
  void resetList();

private:
  QMap<int, PlayerItem *> players;
  AOApplication *ao_app;
};

#endif // PLAYER_PRESENCE_H
