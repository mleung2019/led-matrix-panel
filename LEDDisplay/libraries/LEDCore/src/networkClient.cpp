#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

#include "networkManager.h"
#include "networkClient.h"

String baseURL = String("http://") + SERVER_IP;

void connectWiFi() {  
  WiFiManager wm;
  wm.setConfigPortalTimeout(180);

  bool res = wm.autoConnect("ESP32-LED-DISPLAY");
  if(!res) {
    Serial.println("Failed to connect");
    ESP.restart();
  }

  Serial.println("Connected to WiFi");
  long rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void beginWithKey(HTTPClient &http, const String &url) {
  http.begin(url);
  // http.addHeader("X-Device-Key", X_DEVICE_KEY);
}

int initLocation() {
  HTTPClient http;
  http.setTimeout(5000);
  
  http.begin("http://ipinfo.io/json");
  int httpCode = http.GET();
  if (httpCode <= 0) { http.end(); return 1; }

  String locationBody = http.getString();

  http.begin(baseURL + "/location");
  http.addHeader("Content-Type", "application/json");
  // http.addHeader("X-Device-Key", X_DEVICE_KEY);
  httpCode = http.POST(locationBody);

  http.end();
  return 0;
}

int fetchWidget(Widget *w, void *data) {
  HTTPClient http;
  http.setTimeout(5000);
  int httpCode = 0;

  WidgetType type = w->type;
  switch (type) {
    case WEATHER: 
      beginWithKey(http, baseURL + "/weather"); 
      httpCode = http.GET();
      break;
    case SPOTIFY: 
      beginWithKey(http, baseURL + "/spotify"); 
      httpCode = http.GET();
      break;
    case SPORTS: 
      beginWithKey(http, baseURL + "/sports"); 
      httpCode = http.GET();
      break;
  }

  if (httpCode != 200) {
    Serial.printf(
      "(%d) [HTTP] result code: %d (%s)\n",
      (int) type, 
      httpCode,
      http.errorToString(httpCode).c_str()
    );
  }

  if (httpCode <= 0 || networkCancel) { http.end(); return 1; }

  String payload = http.getString();
  StaticJsonDocument<1024> doc;
  if (deserializeJson(doc, payload)) {
    Serial.println("Failed JSON parse:");
    Serial.println(payload);
    http.end();
    // Parsing error
    return 2;
  }

  bool imgError = false;
  // Parse into tempData
  switch (type) {
      case WEATHER: 
        parseWeather((WeatherData *) data, doc);
        imgError = parseWeatherIcon((WeatherData *) data);
        break;
      case SPOTIFY: 
        parseSpotify((SpotifyData *) data, doc); 
        imgError = parseSpotifyCover((SpotifyData *) data);
        break;
      case SPORTS:
        parseSports((SportsData *) data, doc);  
        imgError = parseSportsIcons((SportsData *) data);
        break;
  }
  http.end();
  return imgError;
}

int writeURLtoBitmap(const char *url, uint16_t *frame, int size) {
  HTTPClient http;
  beginWithKey(http, url);

  int httpCode = http.GET();

  if (httpCode <= 0 || networkCancel) { http.end(); return 1; }

  WiFiClient *stream = http.getStreamPtr();

  uint8_t *dst = reinterpret_cast<uint8_t *>(frame);
  int bytesRead = 0;

  while (bytesRead < size) {
    if (networkCancel) { http.end(); return 1; }

    int avail = stream->available();
    if (avail > 0) {
        int toRead = min(avail, size - bytesRead);
        int n = stream->readBytes(dst + bytesRead, toRead);
        if (n > 0) bytesRead += n;
    } 
    else vTaskDelay(1);
    
    if ((bytesRead & 0x3FF) == 0) vTaskDelay(1); 
  }

  http.end();
  return 0;
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
  int imgError = writeURLtoBitmap(
    (baseURL + "/sports/icons").c_str(), 
    combinedBuffer,
    2 * ICON_PIXELS * sizeof(uint16_t)
  );

  if (!imgError) {
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
  return imgError;
}