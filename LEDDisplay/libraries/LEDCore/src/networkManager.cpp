#include "wifiClient.h"
#include "networkManager.h"

volatile bool networkCancel = false;

void networkTask(void *parameters) {
  Widget *w = (Widget *) parameters;
  WidgetType currentType = (WidgetType) -1;
  WidgetType newType;
  Serial.println("NETWORKTASK INIT");
  for (;;) {
    // On widget transition, cancel existing network fetches
    while (xQueueReceive(widgetSwitchQueue, &newType, 0) == pdTRUE) {
      currentType = newType;
      networkCancel = true;
      w->lastUpdate = 0;    
      Serial.println("Widget switch: networkCancel is true");
    }

    // As a precaution, skip if no widget has been initialized yet
    if (currentType == (WidgetType) -1 || !w->isInit) {
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    } 

    unsigned long now = millis();
    if (now - w->lastUpdate >= w->updateInterval) {
      w->lastUpdate = now;
      networkCancel = false;
      Serial.println("networkCancel is false");
      
      void *tempData = nullptr;
      switch (currentType) {
        case WEATHER: tempData = new WeatherData{}; break;
        case SPOTIFY: tempData = new SpotifyData{}; break;
        case SPORTS:  tempData = new SportsData{};  break;
        default: break;
      }
      if (!tempData) continue;

      if (!fetchWidget(w, tempData)) {
        // if (xSemaphoreTake(widgetMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
          if (w->isInit && currentType == w->type) {
            // Write temporary data to widget
            switch (currentType) {
              case WEATHER: {
                WeatherData *wd = (WeatherData *) tempData;

                Scroller tempStatusDesc = w->weather->statusDesc;
                Scroller tempForecastStr = w->weather->forecastStr;

                strcpy(tempStatusDesc.msg, wd->statusDesc.msg);
                strcpy(tempForecastStr.msg, wd->forecastStr.msg);
                wd->statusDesc = tempStatusDesc;
                wd->forecastStr = tempForecastStr;

                memcpy(w->weather, wd, sizeof(WeatherData));
                break;
              }
              case SPOTIFY: {
                SpotifyData *sd = (SpotifyData *) tempData;

                Scroller tempTrackInfo = w->spotify->trackInfo;

                strcpy(tempTrackInfo.msg, sd->trackInfo.msg);
                sd->trackInfo = tempTrackInfo;

                memcpy(w->spotify, sd, sizeof(SpotifyData));
                break;
              }
              case SPORTS: {
                SportsData *pd = (SportsData *) tempData;

                Scroller tempShortDetail = w->sports->shortDetail;

                strcpy(tempShortDetail.msg, pd->shortDetail.msg);
                pd->shortDetail = tempShortDetail;

                memcpy(w->sports, pd, sizeof(SportsData));
                break;
              }
            }
            Serial.println("~~~ LOADED ~~~");
            w->isLoaded = true;
          } else {
            Serial.println("Discard temp data");
          }
          // xSemaphoreGive(widgetMutex);
        // }
      }

      switch (currentType) {
        case WEATHER: delete (WeatherData*) tempData; break;
        case SPOTIFY: delete (SpotifyData*) tempData; break;
        case SPORTS:  delete (SportsData*) tempData;  break;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
    Serial.println("Looped");
  }
}