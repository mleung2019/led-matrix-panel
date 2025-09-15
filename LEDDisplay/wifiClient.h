#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include "widgets.h"

#define PANEL_PIXELS 64*64
#define FRAME_SIZE PANEL_PIXELS*2
#define PAYLOAD_SIZE 1200
#define MAX_PACKETS 16

void connectWiFi(const char *ssid, const char *password);
void fetchWeather(WeatherData *weather);

#endif
