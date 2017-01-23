#ifndef WIN32_FUNCTIONS_H
#define WIN32_FUNCTIONS_H

#include <QString>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif // Q_OS_WIN32

#include <stdio.h>

QString get_hdid();

#endif // WIN32_FUNCTIONS_H
