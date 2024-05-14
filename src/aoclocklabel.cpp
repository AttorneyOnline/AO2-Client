#include "aoclocklabel.h"

AOClockLabel::AOClockLabel(QWidget *parent) : QLabel(parent) {}

void AOClockLabel::start()
{
  timer.start(1000 / 60, this);
}

void AOClockLabel::start(qint64 msecs)
{
  this->set(msecs);
  this->start();
}

void AOClockLabel::set(qint64 msecs, bool update_text)
{
  target_time = QDateTime::currentDateTime().addMSecs(msecs);
  if (update_text)
  {
    if (QDateTime::currentDateTime() >= target_time)
    {
      QTime timezero = QTime(0, 0);
      QString timestring = timezero.toString(time_format);
      this->setText(timestring);
    }
    else
    {
      qint64 ms_left = QDateTime::currentDateTime().msecsTo(target_time);
      QTime timeleft = QTime(0, 0).addMSecs(ms_left % (1000 * 3600 * 24));
      QString timestring = timeleft.toString(time_format);
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
  QTime timezero = QTime(0, 0);
  QString timestring = timezero.toString(time_format);
  this->setText(timestring);
  timer.stop();
}

void AOClockLabel::skip(qint64 msecs)
{
  qint64 ms_left = QDateTime::currentDateTime().msecsTo(target_time);
  this->set(ms_left - msecs, true);
}

bool AOClockLabel::active()
{
  return timer.isActive();
}

void AOClockLabel::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == timer.timerId()) {
    if (QDateTime::currentDateTime() >= target_time)
    {
      this->stop();
      return;
    }
    qint64 ms_left = QDateTime::currentDateTime().msecsTo(target_time);
    QTime timeleft = QTime(0, 0).addMSecs(ms_left % (1000 * 3600 * 24));
    QString timestring = timeleft.toString(time_format);
    this->setText(timestring);
  } else {
    QWidget::timerEvent(event);
  }
}

void AOClockLabel::set_format(QString formatting)
{
  time_format = formatting;
  if (!this->active()) {
    target_time = QDateTime::currentDateTime();
    QTime timeleft = QTime(0, 0);
    QString timestring = timeleft.toString(time_format);
    this->setText(timestring);
  }
