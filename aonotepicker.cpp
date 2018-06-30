#include "aonotepicker.hpp"

#include <QFileDialog>
#include <QDebug>

AONotePicker::AONotePicker(QWidget *p_parent, AOApplication *p_ao_app) : QLabel(p_parent)
{
  m_parent = p_parent;
  ao_app = p_ao_app;

//  QLineEdit *m_line = new QLineEdit(this);
//  AOButton *m_button = new AOButton(this, ao_app);
//  QHBoxLayout *m_layout = new QHBoxLayout(this);

//  m_layout->addWidget(m_line);
//  m_layout->addWidget(m_button);
//  m_button->hide();
//  this->setLayout(m_layout);

//  connect(m_button, SIGNAL(clicked(bool)), this, SLOT(on_button_clicked()));
}

AONotePicker::~AONotePicker()
{}
