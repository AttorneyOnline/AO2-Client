#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "aopacket.h"

TEST_CASE("AOPacket construct", "[aopacket]") {
  // Parameters
  QString packet_string = "CT#MY_OOC_NAME#/doc https://docs.google.com/document/d/123/edit##%";

  SECTION("Packet string") {
    AOPacket p(packet_string);
    REQUIRE(p.to_string() == packet_string);
  }
  SECTION("Header and contents") {
    QStringList contents = {"MY_OOC_NAME", "/doc https://docs.google.com/document/d/123/edit#"};
    AOPacket p("CT", contents);
    REQUIRE(p.to_string() == packet_string);
  }
}

TEST_CASE("AOPacket encode/decode", "[aopacket]") {
  // Parameters
  QString packet_string = "CT#MY_OOC_NAME#/doc https://docs.google.com/document/d/%$&/edit##%";
  QString good_encode = "CT#MY_OOC_NAME#/doc https://docs.google.com/document/d/<percent><dollar><and>/edit<num>#%";

  SECTION("Bad encode/decode because packet string constructor splits the '#' after 'edit'") {
    AOPacket p(packet_string);
    p.net_encode();
    REQUIRE(p.to_string() != good_encode);

    p.net_decode();
    REQUIRE(p.to_string() == packet_string);
  }

  SECTION("Good encode/decode with header and contents constructor") {
    QStringList contents = {"MY_OOC_NAME", "/doc https://docs.google.com/document/d/%$&/edit#"};
    AOPacket p("CT", contents);

    p.net_encode();
    REQUIRE(p.to_string() == good_encode);

    p.net_decode();
    REQUIRE(p.to_string() == packet_string);
  }
}
