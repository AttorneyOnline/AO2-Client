#include "debug_functions.h"

void call_error(QString p_message)
{
  QMessageBox *msgBox = new QMessageBox;

  msgBox->setText("Error: " + p_message);
  msgBox->setWindowTitle("Error");

  // msgBox->setWindowModality(Qt::NonModal);
  msgBox->exec();
}

void call_notice(QString p_message)
{
  QMessageBox *msgBox = new QMessageBox;

  msgBox->setText(p_message);
  msgBox->setWindowTitle("Notice");

  // msgBox->setWindowModality(Qt::NonModal);
  msgBox->exec();
}
