#ifndef AOBUTTON_H
#define AOBUTTON_H

#include "aoapplication.h"

#include <QDebug>
#include <QPushButton>
#include <QMovie>

class AOButton : public QPushButton {
  Q_OBJECT

public:
  AOButton(QWidget *parent, AOApplication *p_ao_app);
  ~AOButton();

  AOApplication *ao_app;
  QMovie *movie;

  void set_image(QString p_image, QString p_misc="");
};

#endif // AOBUTTON_H
