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
  AOEvidenceButton(int id, int width, int height, AOApplication *ao_app, QWidget *parent = nullptr);

  void setImage(QString fileName);

  void setThemeImage(QString fileName);

  void setSelected(bool enabled);

Q_SIGNALS:
  void evidenceClicked(int id);
  void evidenceDoubleClicked(int id);

  void mouseoverUpdated(int id, bool state);

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
