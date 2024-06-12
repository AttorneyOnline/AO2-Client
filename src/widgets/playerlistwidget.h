#pragma once

#include <QListWidget>

class PlayerList;
class AOApplication;

class PlayerListWidget : public QListWidget
{
public:
  PlayerListWidget(AOApplication *ao_app, QWidget *parent);
  void yeet();

public Q_SLOTS:
  void populateList(PlayerList list);

private:
  AOApplication *ao_app;
  QMap<int, QListWidgetItem *> m_items;
};

class PlayerList
{
public:
  PlayerList(QByteArray f_data);
  ~PlayerList() = default;

  struct PlayerInfo
  {
    int uid;
    QString icon;
    QString text;
  };

  QList<PlayerInfo> list() const { return m_initial_list; };

private:
  QList<PlayerInfo> m_initial_list;
};

class PlayerListUpdate
{
public:
  PlayerListUpdate(QByteArray f_data);
  ~PlayerListUpdate() = default;

  enum Type
  {
    ADD,
    REMOVE,
    UPDATE
  };
};
