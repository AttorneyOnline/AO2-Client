#include <catch2/catch.hpp>

#include <QPluginLoader>
#include <QImageReader>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QPixmap>

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

TEST_CASE("Detect png animation", "[apng]") {
  // Required for QPixmap methods
  int argc = 1;
  char bin[] = "test";
  char *argv[] = { bin };
  QGuiApplication app(argc, argv);

  // Instantiate reader
  QImageReader reader;

  SECTION("Decide format from content fails on apng") {
    reader.setFileName("snackoo.png");
    reader.setDecideFormatFromContent(true);
    REQUIRE(!reader.supportsAnimation());
    REQUIRE(!QPixmap::fromImage(reader.read()).isNull());
  }

  SECTION("Auto detect fails on apng") {
    reader.setFileName("snackoo.png");
    reader.setAutoDetectImageFormat(true);
    REQUIRE(!reader.supportsAnimation());
    REQUIRE(!QPixmap::fromImage(reader.read()).isNull());
  }

  SECTION("Detect apng supports animation") {
    reader.setFileName("snackoo.png");
    reader.setFormat("apng");
    REQUIRE(reader.supportsAnimation());
    REQUIRE(!QPixmap::fromImage(reader.read()).isNull());
  }

  SECTION("Detect png frame has no animation") {
    reader.setFileName("missle.png");
    reader.setFormat("apng");
    REQUIRE(!reader.supportsAnimation());
    reader.setFormat("png");
    REQUIRE(!reader.supportsAnimation());
    REQUIRE(!QPixmap::fromImage(reader.read()).isNull());
  }
}
