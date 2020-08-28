#include "aoscene.h"
#include "courtroom.h"
#include "file_functions.h"

AOScene::AOScene(QWidget *parent, AOApplication *p_ao_app) : QLabel(parent)
{
  m_parent = parent;
  ao_app = p_ao_app;
  m_movie = new QMovie(this);
  m_movie->setCacheMode(QMovie::CacheAll);
  last_image = "";
}

void AOScene::set_image(QString p_image)
{
  QString background_path =
      ao_app->get_image_suffix(ao_app->get_background_path(p_image));
  if (!file_exists(background_path)) // If image is missing, clear current image
  {
    this->clear();
    this->setMovie(nullptr);

    m_movie->stop();
    last_image = "";
    return;
  }

  if (!file_exists(background_path) || background_path != last_image)
  {
    this->clear();
    this->setMovie(nullptr);

    m_movie->stop();
    m_movie->setFileName(background_path);
  }

  if (m_movie->isValid() && m_movie->frameCount() > 1) {
    m_movie->jumpToNextFrame();
    float scale_factor = static_cast<float>(f_h) /
                         static_cast<float>(m_movie->frameRect().height());
    // preserve aspect ratio
    int n_w = static_cast<int>(m_movie->frameRect().width() * scale_factor);
    int n_h = static_cast<int>(m_movie->frameRect().height() * scale_factor);

    m_movie->setScaledSize(QSize(n_w, n_h));
    this->resize(m_movie->scaledSize());
    if (!file_exists(background_path) || background_path != last_image)
    {
      this->setMovie(m_movie);
      m_movie->start();
    }
    QLabel::move(x + (f_w - n_w) / 2, y + (f_h - n_h) / 2); // Center
  }
  else {
    QPixmap background(background_path);
    auto transform_mode = Qt::FastTransformation;
    if (background.height() > f_h) // We are downscaling, use anti-aliasing.
      transform_mode = Qt::SmoothTransformation;

    background = background.scaledToHeight(f_h, transform_mode);
    this->resize(background.size());
    this->setPixmap(background);
    QLabel::move(
        x + (f_w - background.width()) / 2,
        y + (f_h - background.height()) /
                2); // Always center horizontally, always center vertically
  }
  last_image = background_path;
}

void AOScene::set_legacy_desk(QString p_image)
{

  QString desk_path =
      ao_app->get_image_suffix(ao_app->get_background_path(p_image));
  if (!file_exists(desk_path)) // If image is missing, clear current image
  {
    this->clear();
    this->setMovie(nullptr);

    m_movie->stop();
    last_image = "";
    return;
  }

  if (file_exists(desk_path) && desk_path == last_image)
    return;

  QPixmap f_desk(desk_path);

  // vanilla desks vary in both width and height. in order to make that work
  // with viewport rescaling, some INTENSE math is needed.
  int vp_width = m_parent->width();
  int vp_height = m_parent->height();

  double h_modifier = vp_height / 192;

  int final_h = static_cast<int>(h_modifier * f_desk.height());

  this->clear();
  this->setMovie(nullptr);

  m_movie->stop();
  m_movie->setFileName(desk_path);

  m_movie->setScaledSize(QSize(vp_width, final_h));

  if (m_movie->isValid() && m_movie->frameCount() > 1) {
    this->setMovie(m_movie);
    m_movie->start();
  }
  else {
    this->resize(vp_width, final_h);
    this->setPixmap(f_desk.scaled(vp_width, final_h));
  }
  last_image = desk_path;
}

void AOScene::combo_resize(int w, int h)
{
  QSize f_size(w, h);
  f_w = w;
  f_h = h;
  this->resize(f_size);
}

void AOScene::move(int ax, int ay)
{
  x = ax;
  y = ay;
  QLabel::move(x, y);
}
