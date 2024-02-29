#include "aoevidencebutton.h"

#include "file_functions.h"

AOEvidenceButton::AOEvidenceButton(QWidget *p_parent, AOApplication *p_ao_app,
                                   int p_x, int p_y, int p_w, int p_h)
    : QPushButton(p_parent)
{
  ao_app = p_ao_app;
  m_parent = p_parent;

  ui_selected = new AOImage(this, ao_app, true);
  ui_selected->resize(p_w, p_h);
  //  ui_selected->move(p_x, p_y);
  ui_selected->set_image("evidence_selected");
  ui_selected->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selected->hide();

  ui_selector = new AOImage(this, ao_app, true);
  ui_selector->resize(p_w, p_h);
  //  ui_selector->move(p_x - 1, p_y - 1);
  ui_selector->set_image("evidence_selector");
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->hide();

  this->move(p_x, p_y);
  this->resize(p_w, p_h);
  //  this->setAcceptDrops(true);

  connect(this, &AOEvidenceButton::clicked, this, &AOEvidenceButton::on_clicked);
}

void AOEvidenceButton::set_image(QString p_image)
{
  QString image_path = ao_app->get_real_path(ao_app->get_evidence_path(p_image));
  if (file_exists(p_image)) {
    this->setText("");
    this->setStyleSheet(
        "QPushButton { border-image: url(\"" + p_image +
        "\") 0 0 0 0 stretch stretch; }"
        "QToolTip { color: #000000; background-color: #ffffff; border: 0px; }");
  }
  else if (file_exists(image_path)) {
    this->setText("");
    this->setStyleSheet(
        "QPushButton { border-image: url(\"" + image_path +
        "\") 0 0 0 0 stretch stretch; }"
        "QToolTip { color: #000000; background-color: #ffffff; border: 0px; }");
  }
  else {
    this->setText(p_image);
    this->setStyleSheet("QPushButton { border-image: url(); }"
                        "QToolTip { background-image: url(); color: #000000; "
                        "background-color: #ffffff; border: 0px; }");
  }
}

void AOEvidenceButton::set_theme_image(QString p_image)
{
  QString theme_image_path = ao_app->get_real_path(
        ao_app->get_theme_path(p_image));
  QString default_image_path = ao_app->get_real_path(
        ao_app->get_theme_path(p_image, ao_app->default_theme));

  QString final_image_path;

  if (file_exists(theme_image_path))
    final_image_path = theme_image_path;
  else
    final_image_path = default_image_path;

  this->set_image(final_image_path);
}

void AOEvidenceButton::set_selected(bool p_selected)
{
  if (p_selected)
    ui_selected->show();
  else
    ui_selected->hide();
}

void AOEvidenceButton::on_clicked() { emit evidence_clicked(m_id); }

void AOEvidenceButton::mouseDoubleClickEvent(QMouseEvent *e)
{
  QPushButton::mouseDoubleClickEvent(e);
  emit evidence_double_clicked(m_id);
}

/*
void AOEvidenceButton::dragLeaveEvent(QMouseEvent *e)
{
  //QWidget::dragLeaveEvent(e);

  qDebug() << "drag leave event";
}

void AOEvidenceButton::dragEnterEvent(QMouseEvent *e)
{
  //QWidget::dragEnterEvent(e);

  qDebug() << "drag enter event";
}
*/

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void AOEvidenceButton::enterEvent(QEvent *e)
#else
void AOEvidenceButton::enterEvent(QEnterEvent *e)
#endif
{
  ui_selector->show();

  emit on_hover(m_id, true);

  setFlat(false);
  QPushButton::enterEvent(e);
}

void AOEvidenceButton::leaveEvent(QEvent *e)
{
  ui_selector->hide();

  emit on_hover(m_id, false);
  QPushButton::leaveEvent(e);
}
