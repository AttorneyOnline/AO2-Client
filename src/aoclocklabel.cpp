#include "aoclocklabel.h"

AOClockLabel::AOClockLabel(QWidget *parent) : QLabel(parent) {}

void AOClockLabel::start()
{
  this->resume();
}

void AOClockLabel::start(int msecs)
{
  QTime time = QTime::currentTime();
  if (msecs > time.msec())
  {
    target_time = time.addMSecs(msecs);
    timer.start(100, this);
  }
}

void AOClockLabel::pause()
{
  timer.stop();
}

void AOClockLabel::resume()
{
  timer.start(100, this);
}

void AOClockLabel::stop()
{
  this->setText("00:00:00.000");
  timer.stop();
}

void AOClockLabel::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer.timerId()) {
        if (QTime::currentTime() >= target_time)
        {
          this->stop();
          return;
        }
        QTime timeleft = QTime(0,0).addMSecs(QTime::currentTime().msecsTo(target_time));
        QString timestring = timeleft.toString("hh:mm:ss.zzz");
        this->setText(timestring);
    } else {
        QWidget::timerEvent(event);
    }
}
