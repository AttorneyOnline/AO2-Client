#include "hardware_functions.h"

#include <QDebug>
#include <QtGlobal>

#if defined ANDROID
#include <QAndroidJniObject>
#endif



#if defined ANDROID
QString get_hdid()
{
    QAndroidJniObject myID = QAndroidJniObject::fromString("android_id");
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
    QAndroidJniObject appctx = activity.callObjectMethod("getApplicationContext","()Landroid/content/Context;");
    QAndroidJniObject contentR = appctx.callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
    QAndroidJniObject result = QAndroidJniObject::callStaticObjectMethod("android/provider/Settings$Secure","getString", "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;",contentR.object<jobject>(), myID.object<jstring>());
    QString androidID = result.toString();
    return androidID;
}
#elif QT_VERSION < QT_VERSION_CHECK(5, 11, 0)

#if (defined(_WIN32) || defined(_WIN64))
#include <windows.h>

static DWORD dwVolSerial;
static BOOL bIsRetrieved;

QString get_hdid()
{
  bIsRetrieved = GetVolumeInformation(TEXT("C:\\"), nullptr, 0, &dwVolSerial,
                                      nullptr, nullptr, nullptr, 0);

  if (bIsRetrieved)
    return QString::number(dwVolSerial, 16);
  else
    // a totally random string
    // what could possibly go wrong
    return "gxsps32sa9fnwic92mfbs0";
}

#elif (defined(LINUX) || defined(__linux__))

#include <QFile>
#include <QTextStream>

QString get_hdid()
{
  QFile fstab_file("/etc/fstab");
  if (!fstab_file.open(QIODevice::ReadOnly))
    return "gxcps32sa9fnwic92mfbs0";

  QTextStream in(&fstab_file);

  while (!in.atEnd()) {
    QString line = in.readLine();

    if (line.startsWith("UUID")) {
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
