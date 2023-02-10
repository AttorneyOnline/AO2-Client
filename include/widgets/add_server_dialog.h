#pragma once

#ifndef ADD_SERVER_DIALOG_H
#define ADD_SERVER_DIALOG_H

#include "interfaces/server_dialog.h"

class QComboBox;
class QDialogButton;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;

class AddServerDialog : public AttorneyOnline::UI::FavoriteServerDialog {
  Q_OBJECT
public:
  AddServerDialog();
  ~AddServerDialog() = default;

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

private slots:
  void onSavePressed() override;
  void onCancelPressed() override;
  void parseLegacyServerEntry();
};

#endif // ADD_SERVER_DIALOG_H
