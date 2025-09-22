#ifndef WIDGETS_H
#define WIDGETS_H

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#define SCROLLER_SIZE 128

#define RING_SIZE 5

#define PANEL_LENGTH 64
#define PANEL_PIXELS PANEL_LENGTH*PANEL_LENGTH
#define BUFFER_SIZE PANEL_PIXELS*2 

#define ICON_LENGTH 24
#define ICON_PIXELS ICON_LENGTH*ICON_LENGTH

/* ----------------------- ENUMS/STRUCTS ----------------------- */
enum WidgetType {
  WEATHER,
  SPOTIFY,
  GALLERY,
  SPORTS
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
  SemaphoreHandle_t filledSem = xSemaphoreCreateCounting(RING_SIZE, 0);
  SemaphoreHandle_t emptySem = xSemaphoreCreateCounting(RING_SIZE, RING_SIZE);
  uint16_t frame[PANEL_PIXELS];
  TaskHandle_t streamTaskHandle = NULL;
  volatile bool isStreaming = false;
};

struct WeatherData {
  // Location
  char city[32];
  char time[16];
  // Temperature
  char currentTemp[8];
  char hiloTemp[16];
  // Status
  uint16_t statusIcon[ICON_PIXELS];
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

struct SportsData {
  char sportName[16] = "";
  char team1Name[8] = "";
  char team1Score[8] = "";
  uint16_t team1Icon[ICON_PIXELS];
  char team2Name[8] = "";
  char team2Score[8] = "";
  uint16_t team2Icon[ICON_PIXELS];
  Scroller shortDetail;
};

struct Widget {
  volatile WidgetType type;
  // Cache and timing variables
  bool isInit = false;
  unsigned long lastUpdate = 0;
  unsigned long updateInterval = 5000;
  // Data
  WeatherData weather;
  SpotifyData spotify;
  GalleryData gallery;
  SportsData sports;
};

/* ----------------------- FREERTOS TASKS ----------------------- */
void fetchTask(void *parameter);
void galleryProducerTask(void *parameter);

/* ----------------------- WIDGET/SCROLLER CONTROL ----------------------- */
bool needScrollerUpdate(Scroller *scroller);

void widgetControl(MatrixPanel_I2S_DMA *display, Widget *widget, WidgetType type);
void drawWeather(MatrixPanel_I2S_DMA *display, Widget *widget);
void drawSpotify(MatrixPanel_I2S_DMA *display, Widget *widget);
void drawGallery(MatrixPanel_I2S_DMA *display, Widget *widget);
void drawSports(MatrixPanel_I2S_DMA *display, Widget *widget);

void scrollerControl(MatrixPanel_I2S_DMA *display, Scroller *scroller);
void scrollerResize(MatrixPanel_I2S_DMA *display, Scroller *scroller);

/* ----------------------- ADAFRUIT GFX HELPER FUNCTIONS ----------------------- */
void drawCenteredText(
  MatrixPanel_I2S_DMA *display, 
  const char* msg, int y,
  int width=0, int offset=0
);

#endif