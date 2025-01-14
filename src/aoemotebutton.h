#pragma once

#include "aoapplication.h"

#include <QLabel>
#include <QPushButton>

class AOEmoteButton : public QPushButton
{
  Q_OBJECT

public:
  AOEmoteButton(int id, int width, int height, AOApplication *ao_app, QWidget *parent = nullptr);

  int id();

  void setImage(QString character, QString path_to_buttons, int emoteId, bool enabled);

  void setSelectedImage(QString p_image);

Q_SIGNALS:
  void emoteClicked(int p_id);

private:
  AOApplication *ao_app;

  int m_id = 0;

  QLabel *ui_selected = nullptr;
};
