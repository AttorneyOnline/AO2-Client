#include <catch2/catch.hpp>

#include <QPluginLoader>
#include <QImageReader>
#include <QCoreApplication>

TEST_CASE("Support APNG Plugin", "[apng]") {
  // Check paths for libs
  QCoreApplication::addLibraryPath(".");
  QCoreApplication::addLibraryPath("lib");

  // Either it's loaded from system or we load local
  QPluginLoader apngPlugin("qapng");
  apngPlugin.load();

  INFO(QImageReader::supportedImageFormats().join(' ').toStdString());
  REQUIRE(QImageReader::supportedImageFormats().contains("apng"));
}
