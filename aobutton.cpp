#include "aobutton.h"

#include "debug_functions.h"
#include "file_functions.h"

#include <QDebug>

AOButton::AOButton(QWidget *parent, AOApplication *p_ao_app) : QPushButton(parent)
{
  ao_app = p_ao_app;
}

AOButton::~AOButton()
{

}

void AOButton::set_image(QString p_image)
{
  QString f_image_name = p_image.left(p_image.lastIndexOf(QChar('.')));
  QString image_path = ao_app->get_theme_path() + p_image;
  QString hover_image_path = ao_app->get_theme_path() + f_image_name + "_hover.png";
  QString default_image_path = ao_app->get_default_theme_path() + p_image;

  if (file_exists(image_path))
    {
      if(file_exists(hover_image_path))
        this->setStyleSheet("QPushButton {border-image:url(\"" + image_path + "\");}"
                            "QPushButton:hover {border-image:url(\"" + hover_image_path + "\");}");
      else
        this->setStyleSheet("border-image:url(\"" + image_path + "\")");
    }

  else
    this->setStyleSheet("border-image:url(\"" + default_image_path + "\")");
}
