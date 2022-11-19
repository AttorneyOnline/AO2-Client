#ifndef HARDWARE_FUNCTIONS_H
#define HARDWARE_FUNCTIONS_H

#include <QString>

#include <stdio.h>

#ifdef ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

QString get_hdid();

#endif // HARDWARE_FUNCTIONS_H
