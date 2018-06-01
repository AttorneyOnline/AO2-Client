#include "aonotepad.h"

AONotepad::AONotepad(QWidget* p_parent, AOApplication *p_ao_app)
    : QTextEdit(p_parent), ao_app(p_ao_app)
{}
