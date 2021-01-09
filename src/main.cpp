
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
#if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
  // High-DPI support is for Qt version >=5.6.
  // However, many Linux distros still haven't brought their stable/LTS
  // packages up to Qt 5.6, so this is conditional.
  AOApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  AOApplication main_app(argc, argv);

  AOApplication::addLibraryPath(AOApplication::applicationDirPath() + "/lib");

  QSettings *configini = main_app.configini;

  QPluginLoader apngPlugin("qapng");
  if (!apngPlugin.load())
    qCritical() << "QApng plugin could not be loaded";

  QPluginLoader webpPlugin("qwebp");
  if (!webpPlugin.load())
    qCritical() << "QWebp plugin could not be loaded";

  QString p_language =
      configini->value("language", QLocale::system().name()).toString();
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
  main_app.net_manager->connect_to_master();
  main_app.w_lobby->show();
  return main_app.exec();
}
