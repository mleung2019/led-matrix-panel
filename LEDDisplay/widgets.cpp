#include "wifiClient.h"
#include "widgets.h"

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

void fetchTask(void *parameter) {
  Widget *widget = (Widget *)parameter;
  for (;;) {
    switch (widget->type) {
      case WEATHER:
        fetchWeather(&widget->weather);
        // Force fetch weather icon on startup
        if (!widget->isInit) {
          fetchWeatherIcon(&widget->weather);
          widget->isInit = true;
        }
        break;
      case SPOTIFY:
        fetchSpotify(&widget->spotify);
        // Force fetch album cover on startup
        if (!widget->isInit) {
          fetchSpotifyCover(&widget->spotify);
          widget->isInit = true;
        }
        break;
      case GALLERY:
        if (xSemaphoreTake(widget->gallery.streamer.filledSem, pdMS_TO_TICKS(500)) == pdTRUE) {
          consumeGallery(widget);
        } else if (!widget->gallery.streamer.isStreaming) {
          Serial.println("Timeout from consumer");
          break;
        }
        xSemaphoreGive(widget->gallery.streamer.emptySem);
        break;
      case SPORTS:
        fetchSports(&widget->sports);
        fetchSportsIcons(&widget->sports);
        if (!widget->isInit) {
          widget->isInit = true;
        }
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(widget->updateInterval));
  }
}

void galleryProducerTask(void *parameter) {
  Widget *widget = (Widget *)parameter;
  for (;;) {
    if (widget->gallery.streamer.isStreaming) {
      fetchGallery(&widget->gallery);
      if (!widget->isInit) {
        widget->isInit = true;
      }
    }
  }
}

void widgetControl(MatrixPanel_I2S_DMA *display, Widget *widget, WidgetType type) {
  // Handle widget change
  if (widget->type != type) {
    if (widget->type == GALLERY) {
      widget->gallery.streamer.isStreaming = false;
    }

    widget->isInit = false;
    widget->type = type;

    switch (widget->type) {
      case WEATHER: 
        widget->updateInterval = 5000; 
        break;
      case SPOTIFY: 
        widget->updateInterval = 2500; 
        break;
      case GALLERY: 
        widget->updateInterval = 0; 
        widget->gallery.streamer.isStreaming = true;
        break;
      case SPORTS: 
        widget->updateInterval = 5000;
        break;
    }
  }

  // Update per frame
  switch (widget->type) {
    case WEATHER:
      needScrollerUpdate(&widget->weather.statusDesc);
      needScrollerUpdate(&widget->weather.forecastStr);
      break;
    case SPOTIFY:
      needScrollerUpdate(&widget->spotify.trackInfo);
      break;
    case GALLERY: break;
    case SPORTS:
      needScrollerUpdate(&widget->sports.shortDetail);
      break;
  }

  display->clearScreen();
  display->setCursor(0, 0);

  // Draw frame
  switch (widget->type) {
    case WEATHER:
      drawWeather(display, widget);
      scrollerControl(display, &widget->weather.statusDesc);
      scrollerControl(display, &widget->weather.forecastStr);
      break;
    case SPOTIFY:
      drawSpotify(display, widget);
      display->fillRect(0, 55, 64, 9, 0x0000);
      scrollerControl(display, &widget->spotify.trackInfo);
      break;
    case GALLERY:
      drawGallery(display, widget);
      break;
    case SPORTS:
      drawSports(display, widget);
      scrollerControl(display, &widget->sports.shortDetail);
      break;
  }

  display->flipDMABuffer();
}

void drawWeather(MatrixPanel_I2S_DMA *display, Widget *widget) {
  WeatherData *weather = &widget->weather;
  
  // CITY
  drawCenteredText(display, weather->city, 1);
  display->println();

  // STATUS DESCRIPTION
  scrollerResize(display, &weather->statusDesc);
  weather->statusDesc.y = 11;
  
  // TIME AND CURRENT TEMPERATURE
  drawCenteredText(display, weather->time, 23, 38);
  drawCenteredText(display, weather->currentTemp, 34, 38);

  // HIGH AND LOW TEMP
  drawCenteredText(display, weather->hiloTemp, 46);

  // STATUS ICON
  display->drawRGBBitmap(
    36, 20, 
    weather->statusIcon, 
    ICON_LENGTH, ICON_LENGTH
  ); 

  // 5 HOUR FORECAST
  scrollerResize(display, &weather->forecastStr);
  weather->forecastStr.y = 56;
}

void drawSpotify(MatrixPanel_I2S_DMA *display, Widget *widget) {
  SpotifyData *spotify = &widget->spotify;

  // COVER
  display->drawRGBBitmap(
    0, 0, 
    spotify->cover, 
    PANEL_LENGTH, PANEL_LENGTH
  ); 

  // TRACK INFO
  scrollerResize(display, &spotify->trackInfo);
  spotify->trackInfo.y = 56;
}

void drawGallery(MatrixPanel_I2S_DMA *display, Widget *widget) {
  GalleryData *gallery = &widget->gallery;
  display->drawRGBBitmap(
    0, 0, 
    gallery->streamer.frame,
    PANEL_LENGTH, PANEL_LENGTH
  );  
}

void drawSports(MatrixPanel_I2S_DMA *display, Widget *widget) {
  SportsData *sports = &widget->sports;

  // SPORT NAME
  drawCenteredText(display, sports->sportName, 2);

  // TEAM NAMES
  drawCenteredText(display, sports->team1Name, 11, 32);
  drawCenteredText(display, sports->team2Name, 11, 32, 32);
  
  // TEAM ICONS
  display->drawRGBBitmap(
    0+4, 20, 
    sports->team1Icon, 
    ICON_LENGTH, ICON_LENGTH
  ); 
  display->drawRGBBitmap(
    64-ICON_LENGTH-4, 20, 
    sports->team2Icon, 
    ICON_LENGTH, ICON_LENGTH
  ); 

  // TEAM SCORES
  drawCenteredText(display, sports->team1Score, 46, 32);
  drawCenteredText(display, sports->team2Score, 46, 32, 32);

  // SHORT DETAIL
  scrollerResize(display, &sports->shortDetail);
  sports->shortDetail.y = 55;
}

// If active, scroll msg. If inactive, center msg
void scrollerControl(MatrixPanel_I2S_DMA *display, Scroller *scroller) {
  display->setTextColor(scroller->color);
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
  display->setTextColor(0xFFFF);
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
  int width, int offset
) {
  int16_t x1, y1;
  uint16_t w, h;
  display->getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);

  // Compute centered X
  if (!width) {
    width = display->width();
  }
  int x = ((width - w) / 2) + offset;

  // Draw the text
  display->setCursor(x, y);
  display->print(msg);
}