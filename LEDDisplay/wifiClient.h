#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include <Arduino.h>

extern uint16_t mediaFrame[4096]; 

void connectWiFi(const char *ssid, const char *password);
void fetchGalleryInfo(int *mediaIndex, int *frameNum, int *sleep);
void fetchGalleryMedia(int mediaIndex);
void fetchStream();
void endStream();
#endif
