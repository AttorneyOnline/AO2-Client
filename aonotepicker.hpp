#ifndef AONOTEPICKER_HPP
#define AONOTEPICKER_HPP

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include "aobutton.h"

class AONotePicker : public QLabel
{
  Q_OBJECT

public:
  AONotePicker(QWidget *p_parent, AOApplication *p_ao_app);
  ~AONotePicker();

  QLineEdit *m_line;
  AOButton *m_button;
  AOButton *m_delete_button;
  AOButton *m_hover;
  QHBoxLayout *m_layout;
  QString real_file = "";

private:
  AOApplication *ao_app;
  QWidget *m_parent;
};

#endif // AONOTEPICKER_HPP
