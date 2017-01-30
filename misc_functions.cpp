#include "misc_functions.h"

#include <QTime>
#include <QCoreApplication>

void delay(int p_milliseconds)
{
  QTime dieTime = QTime::currentTime().addMSecs(p_milliseconds);

  while(QTime::currentTime() < dieTime)
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//alternates between true and false every time it is called. useful for certain optimization
bool cyclic_function()
{
  static bool cycle = true;

  if (cycle)
  {
    cycle = false;
    return cycle;
  }

  else
  {
    cycle = true;
    return cycle;
  }
}
