#ifndef WIDGETS_H
#define WIDGETS_H

#include <Arduino.h>

#define WEATHER_ICON_LEN 16
#define WEATHER_ICON_SIZE WEATHER_ICON_LEN*WEATHER_ICON_LEN

struct WeatherData {
    // Cache
    bool hasCache;
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
    String trackName;
    String artistName;
    String albumName;
    String coverUrl;
    bool isPlaying;
};

struct GalleryData {
    String imageUrl;
    String caption;
};

#endif