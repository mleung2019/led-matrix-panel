#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include "widgets.h"

#define PANEL_PIXELS 64*64
#define FRAME_SIZE PANEL_PIXELS*2
#define PAYLOAD_SIZE 1200
#define MAX_PACKETS 16

void connectWiFi(const char *ssid, const char *password);
void writeURLtoBitmap(const char *url, uint16_t *frame, int imgLength);

int fetchWeather(WeatherData *weather);
void fetchWeatherIcon(WeatherData *weather);

#endif
