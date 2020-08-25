#include "aoclocklabel.h"

AOClockLabel::AOClockLabel(QWidget *parent) : QLabel(parent) {}

void AOClockLabel::start()
{
  timer.start(100, this);
}

void AOClockLabel::start(int msecs)
{
  this->set(msecs);
  this->start();
}

void AOClockLabel::set(int msecs, bool update_text)
{
  QTime time = QTime::currentTime();
  if (msecs > time.msec())
  {
    target_time = time.addMSecs(msecs);
  }
  if (update_text)
  {
    if (QTime::currentTime() >= target_time)
    {
      this->setText("00:00:00.000");
    }
    else
    {
      QTime timeleft = QTime(0,0).addMSecs(QTime::currentTime().msecsTo(target_time));
      QString timestring = timeleft.toString("hh:mm:ss.zzz");
      this->setText(timestring);
    }
  }
}

void AOClockLabel::pause()
{
  timer.stop();
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
