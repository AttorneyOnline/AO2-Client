#include "aobutton.h"

#include "debug_functions.h"
#include "file_functions.h"

AOButton::AOButton(QWidget *parent, AOApplication *p_ao_app)
    : QPushButton(parent)
{
  ao_app = p_ao_app;
}

AOButton::~AOButton() {}

void AOButton::set_image(QString p_path, QString p_misc)
{
  QString p_image = ao_app->get_image(p_path, ao_app->current_theme, ao_app->get_subtheme(), ao_app->default_theme, p_misc);

  if (!file_exists(p_image)) {
      this->setStyleSheet("QPushButton { border-image: url(); }"
                          "QToolTip { background-image: url(); color: #000000; "
                          "background-color: #ffffff; border: 0px; }");
      return;
  };
  this->setText("");
  this->setStyleSheet("QPushButton { border-image: url(\"" + p_image +
                      "\") 0 0 0 0 stretch stretch; }"
                      "QToolTip { background-image: url(); color: #000000; "
                      "background-color: #ffffff; border: 0px; }");
}
