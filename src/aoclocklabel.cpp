#include "aoclocklabel.h"

AOClockLabel::AOClockLabel(QWidget *parent)
    : QLabel(parent)
{}

void AOClockLabel::start()
{
  m_timer.start(1000 / 60, this);
}

void AOClockLabel::start(qint64 msecs)
{
  this->set(msecs);
  this->start();
}

void AOClockLabel::set(qint64 msecs, bool update_text)
{
  m_target_time = QDateTime::currentDateTime().addMSecs(msecs);
  if (update_text)
  {
    if (QDateTime::currentDateTime() >= m_target_time)
    {
      this->setText("00:00:00.000");
    }
    else
    {
      qint64 ms_left = QDateTime::currentDateTime().msecsTo(m_target_time);
      QTime timeleft = QTime(0, 0).addMSecs(ms_left % (1000 * 3600 * 24));
      QString timestring = timeleft.toString("hh:mm:ss.zzz");
      this->setText(timestring);
    }
  }
}

void AOClockLabel::pause()
{
  m_timer.stop();
}

void AOClockLabel::stop()
{
  this->setText("00:00:00.000");
  m_timer.stop();
}

void AOClockLabel::skip(qint64 msecs)
{
  qint64 ms_left = QDateTime::currentDateTime().msecsTo(m_target_time);
  this->set(ms_left - msecs, true);
}

bool AOClockLabel::active()
{
  return m_timer.isActive();
}

void AOClockLabel::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == m_timer.timerId())
  {
    if (QDateTime::currentDateTime() >= m_target_time)
    {
      this->stop();
      return;
    }
    qint64 ms_left = QDateTime::currentDateTime().msecsTo(m_target_time);
    QTime timeleft = QTime(0, 0).addMSecs(ms_left % (1000 * 3600 * 24));
    QString timestring = timeleft.toString("hh:mm:ss.zzz");
    this->setText(timestring);
  }
  else
  {
    QWidget::timerEvent(event);
  }
}
