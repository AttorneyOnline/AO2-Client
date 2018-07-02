#include "aonotepicker.hpp"
#include "aonotearea.hpp"

#include "courtroom.h"

#include <QDebug>

AONoteArea::AONoteArea(QWidget *p_parent, AOApplication *p_ao_app) : AOImage(p_parent, p_ao_app)
{
  ao_app = p_ao_app;
}

AONoteArea::~AONoteArea()
{
}

void Courtroom::on_add_button_clicked()
{
  if(ui_note_area->m_layout->count() > 6)
    return;

  AONotePicker *f_notepicker = new AONotePicker(ui_note_area, ao_app);
  AOButton *f_button = new AOButton(f_notepicker, ao_app);
  AOButton *f_delete = new AOButton(f_notepicker, ao_app);
  QLineEdit *f_line = new QLineEdit(f_notepicker);
  AOButton *f_hover = new AOButton(f_notepicker, ao_app);
  QHBoxLayout *f_layout = new QHBoxLayout(f_notepicker);

  f_notepicker->m_line = f_line;
  f_notepicker->m_button = f_button;
  f_notepicker->m_layout = f_layout;
  f_notepicker->m_delete_button = f_delete;
  f_notepicker->m_hover = f_hover;
  f_notepicker->setProperty("index", ui_note_area->m_layout->count()-1);

  f_button->set_image("note_edit.png");
  f_delete->set_image("note_delete.png");
  f_hover->set_image("note_select.png");

  f_line->setReadOnly(true);

  f_layout->setSizeConstraint(QLayout::SetFixedSize);

  f_layout->addWidget(f_hover);
  f_layout->addWidget(f_line);
  f_layout->addWidget(f_button);
  f_layout->addWidget(f_delete);
  f_notepicker->setLayout(f_layout);
  ui_note_area->m_layout->addWidget(f_notepicker);

  if(contains_add_button)
    {
      ui_note_area->m_layout->removeWidget(ui_note_area->add_button);
      ui_note_area->m_layout->addWidget(ui_note_area->add_button);
      set_note_files();
    }

  set_dropdown(f_line, "[LINE EDIT]");

  connect(f_button, SIGNAL(clicked(bool)), this, SLOT(on_set_file_button_clicked()));
  connect(f_delete, SIGNAL(clicked(bool)), this, SLOT(on_delete_button_clicked()));
  connect(f_hover, SIGNAL(clicked(bool)), this, SLOT(on_file_selected()));
}

void Courtroom::set_note_files()
{
  QString filename = ao_app->get_base_path() + "configs/filesabstract.ini";
  QFile config_file(filename);

  if(!config_file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      qDebug() << "Couldn't open" << filename;
      return;
  }

  QTextStream in(&config_file);

  QByteArray t = "";

  for(int i = 0; i < ui_note_area->m_layout->count()-1; ++i)
    {
      AONotePicker *f_notepicker = static_cast<AONotePicker*>(ui_note_area->m_layout->itemAt(i)->widget());
      QString f_filestring = f_notepicker->real_file;
      QString f_filename = f_notepicker->m_line->text();

      t += QString::number(i) + " = " + f_filestring + " = " + f_filename + "\n\n";
    }


  config_file.close();

  QFile ex(filename);
  if(!ex.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
  {
      qDebug() << "Couldn't open" << filename;
      return;
  }

  ex.write(t);
  ex.close();
}
