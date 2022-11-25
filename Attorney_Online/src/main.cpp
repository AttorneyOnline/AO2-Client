
#include "aoapplication.h"

#include "courtroom.h"
#include "datatypes.h"
#include "lobby.h"
#include "networkmanager.h"
#include <QDebug>
#include <QLibraryInfo>
#include <QPluginLoader>
#include <QTranslator>

int main(int argc, char *argv[])
{
  qSetMessagePattern("%{type}: %{if-category}%{category}: %{endif}%{message}");

  AOApplication main_app(argc, argv);

  AOApplication::addLibraryPath(AOApplication::applicationDirPath() + "/lib");

  QFontDatabase fontDatabase;
  QDirIterator it(main_app.get_base_path() + "fonts",
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

  main_app.open_lobby();
  main_app.net_manager->send_heartbeat();
  return main_app.exec();
}
