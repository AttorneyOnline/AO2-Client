#include "aoemotebutton.h"

AOEmoteButton::AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y) : QPushButton(p_parent)
{
  m_parent = p_parent;
  m_ao_app = p_ao_app;

  this->move(p_x, p_y);
  this->resize(40, 40);
}
