#ifndef AOCHAT_H
#define AOCHAT_H

#include "aoapplication.h"

#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

class AOChat : public QWidget
{
  Q_OBJECT
public:
  explicit AOChat(QWidget *parent, AOApplication *p_ao_app);

private:
  AOApplication *ao_app;

  QPushButton *ui_custom_interjection;
  QPushButton *ui_holdit;
  QPushButton *ui_objection;
  QPushButton *ui_takethat;

  QPushButton * const buttons[4] = {ui_custom_interjection, ui_holdit,
                                    ui_objection, ui_takethat};

  QTextEdit *ui_chat_entry;
  QComboBox *ui_color;
  QComboBox *ui_emote;
  QComboBox *ui_side;

  QCheckBox *ui_flip;
  QCheckBox *ui_no_interrupt;
  QCheckBox *ui_preanim;
  QCheckBox *ui_realization;

  QListWidget *ui_emotes;

  QLineEdit *ui_showname;

signals:
  void sendMessage();
  void positionChanged(QString pos);

public slots:
  void setCharacter(QString character);
  void clearEntry();

private slots:
  void on_interjection_toggled(bool toggled);
  void on_side_currentIndexChanged(int index);

  // TODO - need to subclass chatEntry to create this signal
  void on_chatEntry_enterPressed();
};

#endif // AOCHAT_H
