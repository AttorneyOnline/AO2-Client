#include <catch2/catch.hpp>

#include <QPluginLoader>
#include <QImageReader>
#include <QCoreApplication>

TEST_CASE("Support APNG Plugin (place lib same path)", "[apng]") {
  QCoreApplication::addLibraryPath(".");
  QPluginLoader apngPlugin("qapng");
  REQUIRE(apngPlugin.load());

  // Fails for some reason on windows and linux don't know about osx
  // apng animation seems to be broken linux qt5-5.15.2
  REQUIRE(QImageReader::supportedImageFormats().contains("APNG"));
}
