#include "aolineedit.h"

AOLineEdit::AOLineEdit(QWidget *parent) : QLineEdit(parent)
{
  this->setReadOnly(true);
  this->setFrame(false);

  connect(this, SIGNAL(returnPressed()), this, SLOT(on_enter_pressed()));
}

void AOLineEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
  QLineEdit::mouseDoubleClickEvent(e);

  this->setReadOnly(false);
}

void AOLineEdit::on_enter_pressed()
{
  this->setReadOnly(true);
}
