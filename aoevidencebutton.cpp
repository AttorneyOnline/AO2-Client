#include "aoevidencebutton.h"

#include "file_functions.h"

#include <QDebug>

AOEvidenceButton::AOEvidenceButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y) : QPushButton(p_parent)
{
  ao_app = p_ao_app;

  ui_selected = new AOImage(p_parent, ao_app);
  ui_selected->resize(70, 70);
  ui_selected->move(p_x, p_y);
  ui_selected->set_image("evidence_selected.png");
  ui_selected->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selected->hide();

  ui_selector = new AOImage(p_parent, ao_app);
  ui_selector->resize(71, 71);
  ui_selector->move(p_x - 1, p_y - 1);
  ui_selector->set_image("evidence_selector.png");
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->hide();

  this->move(p_x, p_y);
  this->resize(70, 70);

  connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
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
    this->setText(p_image);
    this->setStyleSheet("");
  }
}

void AOEvidenceButton::set_selected(bool p_selected)
{
  m_selected = p_selected;

  if (!m_selected)
    ui_selected->hide();
  else
    ui_selected->show();
}

void AOEvidenceButton::on_clicked()
{
  evidence_clicked(m_id);
}

void AOEvidenceButton::mouseDoubleClickEvent(QMouseEvent *e) {
  QPushButton::mouseDoubleClickEvent(e);
  evidence_double_clicked(m_id);
}

void AOEvidenceButton::enterEvent(QEvent * e)
{
  ui_selector->show();

  on_hover(m_id, true);

  setFlat(false);
  QPushButton::enterEvent(e);
}

void AOEvidenceButton::leaveEvent(QEvent * e)
{
  ui_selector->hide();

  on_hover(m_id, false);
  QPushButton::leaveEvent(e);
}
