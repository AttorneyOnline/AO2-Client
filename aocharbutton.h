#ifndef AOCHARBUTTON_H
#define AOCHARBUTTON_H

#include <QPushButton>
#include <QString>
#include <QWidget>

class AOCharButton : public QPushButton
{
  Q_OBJECT

public:
  AOCharButton(QWidget *parent);

  void set_image(QString p_character);

private:
  QWidget *m_parent;
};

#endif // AOCHARBUTTON_H
