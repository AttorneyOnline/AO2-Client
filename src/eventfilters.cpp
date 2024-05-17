#include "eventfilters.h"

bool AOLineEditFilter::eventFilter(QObject *obj, QEvent *event)
{
  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
  if (event->type() == QEvent::FocusOut && lineEdit != nullptr && preserve_selection)
  { // lost focus
    int start = lineEdit->selectionStart();
    int len = lineEdit->selectionLength();

    if (start != -1 && len != -1)
    {
      lineEdit->setSelection(start, len);
      return true;
    }
  }
  return false;
}
