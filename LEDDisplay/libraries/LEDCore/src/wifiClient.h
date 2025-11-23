#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include "widgetManager.h"

#define SERVER_IP "100.116.126.182"
#define SERVER_PORT 5001

extern volatile bool networkCancel;

void connectWiFi(const char *ssid, const char *password);
void initLocation();
int writeURLtoBitmap(const char *url, uint16_t *frame, int imgLength);

/* ----------------------- WIDGET HELPER FUNCTIONS ----------------------- */
int fetchWidget(Widget *w, void *data);
int parseWeatherIcon(WeatherData *wd);
int parseSpotifyCover(SpotifyData *sd);
int parseSportsIcons(SportsData *pd);

#endif