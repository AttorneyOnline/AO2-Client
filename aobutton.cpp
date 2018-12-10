#include "aobutton.h"

#include "debug_functions.h"
#include "file_functions.h"

AOButton::AOButton(QWidget *parent) : QPushButton(parent)
{

}

AOButton::~AOButton()
{

}

void AOButton::set_image(QString p_image)
{
  QString image_path = TextFileHandler::get_instance()->get_theme_path(p_image);
  QString default_image_path = TextFileHandler::get_instance()->get_default_theme_path(p_image);

  if (file_exists(image_path))
    this->setStyleSheet("border-image:url(\"" + image_path + "\")");
  else
    this->setStyleSheet("border-image:url(\"" + default_image_path + "\")");
}

