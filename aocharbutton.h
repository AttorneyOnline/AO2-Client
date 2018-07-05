#ifndef AOCHARBUTTON_H
#define AOCHARBUTTON_H

#include "aoapplication.h"

#include <QPushButton>
#include <QString>
#include <QWidget>
#include "aoimage.h"

class AOCharButton : public QPushButton
{
  Q_OBJECT

public:
  AOCharButton(QWidget *parent, AOApplication *p_ao_app, int x_pos, int y_pos);

  AOApplication *ao_app = nullptr;

  void reset();
  void set_taken();
  void set_passworded();

  void set_image(QString p_character);

private:
  AOImage *ui_taken;
  AOImage *ui_passworded;
  AOImage *ui_selector;

protected:
  void enterEvent(QEvent *e);
  void leaveEvent(QEvent *e);
};

#endif // AOCHARBUTTON_H
