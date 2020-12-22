
#include "aoapplication.h"

#include "datatypes.h"
#include "lobby.h"
#include "widgets/courtroom.h"
#include <QPluginLoader>
#include <QDebug>
#include <QStyleFactory>

//Q_IMPORT_PLUGIN(ApngImagePlugin);

int main(int argc, char *argv[])
{
  qSetMessagePattern("[%{time process}] [%{function}] %{message}");
#if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
  // High-DPI support is for Qt version >=5.6.
  // However, many Linux distros still haven't brought their stable/LTS
  // packages up to Qt 5.6, so this is conditional.
  AOApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  AOApplication main_app(argc, argv);

  main_app.setStyle(QStyleFactory::create("Fusion"));

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

  main_app.setPalette(darkPalette);
  main_app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

  main_app.openLobby();
  main_app.connectToMaster();
  return main_app.exec();
}
