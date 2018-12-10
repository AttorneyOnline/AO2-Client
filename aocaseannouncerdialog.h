#ifndef AOCASEANNOUNCERDIALOG_H
#define AOCASEANNOUNCERDIALOG_H

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
  explicit AOCaseAnnouncerDialog(QWidget *parent = nullptr);

private:
  QDialogButtonBox *ui_announcer_buttons;

  QVBoxLayout *ui_vbox_layout;
  QFormLayout *ui_form_layout;

  QLabel *ui_case_title_label;
  QLineEdit *ui_case_title_textbox;

  QCheckBox *ui_defense_needed;
  QCheckBox *ui_prosecutor_needed;
  QCheckBox *ui_judge_needed;
  QCheckBox *ui_juror_needed;
  QCheckBox *ui_steno_needed;

signals:
  void announce_case(QString* title, bool def, bool pro, bool jud, bool jur, bool steno);

public slots:
  void ok_pressed();
  void cancel_pressed();
};

#endif // AOCASEANNOUNCERDIALOG_H
