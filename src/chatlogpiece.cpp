#include "chatlogpiece.h"

#include <QStringBuilder>

QString ChatLogPiece::toString()
{
  auto maybe_unknown = [](QString str) -> QString {
    if (str.isEmpty())
    {
      return tr("UNKNOWN");
    }
    else
    {
      return str;
    }
  };

  QString details = QString("[%1] %2").arg(timestamp.toString(), maybe_unknown(character_name));

  if (character_name != character)
  {
    details += " (" % maybe_unknown(character) % ")";
  }

  if (!action.isEmpty())
  {
    details += " " % action;
  }

  details += ": " % maybe_unknown(message);

  return details;
}
