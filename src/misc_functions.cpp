#include "misc_functions.h"

void delay(int p_milliseconds)
{
    QEventLoop loop;
    QTimer t;
    t.connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
    t.start(p_milliseconds);
    loop.exec();
}
