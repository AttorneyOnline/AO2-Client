#ifndef AOCLOCKLABEL_H
#define AOCLOCKLABEL_H

#include <QLabel>
#include <QBasicTimer>
#include <QTimerEvent>
#include <QTime>

class AOClockLabel : public QLabel {
  Q_OBJECT

public:
  AOClockLabel(QWidget *parent);
  void start();
  void start(QTime p_time);
  void pause();
  void resume();
  void stop();

protected:
  void timerEvent(QTimerEvent *event) override;

private:
    QBasicTimer timer;
    QTime starting_time;
    QTime target_time;
};

#endif // AOCLOCKLABEL_H
