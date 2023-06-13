#include "widgets/add_server_dialog.h"
#include "datatypes.h"
#include "options.h"

#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFile>
#include <QLineEdit>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QUiLoader>
#include <QVBoxLayout>

#define FROM_UI(type, name)                                                    \
  ;                                                                            \
  ui_##name = findChild<type *>(#name);

AddServerDialog::AddServerDialog()
{
  QUiLoader l_loader(this);
  QFile l_uiFile(Options::getInstance().getUIAsset(DEFAULT_UI));

  if (!l_uiFile.open(QFile::ReadOnly)) {
    qCritical() << "Unable to open file " << l_uiFile.fileName();
    return;
  }
  ui_widget = l_loader.load(&l_uiFile, this);

  auto l_layout = new QVBoxLayout(this);
  l_layout->addWidget(ui_widget);

  FROM_UI(QLineEdit, server_display_name_edit);
  FROM_UI(QLineEdit, server_hostname_edit);
  FROM_UI(QSpinBox, server_port_box);
  FROM_UI(QComboBox, server_protocol_box);
  FROM_UI(QPlainTextEdit, server_description_edit);
  FROM_UI(QDialogButtonBox, server_dialog_button);
  connect(ui_server_dialog_button, &QDialogButtonBox::accepted, this,
          &::AddServerDialog::onSavePressed);
  connect(ui_server_dialog_button, &QDialogButtonBox::rejected, this,
          &AddServerDialog::onCancelPressed);

  FROM_UI(QLabel, server_legacy_lbl);
  FROM_UI(QLineEdit, server_legacy_edit);
  FROM_UI(QPushButton, server_legacy_load_button);
  connect(ui_server_legacy_load_button, &QPushButton::released, this,
          &AddServerDialog::parseLegacyServerEntry);
}

void AddServerDialog::onSavePressed()
{
  server_type server;
  server.name = ui_server_display_name_edit->text();
  server.ip = ui_server_hostname_edit->text();
  server.port = ui_server_port_box->value();
  server.desc = ui_server_description_edit->toPlainText();
  server.socket_type =
      ui_server_protocol_box->currentIndex() == TCP_INDEX ? TCP : WEBSOCKETS;
  Options::getInstance().addFavorite(server);
  close();
}

void AddServerDialog::onCancelPressed()
{
    close();
    deleteLater();
}

void AddServerDialog::parseLegacyServerEntry()
{
  QStringList l_legacy_entry = ui_server_legacy_edit->text().split(":");
  server_type l_server_entry;
  if (l_legacy_entry.isEmpty()) {
    qDebug() << "Legacy entry empty.";
    return;
  }

  int l_item_count = l_legacy_entry.size();

  if (l_item_count >= 3) {
    ui_server_hostname_edit->setText(l_legacy_entry.at(0));
    ui_server_port_box->setValue(l_legacy_entry.at(1).toInt());
    ui_server_display_name_edit->setText(l_legacy_entry.at(2));
    if (l_item_count >= 4) {
      if (l_legacy_entry.at(3) == "ws") {
        ui_server_protocol_box->setCurrentIndex(1);
      }
      else {
        ui_server_protocol_box->setCurrentIndex(0);
      }
    }
  }
}
