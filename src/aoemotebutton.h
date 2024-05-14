#pragma once

#include "aoapplication.h"
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QPushButton>

class AOEmoteButton : public QPushButton
{
  Q_OBJECT

public:
  AOEmoteButton(AOApplication *p_ao_app, int p_x, int p_y, int p_w, int p_h, QWidget *p_parent);

  void set_image(QString p_image, QString p_emote_comment);
  void set_char_image(QString p_char, int p_emote, bool on);

  void set_selected_image(QString p_image);

  void set_id(int p_id);
  int get_id();

Q_SIGNALS:
  void emote_clicked(int p_id);

private:
  AOApplication *ao_app;

  int m_id = 0;

  QLabel *ui_selected = nullptr;

private Q_SLOTS:
  void on_clicked();
};
