#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "wifiClient.h"

void connectWiFi(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    if (millis() - start > 15000) {
      // avoid hanging indefinitely; try again
      Serial.println("WiFi connect timeout, retrying...");
      start = millis();
      WiFi.begin(ssid, password);
    }
  }
  Serial.println("Connected to WiFi");
}



int fetchWeather(WeatherData *weather) {
  HTTPClient http;
  http.begin("http://192.168.0.14:5001/weather");

  int result = 0;
  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode > 0) {
    String payload = http.getString();

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      strncpy(weather->city, doc["city"], sizeof(weather->city));
      strncpy(weather->time, doc["time_str"], sizeof(weather->time));

      weather->currentTemp = doc["curr_temp"];
      weather->highTemp = doc["high_temp"];
      weather->lowTemp = doc["low_temp"];

      strncpy(weather->statusDesc, doc["status"], sizeof(weather->statusDesc));

      bool needsIcon = doc["needs_icon"];
      if (needsIcon || !weather->hasCache) {
        weather->hasCache = true;
        writeURLtoBitmap(
          "http://192.168.0.14:5001/weather/icon", 
          weather->statusIcon, 
          WEATHER_ICON_LEN
        );
      }
    }
    // Parsing problem
    else {
      result = 1;
    }
  }
  // Server problem
  else {
    result = 2;
  }
  // Success
  http.end();
  return result;
}

void writeURLtoBitmap(const char *url, uint16_t *frame, int imgLength) {
  HTTPClient http;
  http.begin(url);
  
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    WiFiClient *stream = http.getStreamPtr();

    int size = imgLength * imgLength;

    // Get full image
    uint8_t buffer[2];
    for (int i = 0; i < size; i++) {
      if (stream->readBytes(buffer, 2) == 2) {
        frame[i] = buffer[0] | (buffer[1] << 8);
      }
    }
  }

  http.end();
  return;
}
