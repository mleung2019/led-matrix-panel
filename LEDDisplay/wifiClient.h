#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include "widgets.h"

#define SERVER_IP "192.168.0.14"
#define SERVER_PORT 5001
#define GALLERY_SERVER_PORT 5002
#define DEGREE_SYMBOL 0xF8

void connectWiFi(const char *ssid, const char *password);
void writeURLtoBitmap(const char *url, uint16_t *frame, int imgLength);

int fetchWeather(WeatherData *weather);
void fetchWeatherIcon(WeatherData *weather);

int fetchSpotify(SpotifyData *spotify);
void fetchSpotifyCover(SpotifyData *spotify);

int fetchGallery(GalleryData *gallery, bool *isInit);
void consumeGallery(Streamer *streamer);

int fetchSports(SportsData *sports);
void fetchSportsIcons(SportsData *sports);

#endif
