#include "aocaseannouncerdialog.h"

AOCaseAnnouncerDialog::AOCaseAnnouncerDialog(QWidget *parent,
                                             AOApplication *p_ao_app,
                                             Courtroom *p_court)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
  ao_app = p_ao_app;
  court = p_court;

  setWindowTitle(tr("Case Announcer"));
  resize(405, 235);

  ui_announcer_buttons = new QDialogButtonBox(this);

  QSizePolicy sizepolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sizepolicy.setHorizontalStretch(0);
  sizepolicy.setVerticalStretch(0);
  sizepolicy.setHeightForWidth(
      ui_announcer_buttons->sizePolicy().hasHeightForWidth());
  ui_announcer_buttons->setSizePolicy(sizepolicy);
  ui_announcer_buttons->setOrientation(Qt::Horizontal);
  ui_announcer_buttons->setStandardButtons(QDialogButtonBox::Ok |
                                           QDialogButtonBox::Cancel);

  QObject::connect(ui_announcer_buttons, SIGNAL(accepted()), this,
                   SLOT(ok_pressed()));
  QObject::connect(ui_announcer_buttons, SIGNAL(rejected()), this,
                   SLOT(cancel_pressed()));

  setUpdatesEnabled(false);

  ui_vbox_layout = new QVBoxLayout(this);

  ui_form_layout = new QFormLayout(this);
  ui_form_layout->setLabelAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                    Qt::AlignVCenter);
  ui_form_layout->setFormAlignment(Qt::AlignLeading | Qt::AlignLeft |
                                   Qt::AlignTop);
  ui_form_layout->setContentsMargins(6, 6, 6, 6);

  ui_vbox_layout->addItem(ui_form_layout);
  ui_vbox_layout->addWidget(ui_announcer_buttons);

  ui_case_title_label = new QLabel(this);
  ui_case_title_label->setText(tr("Case title:"));

  ui_form_layout->setWidget(0, QFormLayout::LabelRole, ui_case_title_label);

  ui_case_title_textbox = new QLineEdit(this);
  ui_case_title_textbox->setMaxLength(50);

  ui_form_layout->setWidget(0, QFormLayout::FieldRole, ui_case_title_textbox);

  ui_defense_needed = new QCheckBox(this);
  ui_defense_needed->setText(tr("Defense needed"));
  ui_prosecutor_needed = new QCheckBox(this);
  ui_prosecutor_needed->setText(tr("Prosecution needed"));
  ui_judge_needed = new QCheckBox(this);
  ui_judge_needed->setText(tr("Judge needed"));
  ui_juror_needed = new QCheckBox(this);
  ui_juror_needed->setText(tr("Jurors needed"));
  ui_steno_needed = new QCheckBox(this);
  ui_steno_needed->setText(tr("Stenographer needed"));

  ui_form_layout->setWidget(1, QFormLayout::FieldRole, ui_defense_needed);
  ui_form_layout->setWidget(2, QFormLayout::FieldRole, ui_prosecutor_needed);
  ui_form_layout->setWidget(3, QFormLayout::FieldRole, ui_judge_needed);
  ui_form_layout->setWidget(4, QFormLayout::FieldRole, ui_juror_needed);
  ui_form_layout->setWidget(5, QFormLayout::FieldRole, ui_steno_needed);

  setUpdatesEnabled(true);
}

void AOCaseAnnouncerDialog::ok_pressed()
{
  court->announce_case(
      ui_case_title_textbox->text(), ui_defense_needed->isChecked(),
      ui_prosecutor_needed->isChecked(), ui_judge_needed->isChecked(),
      ui_juror_needed->isChecked(), ui_steno_needed->isChecked());

  done(0);
}

void AOCaseAnnouncerDialog::cancel_pressed() { done(0); }
