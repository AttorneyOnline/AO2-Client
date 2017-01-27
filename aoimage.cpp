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
