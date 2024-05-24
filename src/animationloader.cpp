#include "animationloader.h"

#include <QMutexLocker>
#include <QtConcurrent/QtConcurrent>

namespace kal
{
AnimationLoader::AnimationLoader(QThreadPool *threadPool)
    : m_thread_pool(threadPool)
{}

AnimationLoader::~AnimationLoader()
{
  stopLoading();
}

QString AnimationLoader::loadedFileName() const
{
  return m_file_name;
}

void AnimationLoader::load(const QString &fileName)
{
  if (m_file_name == fileName)
  {
    return;
  }
  stopLoading();
  m_file_name = fileName;
  QImageReader *reader = new QImageReader;
  reader->setFileName(fileName);
  m_size = reader->size();
  m_frames.clear();
  m_frame_count = reader->imageCount();
  m_loop_count = reader->loopCount();
  m_exit_task = false;
  m_task = QtConcurrent::run(m_thread_pool, [this, reader]() { populateVector(reader); });
}

void AnimationLoader::stopLoading()
{
  m_exit_task = true;
  if (m_task.isRunning())
  {
    m_task.waitForFinished();
  }
}

QSize AnimationLoader::size()
{
  return m_size;
}

int AnimationLoader::frameCount()
{
  return m_frame_count;
}

AnimationFrame AnimationLoader::frame(int frameNumber)
{
  if (m_frame_count <= 0)
  {
    return AnimationFrame();
  }

  m_task_lock.lock();
  while (m_frames.size() < frameNumber + 1)
  {
#ifdef DEBUG_MOVIE
    qDebug().noquote() << "Waiting for frame" << frameNumber << QString("(file: %1, frame count: %2)").arg(m_file_name).arg(m_frame_count);
#endif
    m_task_signal.wait(&m_task_lock);
  }

  AnimationFrame frame = qAsConst(m_frames)[frameNumber];
  m_task_lock.unlock();

  return frame;
}

int AnimationLoader::loopCount()
{
  return m_loop_count;
}

void AnimationLoader::populateVector(QImageReader *reader)
{
  int loaded_frame_count = 0;
  int frame_count = reader->imageCount();
  while (!m_exit_task && loaded_frame_count < frame_count)
  {
    {
      QMutexLocker locker(&m_task_lock);
      AnimationFrame frame;
      frame.texture = QPixmap::fromImage(reader->read());
      frame.duration = reader->nextImageDelay();
      m_frames.append(frame);
      ++loaded_frame_count;
    }
    m_task_signal.wakeAll();
  }

  delete reader;
}
} // namespace kal
