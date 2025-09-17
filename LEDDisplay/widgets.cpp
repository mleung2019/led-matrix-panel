#include "wifiClient.h"

#include "widgets.h"

// Determine whether widget should ping the server
bool needsUpdate(Widget *widget) {
  unsigned long now = millis();
  if (now - widget->lastUpdate >= widget->updateInterval) {
    widget->lastUpdate = now;
    return true;
  }
  return false;
}

void widgetControl(MatrixPanel_I2S_DMA *display, Widget *widget, WidgetType type) {
  // Handle widget change
  if (widget->type != type) {
    widget->isInit = false;
  }

  // Fetch new widget info
  if (needsUpdate(widget)) {
    // Wipe everything before update
    display->clearScreen();
    display->setCursor(0, 0);
    switch (widget->type) {
      case WEATHER:
        int result = fetchWeather(&widget->data.weather);
        // Force fetch weather icon on startup
        if (!widget->isInit) {
          fetchWeatherIcon(&widget->data.weather);
          widget->isInit = true;
        }
        if (!result) {
          drawWeather(display, widget);
        }
        break;
    }
    display->flipDMABuffer();
  }

  // Scroll text (if needed for the widget)
  switch (widget->type) {
    case WEATHER:
      if 

      break;
  }
}

void drawWeather(MatrixPanel_I2S_DMA *display, Widget *widget) {
  WeatherData weather = widget->data.weather;
  
  // CITY
  drawCenteredText(display, weather.city, 0);
  display->println();

  // STATUS DESCRIPTION
  if (!isTooLong(display, weather.statusDesc)) {
    drawCenteredText(display, weather.statusDesc, 8);
  } else {
    
  }

  // CURRENT TEMPERATURE
  char tempBuffer[16] = {0};
  sprintf(tempBuffer, "%d%c", weather.currentTemp, 0xF8);
  // if (strlen(tempBuffer) != 4) {
  //   display->setTextSize(2);
  //   drawCenteredText(display, tempBuffer, 21, 42);
  //   display->setTextSize(1);
  // } else {
  drawCenteredText(display, weather.time, 25-5, 42);
  drawCenteredText(display, tempBuffer, 25+4, 42);
  // }

  // HIGH AND LOW TEMP
  sprintf(
    tempBuffer, "H:%d%cL:%d%c",
    weather.highTemp, 0xF8,
    weather.lowTemp, 0xF8
  );
  drawCenteredText(display, tempBuffer, 41);

  // STATUS ICON
  display->drawRGBBitmap(
    38, 16, 
    weather.statusIcon, 
    WEATHER_ICON_LEN, WEATHER_ICON_LEN
  ); 
}

bool isTooLong(MatrixPanel_I2S_DMA *display, const char *msg) {
  int16_t x1, y1;
  uint16_t w, h;
  display->getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);
  
  return w > display->width();
}

void drawCenteredText(
  MatrixPanel_I2S_DMA *display, 
  const char* msg, int y, 
  int width,
  uint16_t color
) {
  int16_t x1, y1;
  uint16_t w, h;
  display->getTextBounds(msg, 0, y, &x1, &y1, &w, &h);

  // Compute centered X
  if (!width) {
    width = display->width();
  }
  int x = (width - w) / 2;

  // Draw the text
  display->setCursor(x, y);
  display->setTextColor(color);
  display->print(msg);
}