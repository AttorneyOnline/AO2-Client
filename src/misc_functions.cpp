#include "misc_functions.h"

void delay(int p_milliseconds)
{
  QTime dieTime = QTime::currentTime().addMSecs(p_milliseconds);

  while(QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
