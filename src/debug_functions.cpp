#include <QMessageBox>
#include <QCoreApplication>

#include "debug_functions.h"

void call_error(QString p_message)
{
  QMessageBox *msgBox = new QMessageBox;

  msgBox->setText(QCoreApplication::translate("debug_functions", "Error: %1").arg(p_message));
  msgBox->setWindowTitle(QCoreApplication::translate("debug_functions", "Error"));


  //msgBox->setWindowModality(Qt::NonModal);
  msgBox->exec();
}

void call_notice(QString p_message)
{
  QMessageBox *msgBox = new QMessageBox;

  msgBox->setText(p_message);
  msgBox->setWindowTitle(QCoreApplication::translate("debug_functions", "Notice"));


  //msgBox->setWindowModality(Qt::NonModal);
  msgBox->exec();
}
