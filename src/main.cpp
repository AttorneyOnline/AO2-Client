
#include "aoapplication.h"

#include "courtroom.h"
#include "datatypes.h"
#include "lobby.h"
#include "networkmanager.h"

#include <QDebug>
#include <QDirIterator>
#include <QImageReader>
#include <QLibraryInfo>
#include <QResource>
#include <QTranslator>

int main(int argc, char *argv[])
{
  qSetMessagePattern("%{type}: %{if-category}%{category}: %{endif}%{message}");

  qRegisterMetaType<AOPacket>();

  AOApplication main_app(argc, argv);

#ifdef ANDROID
  if (QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE") == QtAndroid::PermissionResult::Denied)
  {
    QtAndroid::requestPermissionsSync({"android.permission.READ_EXTERNAL_STORAGE", "android.permission.WRITE_EXTERNAL_STORAGE"});
  }
#endif

  QResource::registerResource(main_app.get_asset("themes/" + Options::getInstance().theme() + ".rcc"));

  QFont main_font = main_app.font();
  main_app.default_font = main_font;

  QFont new_font = main_font;
  int new_font_size = main_app.default_font.pointSize() * Options::getInstance().themeScalingFactor();
  new_font.setPointSize(new_font_size);
  main_app.setFont(new_font);

  QFontDatabase fontDatabase;
  QDirIterator it(get_base_path() + "fonts", QDirIterator::Subdirectories);
  while (it.hasNext())
  {
    fontDatabase.addApplicationFont(it.next());
  }

  QStringList expected_formats{"webp", "apng", "gif"};
  for (const QByteArray &i_format : QImageReader::supportedImageFormats())
  {
    if (expected_formats.contains(i_format, Qt::CaseInsensitive))
    {
      expected_formats.removeAll(i_format.toLower());
    }
  }

  if (!expected_formats.isEmpty())
  {
    call_error("Missing image formats: <b>" + expected_formats.join(", ") + "</b>.<br /><br /> Please make sure you have installed the application properly.");
  }

  QString p_language = Options::getInstance().language();
  if (p_language.trimmed().isEmpty())
  {
    p_language = QLocale::system().name();
  }

  QTranslator qtTranslator;
  qtTranslator.load("qt_" + p_language, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  main_app.installTranslator(&qtTranslator);

  QTranslator appTranslator;
  qDebug() << ":/data/translations/ao_" + p_language;
  appTranslator.load("ao_" + p_language, ":/data/translations/");
  main_app.installTranslator(&appTranslator);

  main_app.construct_lobby();
  main_app.net_manager->get_server_list();
  main_app.net_manager->send_heartbeat();
  main_app.w_lobby->show();
  return main_app.exec();
}
