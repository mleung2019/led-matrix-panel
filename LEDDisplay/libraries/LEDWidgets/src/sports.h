#ifndef SPORTS_H
#define SPORTS_H

#include <widgetTool.h>

struct SportsData {
  char sportName[16];
  char team1Name[8];
  char team1Score[8];
  uint16_t team1Icon[ICON_PIXELS];
  char team2Name[8];
  char team2Score[8];
  uint16_t team2Icon[ICON_PIXELS];
  Scroller shortDetail;
};

void drawSports(SportsData *pd);
int parseSports(SportsData *pd, StaticJsonDocument<1024> doc);

#endif