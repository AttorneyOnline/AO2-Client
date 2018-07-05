#ifndef AONOTEPAD_H
#define AONOTEPAD_H

#include <QTextEdit>

#include "aoapplication.h"

class AONotepad : public QTextEdit
{
  Q_OBJECT

public:
  AONotepad(QWidget* p_parent, AOApplication *p_ao_app);

private:
  AOApplication *ao_app = nullptr;
};

#endif // AONOTEPAD_H
