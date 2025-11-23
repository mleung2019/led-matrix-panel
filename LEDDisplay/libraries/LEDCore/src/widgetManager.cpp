#include "widgetManager.h"

void appTask(void *parameters) {
  int type = 0;
  Widget *w = (Widget *) parameters;

  bool isPressed;

  initWidget(w, (WidgetType) type);
  for (;;) {
    // Switch widget on button press
    if (xQueueReceive(buttonQueue, &isPressed, portMAX_DELAY)) {
      if (isPressed) {
        type = (type + 1) % 3;
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
      Serial.println("Create weather");
      break;
    case SPOTIFY:
      w->spotify = new SpotifyData{};
      w->updateInterval = 2500;
      Serial.println("Create spotify");
      break;
    case SPORTS:
      w->sports = new SportsData{};
      w->updateInterval = 5000;
      Serial.println("Create sports");
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
      Serial.println("Delete weather");
      break;
    case SPOTIFY:
      delete w->spotify;
      w->spotify = nullptr;
      Serial.println("Delete spotify");
      break;
    case SPORTS:
      delete w->sports;
      w->sports = nullptr;
      Serial.println("Delete sports");
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