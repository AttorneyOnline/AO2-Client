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

LoginDialog::LoginDialog(QWidget *parent):QDialog(parent)
{
    setupUi();
    setWindowTitle(QCoreApplication::translate("dialog_functions", "Login"));
    setModal(true);
}

void LoginDialog::setupUi()
{
    QGridLayout *formGridLayout = new QGridLayout(this);
    editUsername = new QLineEdit(this);
    editPassword = new QLineEdit(this);
    editPassword->setEchoMode(QLineEdit::Password);

    labelUsername = new QLabel(this);
    labelPassword = new QLabel(this);
    labelUsername->setText(QCoreApplication::translate("dialog_functions", "Username"));
    labelUsername->setBuddy(editUsername);
    labelPassword->setText(QCoreApplication::translate("dialog_functions", "Password"));
    labelPassword->setBuddy(editPassword);

    buttons = new QDialogButtonBox(this);
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(QCoreApplication::translate("dialog_functions", "Login"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QCoreApplication::translate("dialog_functions", "Cancel"));

    connect(buttons->button(QDialogButtonBox::Cancel),
            SIGNAL(clicked()), this, SLOT(close()));
    connect(buttons->button(QDialogButtonBox::Ok),
            SIGNAL(clicked()), this, SLOT(slotSendLogin()));
    formGridLayout->addWidget(labelUsername, 0, 0);
    formGridLayout->addWidget(editUsername, 0, 1);
    formGridLayout->addWidget(labelPassword, 1, 0);
    formGridLayout->addWidget(editPassword, 1, 1);
    formGridLayout->addWidget(buttons, 2, 0, 1, 2);
    setLayout(formGridLayout);
}

void LoginDialog::setUsername(QString &username)
{
    editUsername->setText(username);
    editPassword->setFocus();
}

void LoginDialog::setPassword(QString &password)
{
    editPassword->setText(password);
}

void LoginDialog::slotSendLogin()
{
    QString username = editUsername->text();
    QString password = editPassword->text();
    emit sendLogin(username, password);
    close();
}
