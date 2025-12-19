#ifndef CLOCKW_H
#define CLOCKW_H

#include <widgetTool.h>

struct ClockData {
  int hour;
  int min;
  int sec;
};

void drawClock(ClockData *cd);
int parseClock(ClockData *cd, StaticJsonDocument<1024> doc);

#endif