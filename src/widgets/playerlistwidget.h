#pragma once

#include <QListWidget>

class PlayerList;
class PlayerListUpdate;
class PlayerListEntryUpdate;
class AOApplication;

class PlayerListWidget : public QListWidget
{
public:
  PlayerListWidget(AOApplication *ao_app, QWidget *parent);

public Q_SLOTS:
  void populateList(PlayerList list);
  void updatePlayerList(PlayerListUpdate update);
  void updatePlayerEntry(PlayerListEntryUpdate update);

private:
  AOApplication *ao_app;
  QMap<int, QListWidgetItem *> m_items;

  void addPlayerItem(int uid, QString icon, QString text);
  std::optional<QListWidgetItem *> getItemPointerById(int uid);
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

  enum UpdateType
  {
    ADD,
    REMOVE
  };

  int uid() { return m_uid; };
  QString icon() { return m_icon; };
  QString text() { return m_text; };
  UpdateType type() { return m_update_type; };

private:
  UpdateType m_update_type;
  int m_uid;
  QString m_icon;
  QString m_text;
};
Q_DECLARE_METATYPE(PlayerListUpdate::UpdateType)

class PlayerListEntryUpdate
{
public:
  PlayerListEntryUpdate(QByteArray f_data);
  ~PlayerListEntryUpdate() = default;

  enum UpdateType
  {
    UPDATEICON,
    UPDATETEXT
  };

  int uid() const { return m_uid; };
  QString data() const { return m_data; };
  UpdateType updateType() const { return m_update_type; };

  int m_uid;
  QString m_data;
  UpdateType m_update_type;
};
Q_DECLARE_METATYPE(PlayerListEntryUpdate::UpdateType)
