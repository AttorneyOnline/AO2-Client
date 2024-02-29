#include "widgets/edit_server_dialog.h"
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

EditServerDialog::EditServerDialog(int index) :
    index(index) // lol
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
            &::EditServerDialog::onSavePressed);
    connect(ui_server_dialog_button, &QDialogButtonBox::rejected, this,
            &EditServerDialog::onCancelPressed);

    // We don't need you.
    FROM_UI(QLabel, server_legacy_lbl);
    FROM_UI(QLineEdit, server_legacy_edit);
    FROM_UI(QPushButton, server_legacy_load_button);

    ui_server_legacy_lbl->setVisible(false);
    ui_server_legacy_edit->setVisible(false);
    ui_server_legacy_load_button->setVisible(false);

    loadEntry();
}

void EditServerDialog::loadEntry()
{
   server_type server = Options::getInstance().favorites().at(index);
   ui_server_display_name_edit->setText(server.name);
   ui_server_hostname_edit->setText(server.ip);
   ui_server_port_box->setValue(server.port);
   ui_server_description_edit->setPlainText(server.desc);
   ui_server_protocol_box->setCurrentIndex(server.socket_type);
}

void EditServerDialog::onSavePressed()
{
    server_type server;
    server.name = ui_server_display_name_edit->text();
    server.ip = ui_server_hostname_edit->text();
    server.port = ui_server_port_box->value();
    server.desc = ui_server_description_edit->toPlainText();
    server.socket_type =
        ui_server_protocol_box->currentIndex() == TCP_INDEX ? TCP : WEBSOCKETS;
    Options::getInstance().updateFavorite(server, index);
    close();
    deleteLater();
}

void EditServerDialog::onCancelPressed()
{
    close();
    deleteLater();
}
