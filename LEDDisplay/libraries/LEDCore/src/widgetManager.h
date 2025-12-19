#ifndef WIDGETMANAGER_H
#define WIDGETMANAGER_H

#include <Arduino.h>

#include <spotify.h>
#include <weather.h>
#include <sports.h>
#include <clockw.h>

enum WidgetType {
  SPOTIFY,
  CLOCK,
  WEATHER,
  SPORTS,
  NUM_WIDGETS
};

// BASE WIDGET STRUCT
struct Widget {
  volatile WidgetType type;
  // Cache and timing variables
  bool isInit = false;
  bool isLoaded = false;
  unsigned long lastUpdate = 0;
  unsigned long updateInterval = 5000;
  // Data
  WeatherData *weather = nullptr;
  SpotifyData *spotify = nullptr;
  SportsData *sports = nullptr;
  ClockData *clock = nullptr; 
};

extern QueueHandle_t buttonQueue;
extern SemaphoreHandle_t widgetMutex;
extern QueueHandle_t widgetSwitchQueue;

/* ----------------------- FREERTOS TASKS ----------------------- */
void appTask(void *parameters);

/* ----------------------- WIDGET CONTROL ----------------------- */
void initWidget(Widget *w, WidgetType type);
void destroyWidget(Widget *w);
void switchToWidget(Widget *w, WidgetType type);

#endif