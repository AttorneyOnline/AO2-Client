#ifndef AOEMOTEBUTTON_H
#define AOEMOTEBUTTON_H

#include "aoapplication.h"

#include <QPushButton>
#include <QDebug>

class AOEmoteButton : public QPushButton
{
  Q_OBJECT

public:
  AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y);

  //void set_on(QString p_char, int p_emote);
  //void set_off(QString p_char, int p_emote);
  void set_image(QString p_char, int p_emote, QString suffix);

  void set_id(int p_id) {m_id = p_id;}
  int get_id() {return m_id;}

private:
  QWidget *parent;
  AOApplication *ao_app;

  int m_id = 0;

signals:
  void emote_clicked(int p_id);

private slots:
  void on_clicked();
};

#endif // AOEMOTEBUTTON_H
