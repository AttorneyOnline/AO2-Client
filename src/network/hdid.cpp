#include "network/hdid.h"

#include <QUuid>

namespace AttorneyOnline {

#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>

QString hdid()
{
  DWORD dwVolSerial;
  BOOL bIsRetrieved;

  bIsRetrieved = GetVolumeInformationW(L"C:\\", nullptr, 0, &dwVolSerial,
                                       nullptr, nullptr, nullptr, 0);

  if (bIsRetrieved)
    return QString::number(dwVolSerial, 16);
  else
    return QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
}

#elif (defined (LINUX) || defined (__linux__))
#include <QFile>
#include <QTextStream>

QString hdid()
{
  QFile fstab_file("/etc/fstab");
  if (!fstab_file.open(QIODevice::ReadOnly))
    return QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);

  QTextStream in(&fstab_file);

  while (!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("UUID"))
    {
      QStringList line_elements = line.split("=");

      if (line_elements.size() > 1)
        return line_elements.at(1).left(23).trimmed();
    }
  }
  return QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
}

#else

#error This operating system is unsupported for hardware functions.

#endif

} // namespace AttorneyOnline
