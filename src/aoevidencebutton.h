#pragma once

#include "aoapplication.h"
#include "aoimage.h"

#include <QDebug>
#include <QEnterEvent>
#include <QPushButton>
#include <QString>

class AOEvidenceButton : public QPushButton
{
  Q_OBJECT

public:
  AOEvidenceButton(AOApplication *p_ao_app, int p_x, int p_y, int p_w, int p_h, QWidget *p_parent = nullptr);

  void set_image(QString p_image);
  void set_theme_image(QString p_image);
  void set_id(int p_id) { m_id = p_id; }

  void set_selected(bool p_selected);

Q_SIGNALS:
  void evidence_clicked(int p_id);
  void evidence_double_clicked(int p_id);
  void on_hover(int p_id, bool p_state);

protected:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEvent *e) override;
#else
  void enterEvent(QEnterEvent *e) override;
#endif
  void leaveEvent(QEvent *e) override;
  void mouseDoubleClickEvent(QMouseEvent *e) override;

private:
  AOApplication *ao_app;

  int m_id = 0;

  AOImage *ui_selected;
  AOImage *ui_selector;

private Q_SLOTS:
  void on_clicked();
};
