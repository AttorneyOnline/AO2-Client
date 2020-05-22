#include "aotextedit.h"

AOTextEdit::AOTextEdit(QWidget *parent) : QPlainTextEdit(parent)
{
  this->setReadOnly(true);

  //connect(this, SIGNAL(returnPressed()), this, SLOT(on_enter_pressed()));
}

void AOTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
  QPlainTextEdit::mouseDoubleClickEvent(e);

  this->setReadOnly(false);
}

void AOTextEdit::on_enter_pressed()
{
  this->setReadOnly(true);
}

