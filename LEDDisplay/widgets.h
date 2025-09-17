#ifndef WIDGETS_H
#define WIDGETS_H

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#define SCROLLER_SIZE 128
#define PANEL_LENGTH 64
#define PANEL_PIXELS PANEL_LENGTH*PANEL_LENGTH
#define WEATHER_ICON_LEN 24
#define WEATHER_ICON_SIZE WEATHER_ICON_LEN*WEATHER_ICON_LEN

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
    uint16_t color;
    // Timing
    bool active = false;
    unsigned long lastUpdate = 0;
    unsigned long updateInterval = 30;
};

struct WeatherData {
    // Location
    char city[32];
    char time[16];
    // Temperature
    char currentTemp[8];
    char hiloTemp[16];
    // Status
    uint16_t statusIcon[WEATHER_ICON_SIZE];
    Scroller statusDesc;
    Scroller forecastStr;
};

struct SpotifyData {
    // String artistName;
    // String trackName;
    // String albumName;
    uint16_t cover[PANEL_PIXELS];
};

struct GalleryData {
    // String imageUrl;
    // String caption;
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
bool needWidgetUpdate(Widget *widget);
bool needScrollerUpdate(Scroller *scroller);

void widgetControl(MatrixPanel_I2S_DMA *display, Widget *widget, WidgetType type);
void drawWeather(MatrixPanel_I2S_DMA *display, Widget *widget);

void scrollerControl(MatrixPanel_I2S_DMA *display, Scroller *scroller);
void scrollerResize(MatrixPanel_I2S_DMA *display, Scroller *scroller);

/* ----------------------- ADAFRUIT GFX HELPER FUNCTIONS ----------------------- */
void drawCenteredText(
  MatrixPanel_I2S_DMA *display, 
  const char* msg, int y,
  int width=0, 
  uint16_t color=0xFFFF
);

#endif