#include <iostream>
#include <cstring>

#include <catch2/catch.hpp>
#include <QString>

#include "bass.h"

TEST_CASE("BASS URL streaming", "[bass]") {
  // Sample
  QString url = "https://raw.githubusercontent.com/skyedeving/aocharedit/master/Attorney%20Online%20Character%20Editor/Resources/about.mp3";
  BASS_Init(-1, 44100, 0, 0, nullptr);
#ifdef _WIN32
  HSTREAM stream = BASS_StreamCreateURL(url.toWStdString().c_str(), 0, BASS_STREAM_STATUS, nullptr, 0);
#else
  HSTREAM stream = BASS_StreamCreateURL(url.toStdString().c_str(), 0, BASS_STREAM_STATUS, nullptr, 0);
#endif
  const char *tags = BASS_ChannelGetTags(stream, BASS_TAG_HTTP);
  if (tags) {
    while(*tags) {
      UNSCOPED_INFO(tags);
      tags += strlen(tags) + 1;
    }
  }
  REQUIRE(stream != 0);
  REQUIRE(BASS_ChannelPlay(stream, TRUE) == TRUE);
  // while (BASS_ChannelIsActive(stream) != BASS_ACTIVE_STOPPED);
}
