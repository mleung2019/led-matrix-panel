#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include <Arduino.h>

void connectWiFi(const char *ssid, const char *password);
void fetchGalleryInfo(int *mediaIndex, int *frameNum, int *sleep);
uint16_t *fetchGalleryMedia(int mediaIndex, int frame);

#endif
