#include "widgets/aocharbutton.hpp"
#include "aoapplication.h"
#include "options.h"
#include <file_functions.h>

AOCharButton::AOCharButton(QWidget *parent, AOApplication *f_ao_app,
                           const int &f_id, const bool &f_taken,
                           const QString &f_name)
    : AttorneyOnline::UI::CharButton(parent), ao_app(f_ao_app),
      ui_taken_overlay(new QWidget(this)),
      ui_selector_overlay(new QWidget(this)), m_character_id(f_id),
      m_taken(f_taken), m_character_name(f_name)
{
  connect(this, &AOCharButton::pressed, this,
          [=]() { emit characterSelected(m_character_id); });
  setFixedSize(QSize(fixed_height, fixed_width));

  QString image_path = ao_app->get_image_suffix(
      ao_app->get_character_path(m_character_name, "char_icon"), true);

  if (QFile::exists(image_path)) {
    setIcon(QIcon(image_path));
    setIconSize(size());
  }
  else {
    setText(f_name);
  }

  ui_taken_overlay->setStyleSheet("background-color: rgba(0, 0, 0, 64);");
  ui_taken_overlay->setVisible(m_taken);
  ui_taken_overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_taken_overlay->setFixedSize(QSize(fixed_height, fixed_width));

  QString l_selector_path =
      ao_app->get_image("char_selector", Options::getInstance().theme(),
                        Options::getInstance().subTheme(),
                        ao_app->default_theme, "", "", "", true);

  ui_selector_overlay->setStyleSheet("image: url(" + l_selector_path + ");");
  ui_selector_overlay->setVisible(false);
  ui_selector_overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selector_overlay->setFixedSize(QSize(fixed_height, fixed_width));
}

AOCharButton::~AOCharButton() {}

void AOCharButton::setTaken(const bool &f_state)
{
  m_taken = f_state;
  ui_taken_overlay->setVisible(isTaken());
}

bool AOCharButton::isTaken() { return m_taken; }

int AOCharButton::characterID() { return m_character_id; }

QString AOCharButton::characterName() { return m_character_name; }

QSize AOCharButton::sizeHint() { return QSize(fixed_height, fixed_width); }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void AOCharButton::enterEvent(QEvent *e)
#else
void AOCharButton::enterEvent(QEnterEvent *e)
#endif
{
  ui_selector_overlay->setVisible(true);
  QPushButton::enterEvent(e);
}

void AOCharButton::leaveEvent(QEvent *e)
{
  ui_selector_overlay->setVisible(false);
  QPushButton::leaveEvent(e);
}
