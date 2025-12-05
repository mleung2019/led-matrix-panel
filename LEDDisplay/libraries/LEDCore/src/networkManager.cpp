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
    if (now - w->lastUpdate >= w->updateInterval) {
      w->lastUpdate = now;
      networkCancel = false;
      
      // Create a temporary pointer to new widget data
      void *tempData = nullptr;
      switch (currentType) {
        case WEATHER: tempData = new WeatherData{}; break;
        case SPOTIFY: tempData = new SpotifyData{}; break;
        case SPORTS:  tempData = new SportsData{};  break;
        default: break;
      }
      if (!tempData) continue;

      if (!fetchWidget(w, tempData) && w->isInit && currentType == w->type) {
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
        }
        w->isLoaded = true;
      }

      switch (currentType) {
        case WEATHER: delete (WeatherData*) tempData; break;
        case SPOTIFY: delete (SpotifyData*) tempData; break;
        case SPORTS:  delete (SportsData*) tempData;  break;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}