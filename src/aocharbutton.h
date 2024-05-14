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
  AOCharButton(AOApplication *p_ao_app, int x_pos, int y_pos, bool is_taken, QWidget *parent);

  void refresh();
  void reset();
  void set_taken(bool is_taken);

  void apply_taken_image();

  void set_image(QString p_character);

protected:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEvent *e) override;
#else
  void enterEvent(QEnterEvent *e) override;
#endif
  void leaveEvent(QEvent *e) override;

private:
  AOApplication *ao_app;
  bool m_taken = false;
  AOImage *ui_taken;
  AOImage *ui_selector;
};
