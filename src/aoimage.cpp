#include "file_functions.h"

#include "aoimage.h"

#include <QBitmap>

AOImage::AOImage(QWidget *parent, AOApplication *p_ao_app) : QLabel(parent)
{
  m_parent = parent;
  ao_app = p_ao_app;
}

AOImage::~AOImage() {}

bool AOImage::set_image(QString p_path, QString p_misc)
{
  p_path = ao_app->get_asset_path(true, p_path, ao_app->current_theme, ao_app->subtheme, ao_app->default_theme, p_misc);
  if (!file_exists(p_path)) {
    qDebug() << "Warning: Image" << p_path << "not found! Can't set!";
    return false;
  }

  QPixmap f_pixmap(p_path);

  f_pixmap =
      f_pixmap.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
  this->setPixmap(f_pixmap);
  this->setMask(f_pixmap.mask());
  return true;
}
