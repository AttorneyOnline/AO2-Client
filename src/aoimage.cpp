#include "file_functions.h"

#include "aoimage.h"
#include "options.h"

#include <QBitmap>

AOImage::AOImage(AOApplication *ao_app, QWidget *parent)
    : QLabel(parent)
    , ao_app(ao_app)
{}

QString AOImage::image()
{
  return m_file_name;
}

bool AOImage::setImage(QString fileName, QString miscellaneous)
{
  QString p_image_resolved = ao_app->get_image(fileName, Options::getInstance().theme(), Options::getInstance().subTheme(), ao_app->default_theme, miscellaneous, "", "", false);

  if (!file_exists(p_image_resolved))
  {
    qWarning() << "could not find image" << fileName;
    return false;
  }

  m_file_name = p_image_resolved;
  QPixmap f_pixmap(m_file_name);
  f_pixmap = f_pixmap.scaled(size(), Qt::IgnoreAspectRatio);
  setPixmap(f_pixmap);

  return true;
}

bool AOImage::setImage(QString fileName)
{
  return setImage(fileName, QString());
}
