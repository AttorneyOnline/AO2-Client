
#include "aoapplication.h"

#include "datatypes.h"
#include "networkmanager.h"
#include "lobby.h"
#include "courtroom.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    AOApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    AOApplication main_app(argc, argv);
    main_app.construct_lobby();
    main_app.net_manager->connect_to_master();
    main_app.w_lobby->show();

    return main_app.exec();
}
