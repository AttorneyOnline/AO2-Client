#ifndef AOCLOCKLABEL_H
#define AOCLOCKLABEL_H

#include <QLabel>
#include <QBasicTimer>
#include <QTimerEvent>
#include <QTime>
#include <QDebug>

class AOClockLabel : public QLabel {
  Q_OBJECT

public:
  AOClockLabel(QWidget *parent);
  void start();
  void start(int msecs);
  void pause();
  void resume();
  void stop();

protected:
  void timerEvent(QTimerEvent *event) override;

private:
    QBasicTimer timer;
    QTime target_time;
};

#endif // AOCLOCKLABEL_H
