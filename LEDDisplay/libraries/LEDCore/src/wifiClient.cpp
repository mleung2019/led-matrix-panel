#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "networkManager.h"
#include "wifiClient.h"

String baseURL = String("http://") + SERVER_IP + ":" + SERVER_PORT;
StaticJsonDocument<1024> doc;

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
  long rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

int fetchWidget(Widget *w, void *data) {
  WidgetType type = w->type;
  
  HTTPClient http;
  http.setTimeout(5000);

  Serial.println("Fetching");
  switch (type) {
    case WEATHER: http.begin(baseURL + "/weather"); break;
    case SPOTIFY: http.begin(baseURL + "/spotify"); break;
    case SPORTS: http.begin(baseURL + "/sports"); break;
  }
  Serial.println("After http.begin()");

  int result = 0;
  Serial.println("Get started");
  int httpCode = http.GET();
  Serial.println("Get finished");
  if (httpCode <= 0 || networkCancel) {
      Serial.println("~~~ CANCELLED ~~~");
      http.end();
      return 1;
  }

  String payload = http.getString();
  if (networkCancel) { http.end(); return 1; Serial.println("~~~ CANCELLED ~~~"); }

  StaticJsonDocument<1024> doc;
  if (deserializeJson(doc, payload)) {
      http.end();
      // Parsing error
      return 2;
  }

  Serial.println("Parsing");
  bool parseError = false;
  // Parse into tempData
  switch (type) {
      case WEATHER: 
        parseWeather((WeatherData *) data, doc);
        parseError = parseWeatherIcon((WeatherData *) data);
        break;
      case SPOTIFY: 
        parseSpotify((SpotifyData *) data, doc); 
        parseError = parseSpotifyCover((SpotifyData *) data);
        break;
      case SPORTS:
        parseSports((SportsData *) data, doc);  
        parseError = parseSportsIcons((SportsData *) data);
        break;
  }
  Serial.println("Finished parsing");
  http.end();
  return parseError;
}

int parseWeatherIcon(WeatherData *wd) {
  return writeURLtoBitmap(
    (baseURL + "/weather/icon").c_str(), 
    wd->statusIcon,
    ICON_PIXELS * sizeof(uint16_t)
  );
}

int parseSpotifyCover(SpotifyData *sd) {
  return writeURLtoBitmap(
    (baseURL + "/spotify/cover").c_str(), 
    sd->cover,
    PANEL_PIXELS * sizeof(uint16_t)
  );
}

int parseSportsIcons(SportsData *pd) {
  uint16_t combinedBuffer[2 * ICON_PIXELS];
  int error = writeURLtoBitmap(
    (baseURL + "/sports/icons").c_str(), 
    combinedBuffer,
    2 * ICON_PIXELS * sizeof(uint16_t)
  );
  if (!error) {
    memcpy(
      pd->team1Icon, 
      combinedBuffer, 
      ICON_PIXELS * sizeof(uint16_t)
    );
    memcpy(
      pd->team2Icon, 
      combinedBuffer + ICON_PIXELS, 
      ICON_PIXELS * sizeof(uint16_t)
    );
  } 
  return error;
}

int writeURLtoBitmap(const char *url, uint16_t *frame, int size) {
  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode <= 0 || networkCancel) {
    Serial.println("~~~ CANCELLED BITMAP ~~~");
    http.end();
    return 1;
  }

  WiFiClient *stream = http.getStreamPtr();

  uint8_t *dst = reinterpret_cast<uint8_t *>(frame);
  int bytesRead = 0;

  while (bytesRead < size) {
    if (networkCancel) {
      http.end();
      return 1;
    }

    int avail = stream->available();
    if (avail > 0) {
        int toRead = min(avail, size - bytesRead);
        int n = stream->readBytes(dst + bytesRead, toRead);
        if (n > 0) bytesRead += n;
    } else {
      vTaskDelay(1);
    } 
    if ((bytesRead & 0x3FF) == 0) vTaskDelay(1); 
  }

  http.end();
  return 0;
}