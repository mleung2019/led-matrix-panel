#ifndef WIDGETS_H
#define WIDGETS_H

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#define PANEL_PIXELS 64*64
#define WEATHER_ICON_LEN 24
#define WEATHER_ICON_SIZE WEATHER_ICON_LEN*WEATHER_ICON_LEN
#define DEGREE_SYMBOL 0xF8

/* ----------------------- ENUMS/STRUCTS ----------------------- */
enum WidgetType {
    WEATHER,
    SPOTIFY,
    GALLERY
};

struct Scroller {
    char msg[128] = {};
    // Position
    int16_t x;
    int y;
    uint16_t color;
    // Timing
    bool active = false;
    unsigned long lastMove = 0;
    int updateInterval = 30;
};

struct WeatherData {
    // Location
    char city[32];
    char time[16];
    // Current temp
    int currentTemp;
    int highTemp;
    int lowTemp;
    // Status
    char statusDesc[32];
    uint16_t statusIcon[WEATHER_ICON_SIZE];
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

    union {
        WeatherData weather;
        SpotifyData spotify;
        GalleryData gallery;
    } data;
};

/* ----------------------- WIDGET CONTROL ----------------------- */
bool needsUpdate(Widget *widget);
void widgetControl(MatrixPanel_I2S_DMA *display, Widget *widget, WidgetType type);
void drawWeather(MatrixPanel_I2S_DMA *display, Widget *widget);

/* ----------------------- ADAFRUIT GFX HELPER FUNCTIONS ----------------------- */
bool isTooLong(MatrixPanel_I2S_DMA *display, const char *msg);
void drawCenteredText(
  MatrixPanel_I2S_DMA *display, 
  const char* msg, int y,
  int width=0, 
  uint16_t color=0xFFFF
);

#endif