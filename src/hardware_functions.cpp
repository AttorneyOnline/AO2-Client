#include "hardware_functions.h"

#include <QDebug>
#include <QtGlobal>

#if (defined(_WIN32) || defined(_WIN64))
#include <windows.h>
#include <sddl.h>

QString get_hdid()
{
    HANDLE hToken;
    HANDLE pHandle;
    PTOKEN_USER pToken;
    DWORD uSize = 0;
    LPWSTR HDIDParam;

    pHandle = GetCurrentProcess();
    OpenProcessToken(pHandle, TOKEN_QUERY, &hToken);
    if (!GetTokenInformation(hToken, (TOKEN_INFORMATION_CLASS)1, NULL, 0, &uSize))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            return "gxsps32sa9fnwic92mfbs1";
        }
    }

    pToken = (PTOKEN_USER)GlobalAlloc(GPTR, uSize);

    if (!GetTokenInformation(hToken, (TOKEN_INFORMATION_CLASS)1, pToken, uSize, &uSize))
    {
        return "gxsps32sa9fnwic92mfbs2";
    }

    ConvertSidToStringSidW(pToken->User.Sid, &HDIDParam);
    QString returnHDID = QString::fromWCharArray(HDIDParam);
    CloseHandle(hToken);
    return returnHDID;
}
#elif QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
#if (defined(LINUX) || defined(__linux__))

#include <QFile>
#include <QTextStream>

QString get_hdid()
{
  QFile fstab_file("/etc/fstab");
  if (!fstab_file.open(QIODevice::ReadOnly))
    return "uxcps32sa9fnwic92mfbs0";

  QTextStream in(&fstab_file);

  while (!in.atEnd()) {
    QString line = in.readLine();

    if (line.startsWith("UUID")) {
      QStringList line_elements = line.split("=");

      if (line_elements.size() > 1)
        return line_elements.at(1).left(23).trimmed();
    }
  }

  return "uxcpz32sa9fnwic92mfbs1";
}

#elif defined __APPLE__

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

QString get_hdid()
{
  CFStringRef serial;
  char buffer[64] = {0};
  QString hdid;
  io_service_t platformExpert = IOServiceGetMatchingService(
      kIOMasterPortDefault, IOServiceMatching("IOPlatformExpertDevice"));
  if (platformExpert) {
    CFTypeRef serialNumberAsCFString = IORegistryEntryCreateCFProperty(
        platformExpert, CFSTR(kIOPlatformSerialNumberKey), kCFAllocatorDefault,
        0);
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

#else
#include <QSysInfo>

QByteArray machineId;

QString get_hdid()
{
  machineId = QSysInfo::machineUniqueId();
  return QString(machineId);
}

#endif
