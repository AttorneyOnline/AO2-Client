#ifndef AOBUTTON_H
#define AOBUTTON_H

#include "aoapplication.h"

#include <QPushButton>
#include <QDebug>

class AOButton : public QPushButton
{
  Q_OBJECT

public:
  AOButton(QWidget *parent, AOApplication *p_ao_app);
  ~AOButton();

  AOApplication *ao_app;

  void set_image(QString p_image);
};

#endif // AOBUTTON_H
