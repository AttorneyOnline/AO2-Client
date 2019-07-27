#ifndef AOCHAT_H
#define AOCHAT_H

#include "aoapplication.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

class AOChat : public QWidget
{
  Q_OBJECT
public:
  AOChat(QWidget *parent, AOApplication *p_ao_app);
  void addMessageData(chat_message_type &message);
  QString showname() { return ui_showname->text(); }

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

  const QVector<QString> positions = {"wit", "def", "pro", "jud", "hld", "hlp",
                                      "jur", "sea"};

  QCheckBox *ui_flip;
  QCheckBox *ui_no_interrupt;
  QCheckBox *ui_preanim;
  QCheckBox *ui_realization;

  QListWidget *ui_emotes;

  QLineEdit *ui_showname;

  QString character;

signals:
  void messageSent();
  void positionChanged(QString pos);

public slots:
  void setCharacter(const QString &character);
  void clearEntry();

private slots:
  void on_interjection_toggled(bool toggled);
  void on_side_currentIndexChanged(int index);
  void on_emote_currentIndexChanged(int index);
  void on_emotes_currentItemChanged(QListWidgetItem *cur, QListWidgetItem *prev);
  void on_emotes_itemClicked(QListWidgetItem *emote);

  // TODO - need to subclass chatEntry to create this signal
  void on_chatEntry_enterPressed();
};

#endif // AOCHAT_H
