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
  Serial.printf("Weather: %d\n", httpCode);
  if (httpCode > 0) {
    String payload = http.getString();

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      // TODO: unsafe if city name is too long
      strncpy(weather->city, doc["city"], sizeof(weather->city));
      strncpy(weather->time, doc["time_str"], sizeof(weather->time));

      sprintf(
        weather->currentTemp, 
        "%d%c", 
        (int) doc["curr_temp"],
        DEGREE_SYMBOL
      );
      sprintf(
        weather->hiloTemp, "H:%d%cL:%d%c",
        (int) doc["high_temp"], DEGREE_SYMBOL,
        (int) doc["low_temp"], DEGREE_SYMBOL
      );

      strncpy(weather->statusDesc.msg, doc["status"], sizeof(weather->statusDesc.msg));

      memset(weather->forecastStr.msg, 0, SCROLLER_SIZE);
      for (int i = 0; i < 5; i++) {
        const char *timeStr = doc["five_hr_log"][i]["time_str"].as<const char *>();
        int temp = doc["five_hr_log"][i]["temp"];

        size_t used = strlen(weather->forecastStr.msg);
        if (used < SCROLLER_SIZE - 1) {
          snprintf(weather->forecastStr.msg + used, SCROLLER_SIZE - used,
          "%s:%d%c ", timeStr, temp, DEGREE_SYMBOL);
        }
      }

      bool needsIcon = doc["needs_icon"];
      if (needsIcon) {
        fetchWeatherIcon(weather);
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

void fetchWeatherIcon(WeatherData *weather) {
  writeURLtoBitmap(
    "http://192.168.0.14:5001/weather/icon", 
    weather->statusIcon,
    WEATHER_ICON_LENGTH
  );
}

int fetchSpotify(SpotifyData *spotify) {
  HTTPClient http;
  http.begin("http://192.168.0.14:5001/spotify");

  int result = 0;
  int httpCode = http.GET();
  Serial.printf("Spotify: %d\n", httpCode);
  if (httpCode > 0) {
    String payload = http.getString();

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      memset(spotify->trackInfo.msg, 0, SCROLLER_SIZE);

      const char *trackStr = doc["track_name"].as<const char *>();
      const char *artistStr = doc["artist_name"].as<const char *>();
      sprintf(
        spotify->trackInfo.msg, 
        "%s - %s", 
        artistStr,
        trackStr
      );

      bool needsCover = doc["needs_cover"];
      if (needsCover) {
        fetchSpotifyCover(spotify);
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

void fetchSpotifyCover(SpotifyData *spotify) {
  writeURLtoBitmap(
    "http://192.168.0.14:5001/spotify/cover", 
    spotify->cover,
    PANEL_LENGTH
  );
}

// Writes to ringBuffer (producer)
int fetchGallery(GalleryData *gallery, bool *isInit) {
  WiFiClient client;

  if (!client.connect(SERVER_IP, GALLERY_SERVER_PORT)) {
    Serial.println("Connection failed");
    return 1;
  }

  Serial.println("Connected to gallery server");
  while (client.connected() && *isInit) {
    Streamer *streamer = &gallery->streamer;

    // Wait until we have space in the ring buffer
    if (streamer->framesReady >= RING_SIZE) {
      delay(1);
      continue;
    }

    // Temp storage to read 2 bytes at a time
    uint8_t tempBytes[2];

    // Read header
    while (client.available() < 2) delay(1);
    client.read(tempBytes, 2);
    uint16_t updateInterval = (tempBytes[0] << 8) | tempBytes[1];

    // Read frame in a buffer
    FrameSlot *ringBuffer = streamer->ringBuffer;
    uint16_t *frame = ringBuffer[streamer->in].frame;
    for (int i = 0; i < PANEL_PIXELS; ++i) {
      while (client.available() < 2) delay(1);
      client.read(tempBytes, 2);
      frame[i] = tempBytes[0] | (tempBytes[1] << 8);
    }
    ringBuffer[streamer->in].updateInterval = updateInterval;
    ringBuffer[streamer->in].valid = true;

    streamer->in = (streamer->in + 1) % RING_SIZE;
    streamer->framesReady++;
  }

  // Client disconnected
  client.stop();
  Serial.println("Disconnected from server");
  return 0;
}

// Reads from ringBuffer (consumer)
void consumeGallery(Streamer *streamer) {
  if (streamer->framesReady == 0) return; // No frame ready

  // Copy frame to display
  FrameSlot *ringBuffer = streamer->ringBuffer;
  uint16_t *frame = ringBuffer[streamer->out].frame;
  for (int i = 0; i < PANEL_PIXELS; ++i) {
    streamer->frame[i] = frame[i];
  }

  streamer->updateInterval = ringBuffer[streamer->out].updateInterval;
  ringBuffer[streamer->out].valid = false;

  streamer->out = (streamer->out + 1) % RING_SIZE;
  streamer->framesReady--;
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
