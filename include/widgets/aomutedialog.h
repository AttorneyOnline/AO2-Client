#ifndef AOMUTEDIALOG_H
#define AOMUTEDIALOG_H

#include "aoapplication.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTextEdit>

class AOMuteDialog : public QDialog
{
  Q_OBJECT
public:
  explicit AOMuteDialog(QWidget *parent);

private:
  QListWidget *ui_mute_list;
  QDialogButtonBox *ui_buttons;

signals:
  void mute(int cid, bool muted);
  void cleared();

public slots:
  void setMuteMap(QVector<QPair<QString, bool>> characters);

private slots:
  void on_mute_list_itemChanged(QListWidgetItem *item);
  void on_buttons_clicked(QAbstractButton *button);

};


#endif // AOMUTEDIALOG_H
