#include "hardware_functions.h"

#if (defined (_WIN32) || defined (_WIN64))
DWORD dwVolSerial;
BOOL bIsRetrieved;

QString get_hdid()
{
    bIsRetrieved = GetVolumeInformation(TEXT("C:\\"), NULL, NULL, &dwVolSerial, NULL, NULL, NULL, NULL);

    if (bIsRetrieved)
        return QString::number(dwVolSerial, 16);
    else
        return "invalid_windows_hd"; //what could possibly go wrong

}

#elif (defined (LINUX) || defined (__linux__))

QString get_hdid()
{
  //T0D0: add linux implementation
  return "linux_os_hdid";
}

#else

QString get_hdid()
{
  //T0D0: find a sane way to handle this
  return "unknown_os_hdid";
}

#endif
