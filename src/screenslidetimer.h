#pragma once

#include <QObject>
#include <QParallelAnimationGroup>
#include <QTimer>

namespace kal
{
class ScreenSlideTimer : public QObject
{
  Q_OBJECT

public:
  static const int TRANSITION_BOOKEND_DELAY = 300;

  explicit ScreenSlideTimer(QObject *parent = nullptr);
  virtual ~ScreenSlideTimer();

  void addAnimation(QAbstractAnimation *animation);

public Q_SLOTS:
  void start();
  void stop();

Q_SIGNALS:
  void started();
  void finished();

private:
  enum TransitionState
  {
    NoTransition,
    PreTransition,
    GroupTransition,
    PostTransition,
  };

  bool m_running = false;
  TransitionState m_state = NoTransition;
  QParallelAnimationGroup *m_group;
  QTimer *m_pause;

private Q_SLOTS:
  void startNextState();
};
} // namespace kal
