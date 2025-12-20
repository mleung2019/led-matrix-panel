#ifndef CLOCKW_H
#define CLOCKW_H

#include <ESP32Time.h>
#include <widgetTool.h>

struct ClockData {
  ESP32Time rtc;
};

void drawClock(ClockData *cd);
void drawCenterCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color); 
void drawCenterTickMarks(int cx, int cy,
                   int innerRadius,
                   int outerRadius,
                   float angleDeg,
                   uint16_t color);
void drawTickMarks(int cx, int cy,
                   int innerRadius,
                   int outerRadius,
                   float angleDeg,
                   uint16_t color);
void drawCenterHand(int cx, int cy, int length, float angleDeg, uint16_t color);
void drawHand(int cx, int cy, int length, float angleDeg, uint16_t color);
int parseClock(ClockData *cd, StaticJsonDocument<1024> doc);

#endif