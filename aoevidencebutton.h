#ifndef AOEVIDENCEBUTTON_H
#define AOEVIDENCEBUTTON_H

#include "aoapplication.h"

#include <QPushButton>
#include <QString>

class AOEvidenceButton : public QPushButton
{
  Q_OBJECT

public:
  AOEvidenceButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y);

  void set_image(QString p_image);
  void set_id(int p_id) {m_id = p_id;}

private:
  AOApplication *ao_app;

  int m_id;

signals:
  void evidence_clicked(int p_id);

private slots:
  void on_clicked();
};

#endif // AOEVIDENCEBUTTON_H
