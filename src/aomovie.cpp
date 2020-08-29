#include "aomovie.h"

#include "courtroom.h"
#include "file_functions.h"
#include "misc_functions.h"

AOMovie::AOMovie(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;

  m_movie = new QMovie();
  m_movie->setCacheMode(QMovie::CacheAll);

  this->setMovie(m_movie);

  timer = new QTimer(this);
  timer->setTimerType(Qt::PreciseTimer);
  timer->setSingleShot(true);

  connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(frame_change(int)));
  connect(timer, SIGNAL(timeout()), this, SLOT(timer_done()));
}

void AOMovie::set_play_once(bool p_play_once) { play_once = p_play_once; }

void AOMovie::play(QString p_image, QString p_char, QString p_custom_theme,
                   int duration)
{
  m_movie->stop();

  QString shout_path = p_image;
  if (!file_exists(p_image)) {
    QList<QString> pathlist;

    pathlist = {
        ao_app->get_image_suffix(
            ao_app->get_character_path(p_char, p_image)), // Character folder
        ao_app->get_image_suffix(ao_app->get_base_path() + "misc/" +
                                 p_custom_theme + "/" + p_image), // Misc path
        ao_app->get_image_suffix(ao_app->get_custom_theme_path(
            p_custom_theme, p_image)), // Custom theme path
        ao_app->get_image_suffix(ao_app->get_theme_path(p_image)), // Theme path
        ao_app->get_image_suffix(
            ao_app->get_default_theme_path(p_image)), // Default theme path
        ao_app->get_image_suffix(
            ao_app->get_theme_path("placeholder")), // Placeholder path
        ao_app->get_image_suffix(ao_app->get_default_theme_path(
            "placeholder")), // Default placeholder path
    };

    for (QString path : pathlist) {
      if (file_exists(path)) {
        shout_path = path;
        break;
      }
    }
  }

  m_movie->setFileName(shout_path);

  if (m_movie->loopCount() == 0)
    play_once = true;

  this->show();
  m_movie->start();
  if (m_movie->frameCount() == 0 && duration > 0)
    timer->start(duration);
}

void AOMovie::stop()
{
  m_movie->stop();
  this->hide();
}

void AOMovie::frame_change(int n_frame)
{
  // If it's a "static movie" (only one frame - png image), we can't change
  // frames - ignore this function (use timer instead). If the frame didn't reach
  // the last frame or the movie is continuous, don't stop the movie.
  if (m_movie->frameCount() == 0 || n_frame < (m_movie->frameCount() - 1) ||
      !play_once)
    return;
  // we need this or else the last frame wont show
  timer->start(m_movie->nextFrameDelay());
}

void AOMovie::timer_done()
{
  this->stop();
  // signal connected to courtroom object, let it figure out what to do
  done();
}

void AOMovie::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  this->resize(f_size);
  m_movie->setScaledSize(f_size);
}
