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

TEST_CASE("Detect png animation", "[apng][noci]") {
  // Required for QPixmap methods
  int argc = 1;
  char bin[] = "test";
  char *argv[] = { bin };
  QGuiApplication app(argc, argv);

  // Detect apng supports animation
  QImageReader a("snackoo.png", "apng");
  REQUIRE(a.supportsAnimation());
  REQUIRE(!QPixmap::fromImage(a.read()).isNull());

  // Detect png frame has no animation
  QImageReader p("snackoo-frame.png", "apng");
  REQUIRE(!p.supportsAnimation());
  p.setFormat("png");
  REQUIRE(!QPixmap::fromImage(p.read()).isNull());

  // Auto detect fails on apng
  QImageReader d;
  d.setDecideFormatFromContent(true);
  d.setFileName("snackoo.png");
  REQUIRE(!d.supportsAnimation());
  REQUIRE(!QPixmap::fromImage(d.read()).isNull());

  // Decide format fom content fails on apng
  QImageReader c;
  c.setDecideFormatFromContent(true);
  c.setFileName("snackoo.png");
  REQUIRE(!c.supportsAnimation());
  REQUIRE(!QPixmap::fromImage(c.read()).isNull());
}
