#ifndef PLAYERLISTWIDGET_H
#define PLAYERLISTWIDGET_H

#include <QTreeWidget>

class PlayerListWidget : public QTreeWidget
{
public:
  PlayerListWidget(QWidget *parent);

  enum NetCommand
  {
    ADD,
    REMOVE,
    CLEAR,
    UPDATE
  };

  enum UpdateField
  {
    TEXT,
    ICON
  };

public Q_SLOTS:
  void handleNetworkCommand(QString json_body);
};

#endif // PLAYERLISTWIDGET_H
