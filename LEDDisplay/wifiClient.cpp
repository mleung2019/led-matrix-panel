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

void fetchWeather(WeatherData *weather) {
  HTTPClient http;

  http.begin("http://192.168.0.25:5000/weather");

  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode > 0) {
    String payload = http.getString();
  
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      weather->city = doc["city"];
      weather->time = doc["time_str"];
      weather->currentTemp = doc["curr_temp"];
      weather->highTemp = doc["high_temp"];
      weather->lowTemp= doc["low_temp"];


    } else {
    }
  } else {

  }
  http.end();
}
