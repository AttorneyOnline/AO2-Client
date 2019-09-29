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
  int len = selectionEnd() - start; //We're not using selectionLength because Linux build doesn't run qt5.10
  QLineEdit::focusOutEvent(ev);
  if (p_selection && start != -1 && len != -1)
    this->setSelection(start, len);
}
