#include "aocharbutton.h"

#include <QPixmap>
#include <QPixmapCache>

namespace {
QPixmap char_icon_pixmap(const QString &path, const QSize &size)
{
  const QString key = path + QLatin1Char('#') + QString::number(size.width()) +
                      QLatin1Char('x') + QString::number(size.height());

  QPixmap pixmap;
  if (QPixmapCache::find(key, &pixmap))
    return pixmap;

  const QPixmap source(path);
  if (source.isNull())
    return QPixmap();

  pixmap = source.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  QPixmapCache::insert(key, pixmap);
  return pixmap;
}
} // namespace

AOCharButton::AOCharButton(QWidget *parent, AOApplication *p_ao_app, int x_pos,
                           int y_pos, bool is_taken)
    : QPushButton(parent)
{
  m_parent = parent;

  ao_app = p_ao_app;

  taken = is_taken;

  int size = static_cast<int>(60 * Options::getInstance().themeScalingFactor());
  int selector_size = static_cast<int>(62 * Options::getInstance().themeScalingFactor());

  this->resize(size, size);
  this->move(x_pos, y_pos);

  // Set once; set_image() must not replace the widget style sheet on every
  // page turn (that forces a CSS parse and style re-polish per button).
  this->setStyleSheet(
      "QPushButton { border: none; background: transparent; padding: 0px; }"
      "QToolTip { background-image: url(); color: #000000; "
      "background-color: #ffffff; border: 0px; }");

  ui_icon = new QLabel(this);
  ui_icon->resize(size, size);
  ui_icon->move(0, 0);
  ui_icon->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_icon->hide();

  ui_taken = new AOImage(this, ao_app, true);
  ui_taken->resize(size, size);
  ui_taken->set_image("char_taken");
  ui_taken->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_taken->hide();

  ui_passworded = new AOImage(this, ao_app, true);
  ui_passworded->resize(size, size);
  ui_passworded->set_image("char_passworded");
  ui_passworded->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_passworded->hide();

  ui_selector = new AOImage(parent, ao_app, true);
  ui_selector->resize(selector_size, selector_size);
  int offset = static_cast<int>(Options::getInstance().themeScalingFactor());
  ui_selector->move(x_pos - offset, y_pos - offset);
  ui_selector->set_image("char_selector");
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->hide();
}

void AOCharButton::reset()
{
  ui_taken->hide();
  ui_passworded->hide();
  ui_selector->hide();
}

void AOCharButton::set_taken(bool is_taken) { taken = is_taken; }

void AOCharButton::apply_taken_image()
{
  if (taken) {
    ui_taken->move(0, 0);
    ui_taken->show();
  }
  else {
    ui_taken->hide();
  }
}

void AOCharButton::set_passworded() { ui_passworded->show(); }

void AOCharButton::set_image(QString p_character)
{
  const QString image_path = ao_app->get_real_path(
      ao_app->get_character_path(p_character, "char_icon"),
      {".png"});

  const QPixmap pixmap = image_path.isEmpty()
                             ? QPixmap()
                             : char_icon_pixmap(image_path, this->size());
  if (!pixmap.isNull()) {
    ui_icon->setPixmap(pixmap);
    ui_icon->show();
    this->setText("");
  }
  else {
    ui_icon->hide();
    this->setText(p_character);
  }
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void AOCharButton::enterEvent(QEvent *e)
#else
void AOCharButton::enterEvent(QEnterEvent *e)
#endif
{
  int offset = static_cast<int>(Options::getInstance().themeScalingFactor());
  ui_selector->move(this->x() - offset, this->y() - offset);
  ui_selector->raise();
  ui_selector->show();

  setFlat(false);
  QPushButton::enterEvent(e);
}

void AOCharButton::leaveEvent(QEvent *e)
{
  ui_selector->hide();
  QPushButton::leaveEvent(e);
}
