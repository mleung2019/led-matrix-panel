#include "networkClient.h"
#include "networkManager.h"

volatile bool networkCancel = false;

void networkTask(void *parameters) {
  Widget *w = (Widget *) parameters;
  WidgetType currentType = (WidgetType) -1;
  WidgetType newType;

  for (;;) {
    // On widget transition, cancel existing network fetches
    while (xQueueReceive(widgetSwitchQueue, &newType, 0) == pdTRUE) {
      currentType = newType;
      networkCancel = true;
      w->lastUpdate = 0;    
    }

    // As a precaution, skip if no widget has been initialized yet
    if (currentType == (WidgetType) -1 || !w->isInit) {
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    } 

    unsigned long now = millis();
    if (!w->isLoaded || now - w->lastUpdate >= w->updateInterval) {
      Serial.println(
        String("Attempt fetching widget data for type ") + String(currentType)
      );
      w->lastUpdate = now;
      networkCancel = false;
      
      // Create a temporary pointer to new widget data
      void *tempData = nullptr;
      switch (currentType) {
        case WEATHER: tempData = new WeatherData{}; break;
        case SPOTIFY: tempData = new SpotifyData{}; break;
        case SPORTS:  tempData = new SportsData{};  break;
        case CLOCK:   tempData = new ClockData{}; break;
      }
      if (!tempData) continue;

      int error = 0;
      // Did we perform a valid fetch?
      if (!fetchWidget(w, tempData)) {
        // Do we have access to widget data?
        if (xSemaphoreTake(widgetMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
          // Is widget invalid?
          if (w->isInit && currentType == w->type) {
            switch (currentType) {
              case WEATHER: {
                WeatherData *wd = (WeatherData *) tempData;

                updateText(&wd->city, &w->weather->city);
                updateText(&wd->statusDesc, &w->weather->statusDesc);
                updateText(&wd->forecastStr, &w->weather->forecastStr);

                memcpy(w->weather, wd, sizeof(WeatherData));
                break;
              }
              case SPOTIFY: {
                SpotifyData *sd = (SpotifyData *) tempData;

                updateText(&sd->trackInfo, &w->spotify->trackInfo);

                memcpy(w->spotify, sd, sizeof(SpotifyData));
                break;
              }
              case SPORTS: {
                SportsData *pd = (SportsData *) tempData;

                updateText(&pd->shortDetail, &w->sports->shortDetail);

                memcpy(w->sports, pd, sizeof(SportsData));
                break;
              }
              case CLOCK: {
                ClockData *cd = (ClockData *) tempData;
                *w->clock = *cd;
                break;
              }
            }
            w->isLoaded = true;
          }
          xSemaphoreGive(widgetMutex);
        }
      }
      // If error == 1, image fetch failed; mark widget as not loaded
      else if (error == 1) {
        w->isLoaded = false;
      }

      switch (currentType) {
        case WEATHER: delete (WeatherData *) tempData; break;
        case SPOTIFY: delete (SpotifyData *) tempData; break;
        case SPORTS:  delete (SportsData *) tempData;  break;
        case CLOCK:   delete (ClockData *) tempData; break;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}