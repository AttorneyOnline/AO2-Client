#ifndef AOBUTTON_H
#define AOBUTTON_H

#include <QPushButton>

class AOButton : public QPushButton
{
  Q_OBJECT

public:
  AOButton(QWidget *parent);
  ~AOButton();

  void set_image(QString p_image);

signals:
  void clicked();
  void pressed();
  void released();
};

#endif // AOBUTTON_H
