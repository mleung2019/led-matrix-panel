#include "weather.h"

#define DEGREE_SYMBOL 0xF8

void drawWeather(WeatherData *wd) {
  // CITY
  drawScroller(&wd->city, 1);

  // TIME AND CURRENT TEMPERATURE
  drawCenteredText(wd->time, 23, 38);
  drawCenteredText(wd->currentTemp, 34, 38);
  
  // HIGH AND LOW TEMP
  drawCenteredText(wd->hiloTemp, 46);
  
  // STATUS ICON
  display->drawRGBBitmap(
    36, 20, 
    wd->statusIcon, 
    ICON_LENGTH, ICON_LENGTH
  ); 
  
  // STATUS DESCRIPTION
  drawScroller(&wd->statusDesc, 11);

  // 5 HOUR FORECAST
  drawScroller(&wd->forecastStr, 56);
}

int parseWeather(WeatherData *wd, StaticJsonDocument<1024> doc) {
  strncpy(wd->city.msg, doc["city"], sizeof(wd->city.msg));
  strncpy(wd->time, doc["time_str"], sizeof(wd->time));

  sprintf(
    wd->currentTemp, 
    "%d%c", 
    (int) doc["curr_temp"],
    DEGREE_SYMBOL
  );

  // Don't display degree symbol if triple digit high
  if ((int) doc["high_temp"] < 100) {
    sprintf(
      wd->hiloTemp, "H:%d%cL:%d%c",
      (int) doc["high_temp"], DEGREE_SYMBOL,
      (int) doc["low_temp"], DEGREE_SYMBOL
    );
  } else {
    sprintf(
      wd->hiloTemp, "H:%d L:%d",
      (int) doc["high_temp"], (int) doc["low_temp"]
    );
  }

  strncpy(wd->statusDesc.msg, doc["status"], sizeof(wd->statusDesc.msg));

  for (int i = 0; i < 5; i++) {
    const char *timeStr = doc["five_hr_log"][i]["time_str"].as<const char *>();
    int temp = doc["five_hr_log"][i]["temp"];
    size_t used = strlen(wd->forecastStr.msg);
    if (used < SCROLLER_SIZE - 1) {
      snprintf(wd->forecastStr.msg + used, SCROLLER_SIZE - used,
      "%s:%d%c ", timeStr, temp, DEGREE_SYMBOL);
    }
  }

  return doc["needs_icon"];
}