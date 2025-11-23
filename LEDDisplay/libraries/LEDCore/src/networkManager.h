#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <Arduino.h>

#include "widgetManager.h"

#define DISPLAY_FPS 60
#define FRAME_INTERVAL_MS (1000 / DISPLAY_FPS)

extern SemaphoreHandle_t widgetMutex;
extern QueueHandle_t widgetSwitchQueue;

/* ----------------------- FREERTOS TASKS ----------------------- */
void networkTask(void *parameters);

#endif