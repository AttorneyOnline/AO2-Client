
#include "aoapplication.h"

#include "datatypes.h"
#include "lobby.h"
#include "widgets/courtroom.h"
#include <QPluginLoader>
#include <QDebug>
#include <QStyleFactory>
#include <QDirIterator>

//Q_IMPORT_PLUGIN(ApngImagePlugin);

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);

static void customMessageHandler(QtMsgType type, const QMessageLogContext &context,
                                 const QString &msg)
{
  static const QString before = "AttorneyOnline::", after = "AO::";
  QString function = QString::fromLocal8Bit(context.function);
  function.replace(before, after);
  const std::string functionName = function.toStdString();
  QMessageLogContext newContext(context.file, context.line, functionName.c_str(), context.category);
  (*QT_DEFAULT_MESSAGE_HANDLER)(type, newContext, msg);
}

void loadFonts() {
  QFontDatabase fontDatabase;
  QDirIterator it(":/resource/fonts", QDirIterator::Subdirectories);
  while (it.hasNext())
    fontDatabase.addApplicationFont(it.next());
}

void loadTheme(QApplication &app) {
  app.setStyle(QStyleFactory::create("Fusion"));

  QPalette darkPalette;
  darkPalette.setColor(QPalette::Window, QColor(61, 61, 61));
  darkPalette.setColor(QPalette::WindowText, Qt::white);
  darkPalette.setColor(QPalette::Base, QColor(34, 20, 9));
  darkPalette.setColor(QPalette::AlternateBase, QColor(73, 45, 23));
  darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
  darkPalette.setColor(QPalette::ToolTipText, Qt::white);
  darkPalette.setColor(QPalette::Text, Qt::white);
  darkPalette.setColor(QPalette::ButtonText, Qt::white);
  darkPalette.setColor(QPalette::BrightText, QColor(186, 153, 0));
  darkPalette.setColor(QPalette::Link, QColor(186, 83, 0));

  darkPalette.setColor(QPalette::Button, QColor(101, 45, 0));
  darkPalette.setColor(QPalette::Mid, QColor(50, 25, 5));
  darkPalette.setColor(QPalette::Dark, QColor(34, 20, 9));

  darkPalette.setColor(QPalette::Highlight, QColor(186, 83, 0));
  darkPalette.setColor(QPalette::HighlightedText, Qt::black);

  app.setPalette(darkPalette);
  app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
}

int main(int argc, char *argv[])
{
  qSetMessagePattern("[%{time process}] [%{function}] %{message}");
  qInstallMessageHandler(customMessageHandler);

  AOApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  AOApplication app(argc, argv);
  loadTheme(app);
  loadFonts();

  app.openLobby();
  app.connectToMaster();
  return app.exec();
}
