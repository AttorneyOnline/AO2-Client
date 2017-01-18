#ifndef AOCHARBUTTON_H
#define AOCHARBUTTON_H

#include "aoapplication.h"

#include <QPushButton>
#include <QString>
#include <QWidget>

class AOCharButton : public QPushButton
{
  Q_OBJECT

public:
  AOCharButton(QWidget *parent, AOApplication *p_ao_app);

  AOApplication *ao_app;

  void set_image(QString p_character);

private:
  QWidget *m_parent;
};

#endif // AOCHARBUTTON_H
