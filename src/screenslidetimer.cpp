#include "screenslidetimer.h"

#include <QDebug>

namespace kal
{
ScreenSlideTimer::ScreenSlideTimer(QObject *parent)
    : QObject(parent)
{
  m_pause = new QTimer(this);
  m_pause->setInterval(TRANSITION_BOOKEND_DELAY);
  m_pause->setSingleShot(true);

  m_group = new QParallelAnimationGroup(this);

  connect(m_pause, &QTimer::timeout, this, &ScreenSlideTimer::startNextState);
  connect(m_group, &QParallelAnimationGroup::finished, this, &ScreenSlideTimer::startNextState);
}

ScreenSlideTimer::~ScreenSlideTimer()
{
  m_group->disconnect(this);
  m_pause->disconnect(this);
  stop();
}

void ScreenSlideTimer::addAnimation(QAbstractAnimation *animation)
{
  if (m_running)
  {
    qWarning() << "Cannot add animations while transition is in progress";
    return;
  }
  m_group->addAnimation(animation);
}

void ScreenSlideTimer::start()
{
  if (m_running)
  {
    qWarning() << "Transition already in progress";
    return;
  }
  m_running = true;
  startNextState();
}

void ScreenSlideTimer::stop()
{
  if (m_running)
  {
    m_running = false;
    m_state = NoTransition;
    m_group->stop();
    m_group->clear();
    m_pause->stop();
  }
}

void ScreenSlideTimer::startNextState()
{
  switch (m_state)
  {
  case NoTransition:
    m_state = PreTransition;
    Q_EMIT started();
    m_pause->start();
    return;

  case PreTransition:
    m_state = GroupTransition;
    m_group->start();
    break;

  case GroupTransition:
    m_state = PostTransition;
    m_pause->start();
    break;

  case PostTransition:
    stop();
    Q_EMIT finished();
    break;
  }
}
} // namespace kal
