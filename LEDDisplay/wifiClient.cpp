#include <WiFi.h>
#include "wifiClient.h"
#include <HTTPClient.h>

#define PANEL_PIXELS 64*64
#define FRAME_SIZE PANEL_PIXELS*2

uint16_t mediaFrame[PANEL_PIXELS]; 

void connectWiFi(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected to WiFi");
}

void fetchGalleryInfo(int *mediaIndex, int *frameNum, int *sleep) {
  HTTPClient http;

  // Request image
  http.begin("http://192.168.0.23:5000/gallery"); 
  int httpCode = http.GET();
  if (httpCode != 200)
    return;

  char *str = strdup(http.getString().c_str());
  char *token = strtok(str, ",");
  int count = 0;

  while (count < 3) {
    if (count == 0) {
      *mediaIndex = atoi(token);
    }
    else if (count == 1) {
      *frameNum = atoi(token);
    } else {
      *sleep = atoi(token);
    }
    count++;
    token = strtok(NULL, ","); 
  }

  free(str);
  http.end();
}

uint16_t *fetchGalleryMedia(int mediaIndex, int frame) {
  HTTPClient http;
  
  char serverURL[100];
  sprintf(serverURL, "http://192.168.0.23:5000/gallery/%d/%d", mediaIndex, frame);

  http.begin(serverURL);
  int httpCode = http.GET();
  
  WiFiClient *stream = http.getStreamPtr();
  uint8_t buffer[2];

  for (int i = 0; i < PANEL_PIXELS; i++) {
    if (stream->readBytes(buffer, 2) == 2) {
      mediaFrame[i] = buffer[0] | (buffer[1] << 8);
    }
  }

  http.end();
  return mediaFrame;
}
