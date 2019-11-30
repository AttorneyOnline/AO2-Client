
#include "aoapplication.h"

#include "datatypes.h"
#include "lobby.h"
#include "widgets/courtroom.h"
#include <QPluginLoader>
#include <QDebug>

//Q_IMPORT_PLUGIN(ApngImagePlugin);

int main(int argc, char *argv[])
{
  qSetMessagePattern("%{file}:%{line}: %{message}");
#if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
  // High-DPI support is for Qt version >=5.6.
  // However, many Linux distros still haven't brought their stable/LTS
  // packages up to Qt 5.6, so this is conditional.
  AOApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  AOApplication main_app(argc, argv);
  main_app.openLobby();
  main_app.connectToMaster();
  return main_app.exec();
}
