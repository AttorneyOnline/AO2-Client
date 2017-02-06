#ifndef AOEMOTEBUTTON_H
#define AOEMOTEBUTTON_H

#include <QPushButton>

#include "aoapplication.h"

class AOEmoteButton : public QPushButton
{
  Q_OBJECT

public:
  AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y);

private:
  QWidget *m_parent;
  AOApplication *m_ao_app;
};

#endif // AOEMOTEBUTTON_H
