#include <QMessageBox>

#include "debug_functions.h"

void call_error(QString p_message)
{
  QMessageBox *f_box = new QMessageBox;

  f_box->setText("Error: " + p_message);
  f_box->setWindowTitle("Error");

  //msgBox->setWindowModality(Qt::NonModal);
  f_box->exec();
  delete f_box;
}

void call_notice(QString p_message)
{
  QMessageBox *f_box = new QMessageBox;

  f_box->setText(p_message);
  f_box->setWindowTitle("Notice");

  //msgBox->setWindowModality(Qt::NonModal);
  f_box->exec();
  delete f_box;
}
