#pragma once

#include <QFuture>
#include <QImageReader>
#include <QMutex>
#include <QObject>
#include <QPixmap>
#include <QString>
#include <QWaitCondition>

#include <atomic>

namespace kal
{
class AnimationFrame
{
public:
  QPixmap texture;
  int duration = 0;
};

class AnimationLoader
{
  Q_DISABLE_COPY_MOVE(AnimationLoader)

public:
  explicit AnimationLoader(QThreadPool *threadPool);
  virtual ~AnimationLoader();

  QString loadedFileName() const;
  void load(const QString &fileName);
  void stopLoading();

  QSize size();

  int frameCount();
  AnimationFrame frame(int frameNumber);

  int loopCount();

private:
  QThreadPool *m_thread_pool;
  QString m_file_name;
  QSize m_size;
  int m_frame_count = 0;
  int m_loop_count = -1;
  QList<AnimationFrame> m_frames;
  QFuture<void> m_task;
  std::atomic_bool m_exit_task = false;
  QMutex m_task_lock;
  QWaitCondition m_task_signal;

  void populateVector(QImageReader *reader);
};
} // namespace kal
