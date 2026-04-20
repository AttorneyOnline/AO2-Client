#pragma once

#include "aoapplication.h"
#include "aoimage.h"

#include <QEnterEvent>
#include <QFile>
#include <QPushButton>
#include <QString>
#include <QWidget>

class AOCharButton : public QPushButton
{
  Q_OBJECT

public:
  AOCharButton(AOApplication *ao_app, QWidget *parent);

  void setCharacter(QString character);

  QString character() const;

  void refreshIcon();

  void setTaken(bool enabled);

protected:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEvent *event) override;
#else
  void enterEvent(QEnterEvent *event) override;
#endif
  void leaveEvent(QEvent *event) override;

private:
  AOApplication *ao_app;
  QString m_character;
  bool m_taken = false;
  AOImage *ui_taken;
  AOImage *ui_selector;
};
