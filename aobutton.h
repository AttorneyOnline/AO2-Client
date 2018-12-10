#ifndef AOBUTTON_H
#define AOBUTTON_H

#include "text_file_functions.h"

#include <QPushButton>
#include <QDebug>

class AOButton : public QPushButton
{
  Q_OBJECT

public:
  AOButton(QWidget *parent);
  ~AOButton();

  void set_image(QString p_image);
};

#endif // AOBUTTON_H
