#pragma once

#include "interfaces/server_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>

class EditServerDialog : public AttorneyOnline::UI::FavoriteServerDialog
{
  Q_OBJECT

public:
  EditServerDialog(int index);
  ~EditServerDialog() = default;

private:
  QWidget *ui_widget;

  QLineEdit *ui_server_display_name_edit;
  QLineEdit *ui_server_hostname_edit;
  QSpinBox *ui_server_port_box;
  QComboBox *ui_server_protocol_box;
  QPlainTextEdit *ui_server_description_edit;
  QDialogButtonBox *ui_server_dialog_button;

  // Legacy Server UI
  QLabel *ui_server_legacy_lbl;
  QLineEdit *ui_server_legacy_edit;
  QPushButton *ui_server_legacy_load_button;

  int index;
  void loadEntry();

private Q_SLOTS:
  void onSavePressed() override;
  void onCancelPressed() override;
};
