#ifndef WIDGETS_H
#define WIDGETS_H

#include <Arduino.h>

#define WEATHER_ICON_LEN 16
#define WEATHER_ICON_SIZE WEATHER_ICON_LEN*WEATHER_ICON_LEN

struct WeatherData {
    // Location
    String city;
    String time;
    // Current temp
    int currentTemp;
    int highTemp;
    int lowTemp;
    // Status
    String statusDesc;
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