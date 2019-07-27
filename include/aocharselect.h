#ifndef AOCHARSELECT_H
#define AOCHARSELECT_H

#include "aoapplication.h"
#include "datatypes.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTextEdit>

class AOCharSelect : public QDialog
{
  Q_OBJECT
public:
  explicit AOCharSelect(QWidget *parent, AOApplication *p_ao_app);

  int selectedCharacterIndex();

private:
  AOApplication *ao_app;

  QListWidget *ui_char_list;
  QDialogButtonBox *ui_buttons;
  QPushButton *ui_spectate;

  void done(int retval);

public slots:
  // TODO: the data structure for characters has not been
  // set in stone yet
  void setCharacters(QVector<char_type> characters);

private slots:
  void on_charList_itemChanged(QListWidgetItem *item);
};


#endif // AOCHARSELECT_H
