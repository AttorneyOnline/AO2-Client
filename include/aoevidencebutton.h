#ifndef AOEVIDENCEBUTTON_H
#define AOEVIDENCEBUTTON_H

#include "aoapplication.h"
#include "aoimage.h"

#include <QDebug>
#include <QPushButton>
#include <QEnterEvent>
#include <QString>

class AOEvidenceButton : public QPushButton {
  Q_OBJECT

public:
  AOEvidenceButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y,
                   int p_w, int p_h);

  void set_image(QString p_image);
  void set_theme_image(QString p_image);
  void set_id(int p_id) { m_id = p_id; }

  void set_selected(bool p_selected);

private:
  AOApplication *ao_app;
  QWidget *m_parent;

  AOImage *ui_selected;
  AOImage *ui_selector;

  int m_id = 0;

protected:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEvent *e) override;
#else
  void enterEvent(QEnterEvent *e) override;
#endif
  void leaveEvent(QEvent *e) override;
  void mouseDoubleClickEvent(QMouseEvent *e) override;
  /*
void dragLeaveEvent(QMouseEvent *e);
void dragEnterEvent(QMouseEvent *e);
*/

signals:
  void evidence_clicked(int p_id);
  void evidence_double_clicked(int p_id);
  void on_hover(int p_id, bool p_state);

private slots:
  void on_clicked();
};

#endif // AOEVIDENCEBUTTON_H
