#include "win32_functions.h"

DWORD dwVolSerial;
BOOL bIsRetrieved;

QString get_hdid()
{
    bIsRetrieved = GetVolumeInformation(TEXT("C:\\"), NULL, NULL, &dwVolSerial, NULL, NULL, NULL, NULL);

    if (bIsRetrieved)
        return QString::number(dwVolSerial, 16);
    else
        return "invalidhd"; //what could possibly go wrong

}
