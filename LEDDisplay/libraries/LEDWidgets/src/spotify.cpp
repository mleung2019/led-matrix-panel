#include "spotify.h"

void drawSpotify(SpotifyData *sd) {
  // COVER
  display->drawRGBBitmap(
    0, 0, 
    sd->cover, 
    PANEL_LENGTH, PANEL_LENGTH
  ); 
  
  // BLACK RECTANGLE
  display->fillRect(0, 55, 64, 9, 0x0000);
  
  // TRACK INFO
  drawScroller(&sd->trackInfo, 56);
}

void parseSpotify(SpotifyData *sd, StaticJsonDocument<1024> doc) {
  if (!doc["is_active"]) {
    strncpy(
      sd->trackInfo.msg, 
      "No track has been played recently",
      sizeof(sd->trackInfo.msg)
    );
    return;
  }

  const char *trackStr = doc["track_name"].as<const char *>();
  const char *artistStr = doc["artist_name"].as<const char *>();
  sprintf(
    sd->trackInfo.msg, 
    "%s - %s", 
    artistStr,
    trackStr
  );
}