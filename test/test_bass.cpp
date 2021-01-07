#include <iostream>
#include <cstring>

#include <catch2/catch.hpp>
#include <QString>

#include "bass.h"
#include "bassopus.h"

TEST_CASE("BASS URL streaming", "[bass][noci]") {
  // Sample
  QString url = "https://raw.githubusercontent.com/skyedeving/aocharedit/master/Attorney%20Online%20Character%20Editor/Resources/about.mp3";

  // initialize
  BASS_Init(-1, 44100, 0, 0, nullptr);

  // create stream from url
  HSTREAM stream;
  unsigned int flags = BASS_STREAM_AUTOFREE | BASS_STREAM_STATUS;
  if (url.endsWith(".opus")) {
    stream = BASS_OPUS_StreamCreateURL(url.toStdString().c_str(), 0, flags, nullptr, 0);
  }
  else {
    stream = BASS_StreamCreateURL(url.toStdString().c_str(), 0, flags, nullptr, 0);
  }

  // Log http status
  const char *tags = BASS_ChannelGetTags(stream, BASS_TAG_HTTP);
  if (tags) {
    while(*tags) {
      UNSCOPED_INFO(tags);
      tags += strlen(tags) + 1;
    }
  }

  // Test
  REQUIRE(stream != 0);
  REQUIRE(BASS_ChannelPlay(stream, TRUE) == TRUE);
  // while (BASS_ChannelIsActive(stream) != BASS_ACTIVE_STOPPED); // block test to listen
}
