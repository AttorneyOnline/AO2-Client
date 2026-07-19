#include "serverinfo.h"

QString ServerInfo::toString() const
{
  return QString("%1 (%2:%3)").arg((name.isEmpty() ? QStringLiteral("Unnamed Server") : name), address, QString::number(port));
}
