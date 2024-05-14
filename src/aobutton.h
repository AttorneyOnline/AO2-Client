#pragma once

#include "aoapplication.h"

#include <QDebug>
#include <QMovie>
#include <QPushButton>

class AOButton : public QPushButton
{
  Q_OBJECT

public:
  AOButton(AOApplication *p_ao_app, QWidget *parent = nullptr);
  ~AOButton();

  void set_image(QString p_image, QString p_misc = QString());

private:
  AOApplication *ao_app;
  QMovie *m_movie;
};
