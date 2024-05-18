#pragma once

#include <QBasicTimer>
#include <QDateTime>
#include <QDebug>
#include <QLabel>
#include <QTimerEvent>

class AOClockLabel : public QLabel
{
  Q_OBJECT

public:
  AOClockLabel(QWidget *parent);

  void start();
  void start(qint64 msecs);
  void set(qint64 msecs, bool update_text = false);
  void pause();
  void stop();
  void skip(qint64 msecs);
  void set_format(QString formatting, qint64 timer_value);
  bool active();

protected:
  void timerEvent(QTimerEvent *event) override;

private:
  QBasicTimer m_timer;
  QDateTime m_target_time;
  QString time_format = "hh:mm:ss";
};
