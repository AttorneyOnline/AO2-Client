
#include "aoapplication.h"

#include "courtroom.h"
#include "datatypes.h"
#include "lobby.h"
#include "networkmanager.h"
#include "mediatester.h"

#include <QDebug>
#include <QFontDatabase>
#include <QLibraryInfo>
#include <QPluginLoader>
#include <QTranslator>
#include <QResource>
#include <QStyleFactory>


int main(int argc, char *argv[])
{
#ifndef Q_OS_MACOS
  if (qgetenv("QT_FONT_DPI").isEmpty()) {
      qputenv("QT_FONT_DPI", "96");
  }
#endif

  qSetMessagePattern("%{type}: %{if-category}%{category}: %{endif}%{message}");

  AOApplication main_app(argc, argv);

  #ifdef ANDROID
  if(QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE")==QtAndroid::PermissionResult::Denied) {
    QtAndroid::requestPermissionsSync({"android.permission.READ_EXTERNAL_STORAGE","android.permission.WRITE_EXTERNAL_STORAGE"});
  }
  #endif

  AOApplication::addLibraryPath(AOApplication::applicationDirPath() + "/lib");
  main_app.ensureValidTheme();
  QResource::registerResource(main_app.get_asset("themes/" + Options::getInstance().theme() + ".rcc"));

  // Bundle the emoji font with the application instead of relying on the host
  // system to provide one (Windows has Segoe UI Emoji, minimal Linux installs
  // frequently have no emoji font at all).
  const int emoji_font_id =
      QFontDatabase::addApplicationFont(":/resource/fonts/NotoColorEmoji.ttf");
  if (emoji_font_id == -1) {
    qWarning() << "Failed to load bundled emoji font";
  }
  else {
    const QStringList emoji_families =
        QFontDatabase::applicationFontFamilies(emoji_font_id);
    for (const QString &family : emoji_families) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
      QFontDatabase::addApplicationEmojiFontFamily(family);
#else
      QFontDatabase::addApplicationFallbackFontFamily(QChar::Script_Common,
                                                       family);
#endif
    }
  }

  QFont main_font = main_app.font();
  main_app.default_font = main_font;

  QFont new_font = main_font;
  int new_font_size = static_cast<int>(main_app.default_font.pointSize() * Options::getInstance().themeScalingFactor());
  new_font.setPointSize(new_font_size);
  main_app.setFont(new_font);

  QFontDatabase fontDatabase;
  QDirIterator it(get_base_path() + "fonts",
                  QDirIterator::Subdirectories);
  while (it.hasNext())
    fontDatabase.addApplicationFont(it.next());

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

  MediaTester *l_media_tester = new MediaTester(&main_app);
  QObject::connect(l_media_tester, SIGNAL(done()), l_media_tester, SLOT(deleteLater()));

  main_app.construct_lobby();
  main_app.net_manager->get_server_list(std::bind(&Lobby::list_servers, main_app.w_lobby));
  main_app.net_manager->send_heartbeat();
  main_app.w_lobby->show();

  if (!main_app.ensureValidTheme())
    main_app.promptMissingThemes();

  return main_app.exec();
}
