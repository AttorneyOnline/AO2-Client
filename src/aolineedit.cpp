#include "aolineedit.h"

AOLineEdit::AOLineEdit(QWidget *parent) : QLineEdit(parent) {}

void AOLineEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
  QLineEdit::mouseDoubleClickEvent(e);

  double_clicked();
}
void AOLineEdit::focusOutEvent(QFocusEvent *ev)
{
  int start = selectionStart();
  #if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  int len = selectionLength();
#else
  int len = selectedText().length();
#endif
  QLineEdit::focusOutEvent(ev);
  if (p_selection && start != -1 && len != -1)
    this->setSelection(start, len);
}
