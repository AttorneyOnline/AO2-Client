#include "moderation_functions.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QObject>

std::optional<QString> call_moderator_support(QString title)
{
  if (title.isEmpty())
  {
    title = QObject::tr("Call moderator");
  }
  else
  {
    title = QObject::tr("Call moderator: %1").arg(title);
  }

  QInputDialog input;
  input.setLabelText(QObject::tr("Reason:"));
  input.setWindowFlags(Qt::WindowSystemMenuHint);
  input.setWindowTitle(title);

  while (input.exec())
  {
    QString text = input.textValue();
    if (text.isEmpty())
    {
      QMessageBox::critical(&input, QObject::tr("Error"), QObject::tr("Please, enter a reason."));
    }
    else if (text.length() > 255)
    {
      QMessageBox::critical(&input, QObject::tr("Error"), QObject::tr("Reason is too long."));
    }
    else
    {
      return text;
    }
  }

  return std::nullopt;
}
