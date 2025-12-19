#include "widgetManager.h"

void appTask(void *parameters) {
  Widget *w = (Widget *) parameters;
  
  int type = 0;
  initWidget(w, (WidgetType) type);
  
  bool isPressed;
  for (;;) {
    // Switch widget on button press
    if (xQueueReceive(buttonQueue, &isPressed, portMAX_DELAY)) {
      if (isPressed) {
        type = (type + 1) % NUM_WIDGETS;
        switchToWidget(w, (WidgetType) type);
      }
    }
  }
}

void initWidget(Widget *w, WidgetType type) {
  w->type = type;
  w->isInit = true;
  w->isLoaded = false;
  w->lastUpdate = 0;

  switch (type) {
    case WEATHER:
      w->weather = new WeatherData{};
      w->updateInterval = 5000;
      break;
    case SPOTIFY:
      w->spotify = new SpotifyData{};
      w->updateInterval = 2500;
      break;
    case SPORTS:
      w->sports = new SportsData{};
      w->updateInterval = 5000;
      break;
    case CLOCK:
      w->clock = new ClockData{};
      w->updateInterval = 300000; // 5 minutes
      break;
  }

  xQueueSend(widgetSwitchQueue, &type, 0);
}

void destroyWidget(Widget *w) {
  if (!w) return;

  switch (w->type) {
    case WEATHER:
      delete w->weather;
      w->weather = nullptr;
      break;
    case SPOTIFY:
      delete w->spotify;
      w->spotify = nullptr;
      break;
    case SPORTS:
      delete w->sports;
      w->sports = nullptr;
      break;
    case CLOCK:
      delete w->clock;
      w->clock = nullptr;
      break;
  }

  w->isInit = false;
}

void switchToWidget(Widget *w, WidgetType type) {
  xSemaphoreTake(widgetMutex, portMAX_DELAY);

  destroyWidget(w);
  initWidget(w, type);

  xSemaphoreGive(widgetMutex);
}