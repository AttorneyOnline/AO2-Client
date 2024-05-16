#pragma once

#include "datatypes.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>

class ServerEditorDialog : public QDialog
{
  Q_OBJECT

public:
  ServerEditorDialog(QWidget *parent = nullptr);
  ServerEditorDialog(const ServerInfo &server, QWidget *parent = nullptr);

  ServerInfo currentServerInfo() const;

  void loadServerInfo(ServerInfo server);

private:
  static const QString UI_FILE_PATH;

  QWidget *ui_body;

  QLineEdit *ui_name;
  QLineEdit *ui_hostname;
  QSpinBox *ui_port;
  QComboBox *ui_protocol;
  QPlainTextEdit *ui_description;
  QDialogButtonBox *ui_button_box;

  QLineEdit *ui_legacy_edit;
  QPushButton *ui_parse_legacy;

private Q_SLOTS:
  void parseLegacyEntry();
};
