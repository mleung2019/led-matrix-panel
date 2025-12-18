#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <widgetTool.h>

struct SpotifyData {
  uint16_t cover[PANEL_PIXELS];
  Scroller trackInfo;
};

void drawSpotify(SpotifyData *sd);
int parseSpotify(SpotifyData *sd, StaticJsonDocument<1024> doc);

#endif