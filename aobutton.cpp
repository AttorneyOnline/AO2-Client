#include "aobutton.h"

#include "debug_functions.h"
#include "file_functions.h"

AOButton::AOButton(QWidget *parent, AOApplication *p_ao_app) : QPushButton(parent)
{
  ao_app = p_ao_app;
}

AOButton::~AOButton()
{

}

void AOButton::set_image(QString p_image)
{
  QString image_path = ao_app->get_theme_path(p_image);
  QString default_image_path = ao_app->get_default_theme_path(p_image);

  if (file_exists(image_path))
    this->setStyleSheet("border-image:url(\"" + image_path + "\")");
  else
    this->setStyleSheet("border-image:url(\"" + default_image_path + "\")");
}

