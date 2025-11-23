#include "weather.h"

void drawWeather(WeatherData *wd) {
  updateScroller(&wd->statusDesc);
  updateScroller(&wd->forecastStr);

  // CITY
  drawCenteredText(wd->city, 1);
  display->println();

  // STATUS DESCRIPTION
  resizeScroller(&wd->statusDesc);
  wd->statusDesc.y = 11;
  
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

  // 5 HOUR FORECAST
  resizeScroller(&wd->forecastStr);
  wd->forecastStr.y = 56;

  drawScroller(&wd->statusDesc);
  drawScroller(&wd->forecastStr);
}

void parseWeather(WeatherData *wd, StaticJsonDocument<1024> doc) {
  // TODO: unsafe if city name is too long
  strncpy(wd->city, doc["city"], sizeof(wd->city));
  strncpy(wd->time, doc["time_str"], sizeof(wd->time));

  sprintf(
    wd->currentTemp, 
    "%d%c", 
    (int) doc["curr_temp"],
    DEGREE_SYMBOL
  );
  sprintf(
    wd->hiloTemp, "H:%d%cL:%d%c",
    (int) doc["high_temp"], DEGREE_SYMBOL,
    (int) doc["low_temp"], DEGREE_SYMBOL
  );

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

  Serial.println(wd->forecastStr.msg);
}