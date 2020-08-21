#include "aoclocklabel.h"

AOClockLabel::AOClockLabel(QWidget *parent) : QLabel(parent) {}

void AOClockLabel::start()
{
  this->resume();
}

void AOClockLabel::start(QTime p_time)
{
  QTime time = QTime::currentTime();
  if (p_time > time)
  {
    target_time = p_time;
    starting_time = time;
    timer.start(100, this);
  }
}

void AOClockLabel::pause() {}

void AOClockLabel::resume() {}

void AOClockLabel::stop() {}

void AOClockLabel::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer.timerId()) {
        QTime elapsed = QTime(0,0).addSecs(starting_time.secsTo(starting_time));
        this->setText(elapsed.toString("hh:mm:ss.zzz"));
    } else {
        QWidget::timerEvent(event);
    }
}
