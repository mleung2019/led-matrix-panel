#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include "widgetManager.h"

#define DISPLAY_FPS 30
#define FRAME_INTERVAL_MS (1000 / DISPLAY_FPS)

extern MatrixPanel_I2S_DMA *display;
extern SemaphoreHandle_t widgetMutex;

/* ----------------------- FREERTOS TASKS ----------------------- */
void displayTask(void *parameters);

#endif