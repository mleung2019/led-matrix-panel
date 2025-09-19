#ifndef WIDGETS_H
#define WIDGETS_H

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#define SCROLLER_SIZE 128

#define RING_SIZE 5

#define PANEL_LENGTH 64
#define PANEL_PIXELS PANEL_LENGTH*PANEL_LENGTH
#define BUFFER_SIZE PANEL_PIXELS*2 

#define WEATHER_ICON_LENGTH 24
#define WEATHER_ICON_PIXELS WEATHER_ICON_LENGTH*WEATHER_ICON_LENGTH

/* ----------------------- ENUMS/STRUCTS ----------------------- */
enum WidgetType {
  WEATHER,
  SPOTIFY,
  GALLERY
};

struct Scroller {
  char msg[SCROLLER_SIZE] = {};
  // Position
  int16_t x;
  int y;
  uint16_t color = 0xFFFF;
  // Timing
  bool active = false;
  unsigned long lastUpdate = 0;
  unsigned long updateInterval = 30;
};

struct FrameSlot {
  uint16_t frame[PANEL_PIXELS];
  uint16_t updateInterval;
  bool valid = false;
};

struct Streamer {
  // Producer and consumer buffer
  FrameSlot ringBuffer[RING_SIZE];
  int in = 0; // Write index
  int out = 0; // Read index
  int framesReady = 0;
  uint16_t frame[PANEL_PIXELS];
  // Control
  unsigned long lastUpdate = 0;
  unsigned long updateInterval = 0;
};

struct WeatherData {
  // Location
  char city[32];
  char time[16];
  // Temperature
  char currentTemp[8];
  char hiloTemp[16];
  // Status
  uint16_t statusIcon[WEATHER_ICON_PIXELS];
  Scroller statusDesc;
  Scroller forecastStr;
};

struct SpotifyData {
  uint16_t cover[PANEL_PIXELS];
  Scroller trackInfo;
};

struct GalleryData {
  Streamer streamer;
};

struct Widget {
  WidgetType type;
  // Cache and timing variables
  bool isInit = false;
  unsigned long lastUpdate = 0;
  unsigned long updateInterval = 5000;
  // Data
  WeatherData weather;
  SpotifyData spotify;
  GalleryData gallery;
};

/* ----------------------- FETCHING FROM SERVER ----------------------- */
void fetchTask(void *parameter);

/* ----------------------- WIDGET/SCROLLER CONTROL ----------------------- */
bool needScrollerUpdate(Scroller *scroller);
bool needStreamerUpdate(Streamer *streamer);

void widgetControl(MatrixPanel_I2S_DMA *display, Widget *widget, WidgetType type);
void drawWeather(MatrixPanel_I2S_DMA *display, Widget *widget);
void drawSpotify(MatrixPanel_I2S_DMA *display, Widget *widget);
void drawGallery(MatrixPanel_I2S_DMA *display, Widget *widget);

void scrollerControl(MatrixPanel_I2S_DMA *display, Scroller *scroller);
void scrollerResize(MatrixPanel_I2S_DMA *display, Scroller *scroller);

/* ----------------------- ADAFRUIT GFX HELPER FUNCTIONS ----------------------- */
void drawCenteredText(
  MatrixPanel_I2S_DMA *display, 
  const char* msg, int y,
  int width=0
);

#endif