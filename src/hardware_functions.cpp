#include "hardware_functions.h"

#include <QDebug>

#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>

static DWORD dwVolSerial;
static BOOL bIsRetrieved;

QString get_hdid()
{
  bIsRetrieved = GetVolumeInformation(TEXT("C:\\"), nullptr, 0, &dwVolSerial, nullptr, nullptr, nullptr, 0);

  if (bIsRetrieved)
    return QString::number(dwVolSerial, 16);
  else
    //a totally random string
    //what could possibly go wrong
    return "gxsps32sa9fnwic92mfbs0";
}

#elif (defined (LINUX) || defined (__linux__))

#include <QFile>
#include <QTextStream>

QString get_hdid()
{
  QFile fstab_file("/etc/fstab");
  if (!fstab_file.open(QIODevice::ReadOnly))
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

#elif defined __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

QString get_hdid()
{
    CFStringRef serial;
        char buffer[64] = {0};
        QString hdid;
        io_service_t platformExpert = IOServiceGetMatchingService(kIOMasterPortDefault,
                                                              IOServiceMatching("IOPlatformExpertDevice"));
        if (platformExpert)
        {
            CFTypeRef serialNumberAsCFString = IORegistryEntryCreateCFProperty(platformExpert,
                                                                           CFSTR(kIOPlatformSerialNumberKey),
                                                                           kCFAllocatorDefault, 0);
            if (serialNumberAsCFString) {
                serial = (CFStringRef)serialNumberAsCFString;
            }
            if (CFStringGetCString(serial, buffer, 64, kCFStringEncodingUTF8)) {
                hdid = buffer;
            }

            IOObjectRelease(platformExpert);
        }
        return hdid;
}

#else

#error This operating system is unsupported for hardware functions.

#endif
