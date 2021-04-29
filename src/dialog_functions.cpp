#include <QCoreApplication>
#include <QMessageBox>
#include <QTimer>
#include <functional>

#include "dialog_functions.h"
#include "networkmanager.h"

void call_error(QString p_message)
{
  QMessageBox *msgBox = new QMessageBox;

  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->setText(QCoreApplication::translate("debug_functions", "Error: %1")
                      .arg(p_message));
  msgBox->setWindowTitle(
      QCoreApplication::translate("debug_functions", "Error"));

  // msgBox->setWindowModality(Qt::NonModal);
  msgBox->exec();
}

void call_notice(QString p_message)
{
  QMessageBox *msgBox = new QMessageBox;

  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->setText(p_message);
  msgBox->setWindowTitle(
      QCoreApplication::translate("debug_functions", "Notice"));

  msgBox->setStandardButtons(QMessageBox::NoButton);

  QTimer::singleShot(3000, msgBox, std::bind(&QMessageBox::setStandardButtons,msgBox,QMessageBox::Ok));

  msgBox->exec();  

}

LoginDialog::LoginDialog(bool username_enabled, QWidget *parent):QDialog(parent)
{
    enable_username = username_enabled;
    setupUi();
    setWindowTitle(tr("Login"));
    setModal(true);
}

void LoginDialog::setupUi()
{
    QGridLayout *formGridLayout = new QGridLayout(this);

    if (enable_username) {
        editUsername = new QLineEdit(this);
        labelUsername = new QLabel(this);
        labelUsername->setText(tr("Username"));
        labelUsername->setBuddy(editUsername);
        formGridLayout->addWidget(labelUsername, 0, 0);
        formGridLayout->addWidget(editUsername, 0, 1);
    }

    editPassword = new QLineEdit(this);
    editPassword->setEchoMode(QLineEdit::Password);
    labelPassword = new QLabel(this);
    labelPassword->setText(tr("Password"));
    labelPassword->setBuddy(editPassword);

    buttons = new QDialogButtonBox(this);
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Login"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(buttons->button(QDialogButtonBox::Cancel),
            SIGNAL(clicked()), this, SLOT(close()));
    connect(buttons->button(QDialogButtonBox::Ok),
            SIGNAL(clicked()), this, SLOT(slotSendLogin()));
    formGridLayout->addWidget(labelPassword, 1, 0);
    formGridLayout->addWidget(editPassword, 1, 1);
    formGridLayout->addWidget(buttons, 2, 0, 1, 2);
    setLayout(formGridLayout);
}

void LoginDialog::slotSendLogin()
{
    QString username;
    if (enable_username)
        username = editUsername->text();
    QString password = editPassword->text();
    emit sendLogin(username, password);
    close();
}
