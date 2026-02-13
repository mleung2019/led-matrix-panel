#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <Arduino.h>
#include <esp_system.h>

#include "widgetManager.h"

extern SemaphoreHandle_t widgetMutex;
extern QueueHandle_t widgetSwitchQueue;

/* ----------------------- FREERTOS TASKS ----------------------- */
void networkTask(void *parameters);

#endif