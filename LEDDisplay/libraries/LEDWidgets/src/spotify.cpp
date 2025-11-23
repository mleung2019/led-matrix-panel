#include "spotify.h"

void drawSpotify(SpotifyData *sd) {
  updateScroller(&sd->trackInfo);

  // COVER
  display->drawRGBBitmap(
    0, 0, 
    sd->cover, 
    PANEL_LENGTH, PANEL_LENGTH
  ); 

  // TRACK INFO
  resizeScroller(&sd->trackInfo);
  sd->trackInfo.y = 56;

  display->fillRect(0, 55, 64, 9, 0x0000);
  drawScroller(&sd->trackInfo);
}

void parseSpotify(SpotifyData *sd, StaticJsonDocument<1024> doc) {
  if (!doc["is_active"]) {
    sprintf(
      sd->trackInfo.msg, 
      "No track has been played recently"
    );
    return;
  }

  const char *trackStr = doc["track_name"].as<const char *>();
  const char *artistStr = doc["artist_name"].as<const char *>();
  
  Serial.println(artistStr);

  sprintf(
    sd->trackInfo.msg, 
    "%s - %s", 
    artistStr,
    trackStr
  );
}