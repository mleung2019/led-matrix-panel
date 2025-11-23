#ifndef WEATHER_H
#define WEATHER_H

#include <widgetTool.h>

struct WeatherData {
  // Location
  Scroller city;
  char time[16];
  // Temperature
  char currentTemp[8];
  char hiloTemp[16];
  // Status
  uint16_t statusIcon[ICON_PIXELS];
  Scroller statusDesc;
  Scroller forecastStr;
};

void drawWeather(WeatherData *wd);
void parseWeather(WeatherData *wd, StaticJsonDocument<1024> doc);

#endif