#include "widgets/aocharbutton.hpp"
#include "QFile"
#include "aoapplication.h"
#include "debug_functions.h"
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QMenu>
#include <file_functions.h>

AOCharButton::AOCharButton(QWidget *parent, AOApplication *f_ao_app,
                           const bool &f_id, const bool &f_taken,
                           const QString &f_name)
    : AttorneyOnline::UI::CharButton(parent), ao_app(f_ao_app),
      m_character_id(f_id), m_taken(f_taken), m_character_name(f_name)
{
  connect(this, &AOCharButton::pressed, this,
          [=]() { emit characterSelected(m_character_id); });
  setFixedSize(QSize(fixed_height, fixed_width));

  QString image_path = ao_app->get_image_suffix(
      ao_app->get_character_path(m_character_name, "char_icon"));

  if (QFile::exists(image_path)) {
    setIcon(QIcon(image_path));
    setIconSize(size());
  }
  else {
    setText(f_name);
  }

  if (f_taken) {
    setStyleSheet("background-color: rgba(0, 0, 0, 128)");
  }
  else {
    setStyleSheet("");
  }
}

AOCharButton::~AOCharButton() {}

void AOCharButton::setTaken(const bool &f_state)
{
  m_taken = f_state;
  if (isTaken()) {
    setStyleSheet("background-color: rgba(0, 0, 0, 128)");
  }
  else {
    setStyleSheet("");
  }
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

  QPushButton::enterEvent(e);
}

void AOCharButton::leaveEvent(QEvent *e) { QPushButton::leaveEvent(e); }

void AOCharButton::contextMenuEvent(QContextMenuEvent *event)
{
  qDebug() << "Context menu invoked on AOCharButton by" << event->reason();
  QMenu *l_context_menu = new QMenu(this);
  connect(l_context_menu, &QMenu::aboutToHide, l_context_menu,
          &QMenu::deleteLater);

  QString char_ini_path = ao_app->get_real_path(
      ao_app->get_character_path(characterName(), "char.ini"));
  if (!QFile::exists(char_ini_path)) {
    call_error(
        tr("Could not find character (char.ini) for %1").arg(characterName()));
    return;
  }
  QMenu *menu = new QMenu(this);
  menu->addAction(QString("Edit " + characterName() + "/char.ini"), this, [=] {
    QDesktopServices::openUrl(QUrl::fromLocalFile(char_ini_path));
  });
  menu->addSeparator();
  menu->addAction(
      QString("Open character folder " + characterName()), this, [=] {
        QString p_path =
            ao_app->get_real_path(VPath("characters/" + characterName() + "/"));
        if (!dir_exists(p_path)) {
          return;
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(p_path));
      });
  connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
  l_context_menu->popup(event->pos());
}
