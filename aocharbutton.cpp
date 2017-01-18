#include "aocharbutton.h"

#include "path_functions.h"
#include "file_functions.h"

#include <QFile>

AOCharButton::AOCharButton(QWidget *parent, AOApplication *p_ao_app)
{
  m_parent = parent;

  ao_app = p_ao_app;

  this->resize(60, 60);
}

void AOCharButton::set_image(QString p_character)
{
  QString image_path = ao_app->get_character_path(p_character) + "char_icon.png";
  QString legacy_path = ao_app->get_demothings_path() + p_character.toLower() + "_char_icon.png";

  if (file_exists(image_path))
    this->setStyleSheet("border-image:url(\"" + image_path + "\")");
  else if (file_exists(legacy_path))
  {
    this->setStyleSheet("border-image:url(\"" + legacy_path + "\")");
    //ninja optimization
    QFile::copy(legacy_path, image_path);
  }
  else
  {
    this->setStyleSheet("border-image:url()");
    this->setText(p_character);
  }
}
