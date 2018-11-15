
#include "aoapplication.h"

#include "datatypes.h"
#include "networkmanager.h"
#include "lobby.h"
#include "courtroom.h"

#include <QDebug>

//this is a quite broad generalization
//the most common OSes(mac and windows) are _usually_ case insensitive
//however, there do exist mac installations with case sensitive filesystems
//in that case, define CASE_SENSITIVE_FILESYSTEM and compile on a mac
#if (defined (LINUX) || defined (__linux__))
#define CASE_SENSITIVE_FILESYSTEM
#endif

int main(int argc, char *argv[])
{
#if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
    // High-DPI support is for Qt version >=5.6.
    // However, many Linux distros still haven't brought their stable/LTS
    // packages up to Qt 5.6, so this is conditional.
    AOApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    AOApplication main_app(argc, argv);
    main_app.construct_lobby();
    main_app.net_manager->connect_to_master();
    main_app.w_lobby->show();

    return main_app.exec();
}
