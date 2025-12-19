#ifndef CLOCKW_H
#define CLOCKW_H

#include <ESP32Time.h>
#include <widgetTool.h>

struct ClockData {
  ESP32Time rtc;
};

void drawClock(ClockData *cd);
int parseClock(ClockData *cd, StaticJsonDocument<1024> doc);

#endif