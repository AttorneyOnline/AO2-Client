#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include <QTextEdit>
#include <QWidget>

class AOApplication;

class ModeratorDialog : public QWidget
{
  Q_OBJECT

public:
  static const QString UI_FILE_PATH;

  explicit ModeratorDialog(int clientId, bool ban, AOApplication *ao_app, QWidget *parent = nullptr);
  virtual ~ModeratorDialog();

private:
  AOApplication *ao_app;
  int m_client_id;
  bool m_ban;

  QWidget *ui_widget;
  QComboBox *ui_action;
  QSpinBox *ui_duration;
  QLabel *ui_duration_label;
  QCheckBox *ui_permanent;
  QTextEdit *ui_details;
  QDialogButtonBox *ui_button_box;

private Q_SLOTS:
  void onAcceptedClicked();
};
