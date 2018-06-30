#include "aonotepicker.hpp"
#include "aonotearea.hpp"

#include <QDebug>

AONoteArea::AONoteArea(QWidget *p_parent, AOApplication *p_ao_app) : AOImage(p_parent, p_ao_app)
{
  m_parent = p_parent;
  ao_app = p_ao_app;

//  AOButton *add_button = new AOButton(this, ao_app);
//  QVBoxLayout *m_layout = new QVBoxLayout(this);
//  AOButton *f_button = new AOButton(this, ao_app);

//  m_layout->addWidget(add_button);
//  add_button->hide();
//  add_button->setText("+");
//  this->setLayout(m_layout);

  qDebug() << "NOTEAREA CREATED";

//  connect(f_button, SIGNAL(clicked(bool)), this, SLOT(on_add_button_clicked()));
}

AONoteArea::~AONoteArea()
{
  for(QVector<AONotePicker*>::iterator pObj = m_notepicker.begin();
      pObj != m_notepicker.end(); ++pObj)
    {
      delete *pObj;
    }

  m_notepicker.clear();

  qDebug() << "NOTEAREA DESTROYED";
}

void AONoteArea::set_layout()
{
//  m_layout->removeWidget(add_button);
  m_layout->addWidget(add_button);

  this->setLayout(m_layout);
}
