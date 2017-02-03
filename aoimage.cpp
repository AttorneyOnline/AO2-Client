#include "file_functions.h"

#include "aoimage.h"

AOImage::AOImage(QWidget *parent, AOApplication *p_ao_app) : QLabel(parent)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOImage::~AOImage()
{

}

void AOImage::set_image(QString p_image)
{
  QString theme_image_path = ao_app->get_theme_path() + p_image;
  QString default_image_path = ao_app->get_default_theme_path() + p_image;

  if (file_exists(theme_image_path))
    this->setPixmap(theme_image_path);
  else
    this->setPixmap(default_image_path);
}

void AOImage::set_image_from_path(QString p_path)
{
  QString default_path = ao_app->get_default_theme_path() + "chatmed.png";

  if (file_exists(p_path))
    this->setPixmap(p_path);
  else
    this->setPixmap(default_path);
}

void AOImage::set_scaled_image(QString p_image)
{
  QString theme_image_path = ao_app->get_theme_path() + p_image;
  QString default_image_path = ao_app->get_default_theme_path() + p_image;

  QString final_image_path;

  if (file_exists(theme_image_path))
    final_image_path = theme_image_path;
  else
    final_image_path = default_image_path;

  QPixmap f_pixmap(final_image_path);

  this->setPixmap(f_pixmap.scaled(this->width(), this->height()));
}
