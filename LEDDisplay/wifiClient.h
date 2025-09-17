#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include "widgets.h"

#define DEGREE_SYMBOL 0xF8

void connectWiFi(const char *ssid, const char *password);
void writeURLtoBitmap(const char *url, uint16_t *frame, int imgLength);

int fetchWeather(WeatherData *weather);
void fetchWeatherIcon(WeatherData *weather);

#endif
