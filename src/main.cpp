
#include "aoapplication.h"

#include "courtroom.h"
#include "datatypes.h"
#include "lobby.h"
#include "networkmanager.h"
#include <QDebug>
#include <QLibraryInfo>
#include <QPluginLoader>
#include <QTranslator>
#include <QResource>

int main(int argc, char *argv[])
{
  qSetMessagePattern("%{type}: %{if-category}%{category}: %{endif}%{message}");

  AOApplication main_app(argc, argv);

  #ifdef ANDROID
  if(QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE")==QtAndroid::PermissionResult::Denied) {
    QtAndroid::requestPermissionsSync({"android.permission.READ_EXTERNAL_STORAGE","android.permission.WRITE_EXTERNAL_STORAGE"});
  }
  #endif

  AOApplication::addLibraryPath(AOApplication::applicationDirPath() + "/lib");
  QResource::registerResource(main_app.get_asset("themes/" + Options::getInstance().theme() + ".rcc"));

  QFontDatabase fontDatabase;
  QDirIterator it(get_base_path() + "fonts",
                  QDirIterator::Subdirectories);
  while (it.hasNext())
    fontDatabase.addApplicationFont(it.next());

  QPluginLoader apngPlugin("qapng");
  if (!apngPlugin.load())
    qCritical() << "QApng plugin could not be loaded";

  QPluginLoader webpPlugin("qwebp");
  if (!webpPlugin.load())
    qCritical() << "QWebp plugin could not be loaded";

  QString p_language =
      Options::getInstance().language();
  if (p_language == "  " || p_language == "")
    p_language = QLocale::system().name();

  QTranslator qtTranslator;
  qtTranslator.load("qt_" + p_language,
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  main_app.installTranslator(&qtTranslator);

  QTranslator appTranslator;
  qDebug() << ":/resource/translations/ao_" + p_language;
  appTranslator.load("ao_" + p_language, ":/resource/translations/");
  main_app.installTranslator(&appTranslator);

  main_app.construct_lobby();
  main_app.net_manager->get_server_list(std::bind(&Lobby::list_servers, main_app.w_lobby));
  main_app.net_manager->send_heartbeat();
  main_app.w_lobby->show();
  return main_app.exec();
}
