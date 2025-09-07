#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#include <Arduino.h>

void connectWiFi(const char *ssid, const char *password);
String fetchGallery();
uint16_t *convertFrame(String str);

#endif
