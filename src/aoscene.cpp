#include "aoscene.h"
#include "courtroom.h"
#include "file_functions.h"

AOScene::AOScene(QWidget *parent, AOApplication *p_ao_app) : QLabel(parent)
{
  m_parent = parent;
  ao_app = p_ao_app;
  m_movie = new QMovie(this);
  last_image = "";
}

void AOScene::set_image(QString p_image)
{
  QString background_path = ao_app->get_image_suffix(ao_app->get_background_path(p_image));
  if (!file_exists(background_path)) //If image is missing, clear current image
  {
    this->clear();
    this->setMovie(nullptr);

    m_movie->stop();
    last_image = "";
    return;
  }

  if (file_exists(background_path) && background_path == last_image)
    return;

  int w = this->width();
  int h = this->height();

  this->clear();
  this->setMovie(nullptr);

  m_movie->stop();
  m_movie->setFileName(background_path);
  m_movie->setScaledSize(QSize(w, h));

  if (m_movie->isValid())
  {
    this->setMovie(m_movie);
    m_movie->start();
  }
  else
  {
    QPixmap background(background_path);
    this->setPixmap(background.scaled(w, h));
  }
  last_image = background_path;
}

void AOScene::set_legacy_desk(QString p_image)
{

  QString desk_path = ao_app->get_image_suffix(ao_app->get_background_path(p_image));
  if (!file_exists(desk_path)) //If image is missing, clear current image
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

  //vanilla desks vary in both width and height. in order to make that work with viewport rescaling,
  //some INTENSE math is needed.
  int vp_width = m_parent->width();
  int vp_height = m_parent->height();

  double h_modifier = vp_height / 192;

  int final_h = static_cast<int>(h_modifier * f_desk.height());

  this->clear();
  this->setMovie(nullptr);

  m_movie->stop();
  m_movie->setFileName(desk_path);

  m_movie->setScaledSize(QSize(vp_width, final_h));

  if (m_movie->isValid())
  {
    this->setMovie(m_movie);
    m_movie->start();
  }
  else
  {
      this->resize(vp_width, final_h);
      this->setPixmap(f_desk.scaled(vp_width, final_h));
  }
  last_image = desk_path;
}
