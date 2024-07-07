#include "server_editor_dialog.h"

#include "datatypes.h"
#include "debug_functions.h"
#include "gui_utils.h"
#include "options.h"

#include <QDebug>
#include <QFile>
#include <QUiLoader>
#include <QVBoxLayout>

const QString ServerEditorDialog::UI_FILE_PATH = "favorite_server_dialog.ui";

ServerEditorDialog::ServerEditorDialog(QWidget *parent)
    : QDialog(parent)
{
  QUiLoader loader(this);
  QFile file(Options::getInstance().getUIAsset(UI_FILE_PATH));

  if (!file.open(QFile::ReadOnly))
  {
    qFatal("Unable to open file %s", qPrintable(file.fileName()));
    return;
  }
  ui_body = loader.load(&file, this);

  auto layout = new QVBoxLayout(this);
  layout->addWidget(ui_body);

  FROM_UI(QLineEdit, name);
  FROM_UI(QLineEdit, hostname);
  FROM_UI(QSpinBox, port);
  FROM_UI(QPlainTextEdit, description);
  FROM_UI(QDialogButtonBox, button_box);

  FROM_UI(QLineEdit, legacy_edit);
  FROM_UI(QPushButton, parse_legacy);

  connect(ui_parse_legacy, &QPushButton::released, this, &ServerEditorDialog::parseLegacyEntry);

  connect(ui_button_box, &QDialogButtonBox::accepted, this, &ServerEditorDialog::accept);
  connect(ui_button_box, &QDialogButtonBox::rejected, this, &ServerEditorDialog::reject);
}

ServerEditorDialog::ServerEditorDialog(const ServerInfo &server, QWidget *parent)
    : ServerEditorDialog(parent)
{
  ui_name->setText(server.name);
  ui_hostname->setText(server.address);
  ui_port->setValue(server.port);
  ui_description->setPlainText(server.description);
}

ServerInfo ServerEditorDialog::currentServerInfo() const
{
  ServerInfo server;
  server.name = ui_name->text();
  server.address = ui_hostname->text();
  server.port = ui_port->value();
  server.description = ui_description->toPlainText();
  return server;
}

void ServerEditorDialog::parseLegacyEntry()
{
  QStringList entry = ui_legacy_edit->text().split(":");
  if (entry.size() < 3)
  {
    call_error("Invalid legacy server entry");
    return;
  }

  ui_hostname->setText(entry.at(0));
  ui_port->setValue(entry.at(1).toInt());
  ui_name->setText(entry.at(2));
}
