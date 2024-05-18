#include "aocharbutton.h"

#include "file_functions.h"

AOCharButton::AOCharButton(AOApplication *ao_app, QWidget *parent)
    : QPushButton(parent)
    , ao_app(ao_app)
{
  int size = 60 * Options::getInstance().themeScalingFactor();
  int selector_size = 62 * Options::getInstance().themeScalingFactor();

  resize(size, size);

  ui_taken = new AOImage(ao_app, this);
  ui_taken->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_taken->resize(size, size);
  ui_taken->setImage("char_taken");
  ui_taken->hide();

  ui_selector = new AOImage(ao_app, parent);
  ui_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector->resize(selector_size, selector_size);
  ui_selector->setImage("char_selector");
  ui_selector->hide();
}

void AOCharButton::setTaken(bool enabled)
{
  if (enabled)
  {
    ui_taken->move(0, 0);
    ui_taken->show();
  }
  else
  {
    ui_taken->hide();
  }
}

void AOCharButton::setCharacter(QString character)
{
  QString image_path = ao_app->get_image_suffix(ao_app->get_character_path(character, "char_icon"), true);

  setText(QString());

  if (file_exists(image_path))
  {
    setStyleSheet("QPushButton { border-image: url(\"" + image_path +
                  "\") 0 0 0 0 stretch stretch; }"
                  "QToolTip { background-image: url(); color: #000000; "
                  "background-color: #ffffff; border: 0px; }");
  }
  else
  {
    setStyleSheet("QPushButton { border-image: url(); }"
                  "QToolTip { background-image: url(); color: #000000; "
                  "background-color: #ffffff; border: 0px; }");
    setText(character);
  }
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void AOCharButton::enterEvent(QEvent *event)
#else
void AOCharButton::enterEvent(QEnterEvent *event)
#endif
{
  int offset = Options::getInstance().themeScalingFactor();
  ui_selector->move(x() - offset, y() - offset);
  ui_selector->raise();
  ui_selector->show();

  setFlat(false);

  QPushButton::enterEvent(event);
}

void AOCharButton::leaveEvent(QEvent *event)
{
  ui_selector->hide();

  QPushButton::leaveEvent(event);
}
