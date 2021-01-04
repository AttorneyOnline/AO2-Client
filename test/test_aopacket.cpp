#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "aopacket.h"

TEST_CASE("AOPacket construct", "[aopacket]") {
  // Parameters
  QString packet_string = "CT#MY_OOC_NAME#/doc https://docs.google.com/document/d/123/edit##%";
  QString header = "CT";
  QStringList contents = {"MY_OOC_NAME", "/doc https://docs.google.com/document/d/123/edit#"};

  // Packet string only
  AOPacket p(packet_string);
  REQUIRE(p.to_string() == packet_string);

  // Header and Contents Separate
  AOPacket p2(header, contents);
  REQUIRE(p2.to_string() == packet_string);
}

TEST_CASE("AOPacket encode/decode", "[aopacket]") {
  // Parameters
  QString packet_string = "CT#MY_OOC_NAME#/doc https://docs.google.com/document/d/%$&/edit##%";
  QString header = "CT";
  QStringList contents = {"MY_OOC_NAME", "/doc https://docs.google.com/document/d/%$&/edit#"};

  // Encodes that get "sent" to the server
  QString bad_send = "CT#MY_OOC_NAME#/doc https://docs.google.com/document/d/<percent><dollar><and>/edit##%";
  QString good_send = "CT#MY_OOC_NAME#/doc https://docs.google.com/document/d/<percent><dollar><and>/edit<num>#%";

  // Bad encode/decode for docs because the split on '#' after "edit" in the doc url
  AOPacket p(packet_string);
  p.net_encode();
  REQUIRE(p.to_string() == bad_send);

  p.net_decode();
  REQUIRE(p.to_string() == packet_string);

  // Good encode/decode for docs because header and contents are separate
  AOPacket p2(header, contents);

  p2.net_encode();
  REQUIRE(p2.to_string() == good_send);

  p2.net_decode();
  REQUIRE(p2.to_string() == packet_string);
}
