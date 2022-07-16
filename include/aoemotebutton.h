#ifndef AOEMOTEBUTTON_H
#define AOEMOTEBUTTON_H

#include "aoapplication.h"
#include <QPainter>
#include <QDebug>
#include <QPushButton>
#include <QLabel>

class AOEmoteButton : public QPushButton {
  Q_OBJECT

public:
  AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y,
                int p_w, int p_h);

  void set_image(QString p_image, QString p_emote_comment);
  void set_char_image(QString p_char, int p_emote, bool on);

  void set_selected_image(QString p_image);

  void set_id(int p_id) { m_id = p_id; }
  int get_id() { return m_id; }

private:
  QWidget *parent;
  AOApplication *ao_app;
  QLabel *ui_selected = nullptr;

  int m_id = 0;
signals:
  void emote_clicked(int p_id);

private slots:
  void on_clicked();
};

#endif // AOEMOTEBUTTON_H
