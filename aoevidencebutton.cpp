#include "aoevidencebutton.h"

#include "file_functions.h"

AOEvidenceButton::AOEvidenceButton(QWidget *p_parent, AOApplication *p_ao_app) : QPushButton(p_parent)
{
  ao_app = p_ao_app;
}

void AOEvidenceButton::set_image(QString p_image)
{
  QString image_path = ao_app->get_evidence_path() + p_image;

  if (file_exists(image_path))
  {
    this->setText("");
    this->setStyleSheet("border-image:url(\"" + image_path + "\")");
  }
  else
  {
    this->setText("evi");
    this->setStyleSheet("");
  }
}
