
#include "aoapplication.h"

#include "courtroom.h"
#include "file_functions.h"
#include "lobby.h"

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

  QApplication app(argc, argv);

#ifdef ANDROID
  if (QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE") == QtAndroid::PermissionResult::Denied)
  {
    QtAndroid::requestPermissionsSync({"android.permission.READ_EXTERNAL_STORAGE", "android.permission.WRITE_EXTERNAL_STORAGE"});
  }
#endif

  AOApplication main_app;
  QApplication::setApplicationVersion(AOApplication::get_version_string());
  QApplication::setApplicationDisplayName(QObject::tr("Attorney Online %1").arg(QApplication::applicationVersion()));

  QResource::registerResource(main_app.get_asset("themes/" + Options::getInstance().theme() + ".rcc"));

  QFont main_font = QApplication::font();
  main_app.default_font = main_font;

  QFont new_font = main_font;
  int new_font_size = main_app.default_font.pointSize() * Options::getInstance().themeScalingFactor();
  new_font.setPointSize(new_font_size);
  QApplication::setFont(new_font);

  QStringList font_paths;
  font_paths.append(get_base_path());
  font_paths.append(Options::getInstance().mountPaths());

  for (const QString &path : font_paths)
  {
    QDirIterator it(path + "fonts", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
      QFontDatabase::addApplicationFont(it.next());
    }
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
  if (!qtTranslator.load("qt_" + p_language, QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
  {
    qDebug() << "Failed to load translation qt_" + p_language;
  }
  else
  {
    QApplication::installTranslator(&qtTranslator);
  }

  QTranslator appTranslator;
  if (!appTranslator.load("ao_" + p_language, ":/data/translations/"))
  {
    qDebug() << "Failed to load translation ao_" + p_language;
  }
  else
  {
    QApplication::installTranslator(&appTranslator);
    qDebug() << ":/data/translations/ao_" + p_language;
  }

  main_app.construct_lobby();
  main_app.net_manager->get_server_list();
  main_app.net_manager->send_heartbeat();
  main_app.w_lobby->show();

  return QApplication::exec();
}
