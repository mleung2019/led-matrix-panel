#ifndef SCROLLER_H
#define SCROLLER_H

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *display;

#define SCROLLER_SIZE 128

struct Scroller {
  char msg[SCROLLER_SIZE] = {};
  // Position
  int16_t x;
  int y;
  uint16_t color = 0xFFFF;
  // Timing
  bool active = false;
  unsigned long lastUpdate = 0;
  unsigned long updateInterval = 30;
};

bool updateScroller(Scroller *s);
void drawScroller(Scroller *s);
void resizeScroller(Scroller *s);

#endif