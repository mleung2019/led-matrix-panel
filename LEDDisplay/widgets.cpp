#include "wifiClient.h"

#include "widgets.h"

// Determine whether widget should ping the server
bool needWidgetUpdate(Widget *widget) {
  unsigned long now = millis();
  if (now - widget->lastUpdate >= widget->updateInterval) {
    widget->lastUpdate = now;
    return true;
  }
  return false;
}

// Determine whether scroller should scroll
bool needScrollerUpdate(Scroller *scroller) {
  unsigned long now = millis();
  if (scroller->active && now - scroller->lastUpdate >= scroller->updateInterval) {
    scroller->lastUpdate = now;
    scroller->x--;
    return true;
  }
  return false;
}

void widgetControl(MatrixPanel_I2S_DMA *display, Widget *widget, WidgetType type) {
  // Handle widget change
  if (widget->type != type) {
    widget->isInit = false;
    widget->type = type;
  }

  // Update frame
  switch (widget->type) {
    case WEATHER:
      // Fetch new widget info
      if (needWidgetUpdate(widget)) {
        int result = fetchWeather(&widget->weather);
        // Force fetch weather icon on startup
        if (!widget->isInit) {
          fetchWeatherIcon(&widget->weather);
          widget->isInit = true;
        }
      }
      // Fetch new scroller info
      needScrollerUpdate(&widget->weather.statusDesc);
      break;
  }

  // Draw frame
  display->clearScreen();
  display->setCursor(0, 0);
  switch (widget->type) {
    case WEATHER:
      drawWeather(display, widget);
      scrollerControl(display, &widget->weather.statusDesc);
    break;
  }

  display->flipDMABuffer();
}

void drawWeather(MatrixPanel_I2S_DMA *display, Widget *widget) {
  WeatherData *weather = &widget->weather;
  
  // CITY
  drawCenteredText(display, weather->city, 0);
  display->println();

  // STATUS DESCRIPTION
  scrollerResize(display, &weather->statusDesc);
  weather->statusDesc.y = 8;
  
  // CURRENT TEMPERATURE
  char tempBuffer[16] = {0};
  sprintf(tempBuffer, "%d%c", weather->currentTemp, 0xF8);
  // if (strlen(tempBuffer) != 4) {
  //   display->setTextSize(2);
  //   drawCenteredText(display, tempBuffer, 21, 42);
  //   display->setTextSize(1);
  // } else {
  drawCenteredText(display, weather->time, 25-5, 42);
  drawCenteredText(display, tempBuffer, 25+4, 42);
  // }

  // HIGH AND LOW TEMP
  sprintf(
    tempBuffer, "H:%d%cL:%d%c",
    weather->highTemp, 0xF8,
    weather->lowTemp, 0xF8
  );
  drawCenteredText(display, tempBuffer, 41);

  // STATUS ICON
  display->drawRGBBitmap(
    38, 16, 
    weather->statusIcon, 
    WEATHER_ICON_LEN, WEATHER_ICON_LEN
  ); 
}

void scrollerControl(MatrixPanel_I2S_DMA *display, Scroller *scroller) {
  if (scroller->active) {
    int16_t x1, y1;
    uint16_t w, h;
    display->getTextBounds(scroller->msg, 0, 0, &x1, &y1, &w, &h);

    if (scroller->x < -w) {
      scroller->x = display->width();
    }

    display->setCursor(scroller->x, scroller->y);
    display->print(scroller->msg);
  } else {
    drawCenteredText(display, scroller->msg, scroller->y);
  }
}

// When scroller info changes, we need to determine if scrolling should
// be active
void scrollerResize(MatrixPanel_I2S_DMA *display, Scroller *scroller) {
  int16_t x1, y1;
  uint16_t w, h;
  display->getTextBounds(scroller->msg, 0, 0, &x1, &y1, &w, &h);
  
  if (w > display->width()) {
    scroller->active = true;
  } else {
    scroller->active = false;
  }
}

void drawCenteredText(
  MatrixPanel_I2S_DMA *display, 
  const char* msg, int y, 
  int width,
  uint16_t color
) {
  int16_t x1, y1;
  uint16_t w, h;
  display->getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);

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