#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <HTTPClient.h>

#include "config.h"
#include "widgetManager.h"

extern volatile bool networkCancel;

void connectWiFi();
void beginWithKey(HTTPClient &http, const String &url);
int initLocation();
int writeURLtoBitmap(const char *url, uint16_t *frame, int imgLength);

/* ----------------------- WIDGET HELPER FUNCTIONS ----------------------- */
int fetchWidget(Widget *w, void *data);
int parseWeatherIcon(WeatherData *wd);
int parseSpotifyCover(SpotifyData *sd);
int parseSportsIcons(SportsData *pd);

#endif