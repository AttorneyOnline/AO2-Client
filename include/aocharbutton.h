#ifndef AOCHARBUTTON_H
#define AOCHARBUTTON_H

#include "aoapplication.h"
#include "aoimage.h"

#include <QFile>
#include <QPushButton>
#include <QEnterEvent>
#include <QString>
#include <QWidget>

class AOCharButton : public QPushButton {
  Q_OBJECT

public:
  AOCharButton(QWidget *parent, AOApplication *p_ao_app, int x_pos, int y_pos,
               bool is_taken);

  AOApplication *ao_app;

  void refresh();
  void reset();
  void set_taken(bool is_taken);
  void set_passworded();

  void apply_taken_image();

  void set_image(QString p_character);

private:
  bool taken;

  QWidget *m_parent;

  AOImage *ui_taken;
  AOImage *ui_passworded;
  AOImage *ui_selector;

protected:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEvent *e) override;
#else
  void enterEvent(QEnterEvent *e) override;
#endif
  void leaveEvent(QEvent *e) override;
};

#endif // AOCHARBUTTON_H
