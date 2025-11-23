#ifndef WIDGETTOOL_H
#define WIDGETTOOL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include <scroller.h>

#define PANEL_LENGTH 64
#define PANEL_PIXELS PANEL_LENGTH*PANEL_LENGTH
#define BUFFER_SIZE PANEL_PIXELS*2 

#define ICON_LENGTH 24
#define ICON_PIXELS ICON_LENGTH*ICON_LENGTH

extern SemaphoreHandle_t widgetMutex;
extern MatrixPanel_I2S_DMA *display;

/* -------------------- ADAFRUIT GFX HELPER FUNCTIONS -------------------- */
void drawCenteredText(const char *msg, int y, int width=0, int offset=0);

#endif