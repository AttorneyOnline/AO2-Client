#include "widgets/aochatentry.h"

#include <QKeyEvent>
#include <QDebug>

void AOChatEntry::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Return)
  {
    emit enterPressed();
    return;
  }

  QTextEdit::keyPressEvent(event);
}
