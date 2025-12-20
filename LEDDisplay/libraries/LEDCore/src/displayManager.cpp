#include "displayManager.h"

void displayTask(void *parameters) {
  Widget *w = (Widget *) parameters;
  TickType_t lastWakeTime = xTaskGetTickCount();

  for (;;) {
    if (w->isInit && xSemaphoreTake(widgetMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
      // Draw frame
      display->clearScreen();
      display->setCursor(0, 0);

      switch (w->type) {
        case WEATHER:
          if (w->isLoaded) drawWeather(w->weather);
          else drawCenteredText("WEATHER", 56);
          break;
        case SPOTIFY:
          if (w->isLoaded) drawSpotify(w->spotify);
          else drawCenteredText("SPOTIFY", 56);
          break;
        case SPORTS:
          if (w->isLoaded) drawSports(w->sports);
          else drawCenteredText("SPORTS", 56);
          break;
        case CLOCK: {
          if (w->isLoaded) drawClock(w->clock);
          else drawCenteredText("CLOCK", 56);
          break;
        }
      }

      display->flipDMABuffer();
      xSemaphoreGive(widgetMutex);
    }

    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(FRAME_INTERVAL_MS));
  }
}
