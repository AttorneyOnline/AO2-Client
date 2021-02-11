#include "aobutton.h"

#include "debug_functions.h"
#include "file_functions.h"

AOButton::AOButton(QWidget *parent, AOApplication *p_ao_app)
    : QPushButton(parent)
{
  ao_app = p_ao_app;
}

AOButton::~AOButton() {}

void AOButton::set_image(QString p_image)
{
  QString image_path =
      ao_app->get_static_image_suffix(ao_app->get_theme_path(p_image));
  QString default_image_path =
      ao_app->get_static_image_suffix(ao_app->get_default_theme_path(p_image));

  if (file_exists(image_path)) {
    this->setText("");
    this->setStyleSheet("QPushButton { border-image: url(\"" + image_path +
                        "\") 0 0 0 0 stretch stretch; }"
                        "QToolTip { background-image: url(); color: #000000; "
                        "background-color: #ffffff; border: 0px; }");
  }
  else if (file_exists(default_image_path)) {
    this->setText("");
    this->setStyleSheet("QPushButton { border-image: url(\"" +
                        default_image_path +
                        "\"); }"
                        "QToolTip { background-image: url(); color: #000000; "
                        "background-color: #ffffff; border: 0px; }");
  }
  else
      this->setStyleSheet("QPushButton { border-image: url(); }"
                          "QToolTip { background-image: url(); color: #000000; "
                          "background-color: #ffffff; border: 0px; }");
}
