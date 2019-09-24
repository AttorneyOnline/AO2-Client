#include "aolineedit.h"

AOLineEdit::AOLineEdit(QWidget *parent) : QLineEdit(parent)
{
}

void AOLineEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
  QLineEdit::mouseDoubleClickEvent(e);

  double_clicked();
}
void AOLineEdit::focusOutEvent(QFocusEvent *ev)
{
  int start = selectionStart();
  int end = selectionEnd();
  QLineEdit::focusOutEvent(ev);
  if (p_selection && start != -1 && end != -1)
    this->setSelection(start, end);
}
