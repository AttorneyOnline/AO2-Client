#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

#include <functional>

#include "debug_functions.h"

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
  auto *msgBox = new QMessageBox;

  msgBox->setAttribute(Qt::WA_DeleteOnClose);
  msgBox->setText(p_message);
  msgBox->setWindowTitle(
      QCoreApplication::translate("debug_functions", "Notice"));

  msgBox->setStandardButtons(QMessageBox::Ok);
  msgBox->setDefaultButton(QMessageBox::Ok);
  msgBox->defaultButton()->setEnabled(false);

  QTimer intervalTimer;
  intervalTimer.setInterval(1000);

  int counter = 3;
  const auto updateCounter = [msgBox, &counter] {
    if (counter <= 0)
      return;
    msgBox->defaultButton()->setText(
        QString("%1 (%2)").arg(QDialogButtonBox::tr("OK")).arg(counter));
    counter--;
  };

  QObject::connect(&intervalTimer, &QTimer::timeout, msgBox, updateCounter);
  intervalTimer.start();
  updateCounter();

  QTimer::singleShot(3000, msgBox, [msgBox, &intervalTimer] {
    msgBox->defaultButton()->setEnabled(true);
    msgBox->defaultButton()->setText(QDialogButtonBox::tr("OK"));
    intervalTimer.stop();
  });

  msgBox->exec();  

}
