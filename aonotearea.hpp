#ifndef AONOTEAREA_HPP
#define AONOTEAREA_HPP

#include <QLabel>
#include <QScrollArea>
#include <QListWidget>
#include <QVector>
#include <QVBoxLayout>
#include <aobutton.h>
#include <aonotepicker.hpp>
#include <aoimage.h>

#include "aoapplication.h"

class AONoteArea : public AOImage
{
  Q_OBJECT

public:
  AONoteArea(QWidget *p_parent, AOApplication *p_ao_app);
  ~AONoteArea();

  AOButton *add_button;
  QVBoxLayout *m_layout;
  AOButton *f_button;

private:
  AOApplication *ao_app;
  void set_layout();

};

#endif // AONOTEAREA_HPP
