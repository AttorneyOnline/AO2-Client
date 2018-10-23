#ifndef AOCASEANNOUNCERDIALOG_H
#define AOCASEANNOUNCERDIALOG_H

#include "aoapplication.h"
#include "courtroom.h"

#include <QtWidgets/QDialog>
#include <QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

class AOCaseAnnouncerDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AOCaseAnnouncerDialog(QWidget *parent = nullptr, AOApplication *p_ao_app = nullptr, Courtroom *p_court = nullptr);

private:
  AOApplication *ao_app;
  Courtroom *court;

  QDialogButtonBox *AnnouncerButtons;

  QVBoxLayout *VBoxLayout;
  QFormLayout *FormLayout;

  QLabel *CaseTitleLabel;
  QLineEdit *CaseTitleLineEdit;

  QCheckBox *DefenceNeeded;
  QCheckBox *ProsecutorNeeded;
  QCheckBox *JudgeNeeded;
  QCheckBox *JurorNeeded;

public slots:
  void ok_pressed();
  void cancel_pressed();
};

#endif // AOCASEANNOUNCERDIALOG_H
