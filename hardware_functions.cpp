#include "hardware_functions.h"

#include <QDebug>

#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>

DWORD dwVolSerial;
BOOL bIsRetrieved;

QString get_hdid()
{
    bIsRetrieved = GetVolumeInformation(TEXT("C:\\"), NULL, NULL, &dwVolSerial, NULL, NULL, NULL, NULL);

    if (bIsRetrieved)
        return QString::number(dwVolSerial, 16);
    else
        return "gxsps32sa9fnwic92mfbs0"; //what could possibly go wrong

}

#elif (defined (LINUX) || defined (__linux__))

#include <QFile>
#include <QTextStream>

QString get_hdid()
{
  QFile fstab_file("/etc/fstab");
  if (!fstab_file.open(QIODevice::ReadOnly))
    //literally a random string.... what else are we supposed to do?
    return "gxcps32sa9fnwic92mfbs0";

  QTextStream in(&fstab_file);

  while(!in.atEnd())
  {
    QString line = in.readLine();

    if (line.startsWith("UUID"))
    {
      QStringList line_elements = line.split("=");

      if (line_elements.size() > 1)
        return line_elements.at(1).left(23).trimmed();
    }
  }

  return "gxcpz32sa9fnwic92mfbs0";
}

#else

//throwing compile-time errors professionally
fhasdfuifhidfhasjkfasdkfahsdj

#endif
