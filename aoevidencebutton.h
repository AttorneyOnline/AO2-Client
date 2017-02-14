#ifndef AOEVIDENCEBUTTON_H
#define AOEVIDENCEBUTTON_H

#include "aoapplication.h"

#include <QPushButton>
#include <QString>

class AOEvidenceButton : public QPushButton
{
public:
  AOEvidenceButton(QWidget *p_parent, AOApplication *p_ao_app);

  void set_image(QString p_image);

private:
  AOApplication *ao_app;
};

#endif // AOEVIDENCEBUTTON_H
