#include "file_functions.h"

#include "aoimage.h"
#include "options.h"

#include <QBitmap>

AOImage::AOImage(AOApplication *p_ao_app, QWidget *parent)
    : QLabel(parent)
    , ao_app(p_ao_app)
{}

AOImage::~AOImage()
{}

QString AOImage::file_name()
{
  return m_file_name;
}

bool AOImage::set_image(QString p_image, QString p_misc)
{
  QString p_image_resolved = ao_app->get_image(p_image, Options::getInstance().theme(), Options::getInstance().subTheme(), ao_app->default_theme, p_misc, "", "", false);

  if (!file_exists(p_image_resolved))
  {
    qWarning() << "could not find image" << p_image;
    return false;
  }

  m_file_name = p_image_resolved;
  QPixmap f_pixmap(m_file_name);
  f_pixmap = f_pixmap.scaled(size(), Qt::IgnoreAspectRatio);
  setPixmap(f_pixmap);

  return true;
}
