#include "wifiClient.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define PANEL_PIXELS 64*64

uint16_t buffer[PANEL_PIXELS]; 

void connectWiFi(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected to WiFi");
}

String fetchGallery() {
  HTTPClient http;

  // Request image
  http.begin("http://192.168.0.23:5000/gallery"); 
  int httpCode = http.GET();
  if (httpCode != 200) {
    return "";
  }
  return http.getString();
}

uint16_t *convertFrame(String str) {
  // Format raw string and put it in array
  int count = 0;
  int start = 0;
  while (count < PANEL_PIXELS) {
    int comma = str.indexOf(',', start);
    String token;
    if (comma == -1) {
      token = str.substring(start);
    } else {
      token = str.substring(start, comma);
    }
    token.trim();
    if (token.length() > 0) {
      buffer[count++] = (uint16_t) strtol(token.c_str(), nullptr, 0);
    }
    if (comma == -1) break;
    start = comma + 1;
  }

  return buffer;
}
