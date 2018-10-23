#include "aocaseannouncerdialog.h"

AOCaseAnnouncerDialog::AOCaseAnnouncerDialog(QWidget *parent, AOApplication *p_ao_app, Courtroom *p_court)
{
  ao_app = p_ao_app;
  court = p_court;

  setWindowTitle("Case Announcer");
  resize(405, 235);

  AnnouncerButtons = new QDialogButtonBox(this);

  QSizePolicy sizepolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sizepolicy.setHorizontalStretch(0);
  sizepolicy.setVerticalStretch(0);
  sizepolicy.setHeightForWidth(AnnouncerButtons->sizePolicy().hasHeightForWidth());
  AnnouncerButtons->setSizePolicy(sizepolicy);
  AnnouncerButtons->setOrientation(Qt::Horizontal);
  AnnouncerButtons->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

  QObject::connect(AnnouncerButtons, SIGNAL(accepted()), this, SLOT(ok_pressed()));
  QObject::connect(AnnouncerButtons, SIGNAL(rejected()), this, SLOT(cancel_pressed()));

  setUpdatesEnabled(false);

  VBoxLayout = new QVBoxLayout(this);

  FormLayout = new QFormLayout(this);
  FormLayout->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
  FormLayout->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
  FormLayout->setContentsMargins(6, 6, 6, 6);

  VBoxLayout->addItem(FormLayout);
  VBoxLayout->addWidget(AnnouncerButtons);

  CaseTitleLabel = new QLabel(this);
  CaseTitleLabel->setText("Case title:");

  FormLayout->setWidget(0, QFormLayout::LabelRole, CaseTitleLabel);

  CaseTitleLineEdit = new QLineEdit(this);
  CaseTitleLineEdit->setMaxLength(50);

  FormLayout->setWidget(0, QFormLayout::FieldRole, CaseTitleLineEdit);

  DefenceNeeded = new QCheckBox(this);
  DefenceNeeded->setText("Defence needed");
  ProsecutorNeeded = new QCheckBox(this);
  ProsecutorNeeded->setText("Prosecution needed");
  JudgeNeeded = new QCheckBox(this);
  JudgeNeeded->setText("Judge needed");
  JurorNeeded = new QCheckBox(this);
  JurorNeeded->setText("Jurors needed");
  StenographerNeeded = new QCheckBox(this);
  StenographerNeeded->setText("Stenographer needed");

  FormLayout->setWidget(1, QFormLayout::FieldRole, DefenceNeeded);
  FormLayout->setWidget(2, QFormLayout::FieldRole, ProsecutorNeeded);
  FormLayout->setWidget(3, QFormLayout::FieldRole, JudgeNeeded);
  FormLayout->setWidget(4, QFormLayout::FieldRole, JurorNeeded);
  FormLayout->setWidget(5, QFormLayout::FieldRole, StenographerNeeded);

  setUpdatesEnabled(true);
}

void AOCaseAnnouncerDialog::ok_pressed()
{
  court->announce_case(CaseTitleLineEdit->text(),
                       DefenceNeeded->isChecked(),
                       ProsecutorNeeded->isChecked(),
                       JudgeNeeded->isChecked(),
                       JurorNeeded->isChecked(),
                       StenographerNeeded->isChecked());

  done(0);
}

void AOCaseAnnouncerDialog::cancel_pressed()
{
  done(0);
}
