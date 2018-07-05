#include "aonotepicker.hpp"

#include "courtroom.h"

#include <QFileDialog>
#include <QDebug>

AONotePicker::AONotePicker(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  ao_app = p_ao_app;
}

AONotePicker::~AONotePicker()
{}

void Courtroom::on_file_selected()
{
  for(int i=0; i < ui_note_area->m_layout->count() -1; ++i)
    {
      AONotePicker *f_notepicker = static_cast<AONotePicker*>(ui_note_area->m_layout->itemAt(i)->widget());
      f_notepicker->m_hover->set_image("note_select.png");
    }

  AOButton *f_button = static_cast<AOButton*>(sender());
  AONotePicker *f_notepicker = static_cast<AONotePicker*>(f_button->parent());
  current_file = f_notepicker->real_file;
  load_note();
  f_button->set_image("note_select_selected.png");
}


void Courtroom::on_set_file_button_clicked()
{
  AOButton *f_button = static_cast<AOButton*>(sender());
  AONotePicker *f_notepicker = static_cast<AONotePicker*>(f_button->parent());
  QString f_filename = QFileDialog::getOpenFileName(this, "Open File");
  if(f_filename != "")
    {
      f_notepicker->m_line->setText(f_filename);
      f_notepicker->real_file = f_filename;

      set_note_files();
    }
}

void Courtroom::on_delete_button_clicked()
{
  AOButton *f_button = static_cast<AOButton*>(sender());
  AONotePicker *f_notepicker = static_cast<AONotePicker*>(f_button->parent());
  ui_note_area->m_layout->removeWidget(f_notepicker);
  delete f_notepicker;
  set_note_files();
}
