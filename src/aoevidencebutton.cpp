#include "aoevidencebutton.h"

#include "file_functions.h"

AOEvidenceButton::AOEvidenceButton(int id, int width, int height, AOApplication *ao_app, QWidget *parent)
    : QPushButton(parent)
    , ao_app(ao_app)
    , m_id(id)
{
  resize(width, height);

  ui_selected = new AOImage(ao_app, this);
  ui_selected->resize(width, height);
  ui_selected->setImage("evidence_selected");
  ui_selected->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selected->hide();

  ui_selector = new AOImage(ao_app, this);
  ui_selector->resize(width, height);
  ui_selector->setImage("evidence_selector");
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->hide();

  connect(this, &AOEvidenceButton::clicked, this, &AOEvidenceButton::on_clicked);
}

void AOEvidenceButton::setImage(QString fileName)
{
  QString image_path = ao_app->get_real_path(ao_app->get_evidence_path(fileName));
  if (file_exists(fileName))
  {
    setText("");
    setStyleSheet("QPushButton { border-image: url(\"" + fileName +
                  "\") 0 0 0 0 stretch stretch; }"
                  "QToolTip { color: #000000; background-color: #ffffff; border: 0px; }");
  }
  else if (file_exists(image_path))
  {
    setText("");
    setStyleSheet("QPushButton { border-image: url(\"" + image_path +
                  "\") 0 0 0 0 stretch stretch; }"
                  "QToolTip { color: #000000; background-color: #ffffff; border: 0px; }");
  }
  else
  {
    setText(fileName);
    setStyleSheet("QPushButton { border-image: url(); }"
                  "QToolTip { background-image: url(); color: #000000; "
                  "background-color: #ffffff; border: 0px; }");
  }
}

void AOEvidenceButton::setThemeImage(QString fileName)
{
  QString theme_image_path = ao_app->get_real_path(ao_app->get_theme_path(fileName));
  QString default_image_path = ao_app->get_real_path(ao_app->get_theme_path(fileName, ao_app->default_theme));

  QString final_image_path;

  if (file_exists(theme_image_path))
  {
    final_image_path = theme_image_path;
  }
  else
  {
    final_image_path = default_image_path;
  }

  setImage(final_image_path);
}

void AOEvidenceButton::setSelected(bool p_selected)
{
  if (p_selected)
  {
    ui_selected->show();
  }
  else
  {
    ui_selected->hide();
  }
}

void AOEvidenceButton::on_clicked()
{
  Q_EMIT evidenceClicked(m_id);
}

void AOEvidenceButton::mouseDoubleClickEvent(QMouseEvent *e)
{
  QPushButton::mouseDoubleClickEvent(e);
  Q_EMIT evidenceDoubleClicked(m_id);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void AOEvidenceButton::enterEvent(QEvent *e)
#else
void AOEvidenceButton::enterEvent(QEnterEvent *e)
#endif
{
  ui_selector->show();

  Q_EMIT mouseoverUpdated(m_id, true);

  setFlat(false);
  QPushButton::enterEvent(e);
}

void AOEvidenceButton::leaveEvent(QEvent *e)
{
  ui_selector->hide();

  Q_EMIT mouseoverUpdated(m_id, false);
  QPushButton::leaveEvent(e);
}
