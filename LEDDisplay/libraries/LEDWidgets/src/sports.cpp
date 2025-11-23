#include "sports.h"

void drawSports(SportsData *pd) {
  updateScroller(&pd->shortDetail);

  // SPORT NAME
  drawCenteredText(pd->sportName, 2);

  // TEAM NAMES
  drawCenteredText(pd->team1Name, 11, 32);
  drawCenteredText(pd->team2Name, 11, 32, 32);
  
  // TEAM ICONS
  display->drawRGBBitmap(
    0+4, 20, 
    pd->team1Icon, 
    ICON_LENGTH, ICON_LENGTH
  ); 
  display->drawRGBBitmap(
    64-ICON_LENGTH-4, 20, 
    pd->team2Icon, 
    ICON_LENGTH, ICON_LENGTH
  ); 

  // TEAM SCORES
  drawCenteredText(pd->team1Score, 46, 32);
  drawCenteredText(pd->team2Score, 46, 32, 32);

  // SHORT DETAIL
  resizeScroller(&pd->shortDetail);
  pd->shortDetail.y = 55;

  drawScroller(&pd->shortDetail);
}

void parseSports(SportsData *pd, StaticJsonDocument<1024> doc) {
  strncpy(pd->sportName, doc["sport_name"], sizeof(pd->sportName));
  strncpy(pd->team1Name, doc["team1_name"], sizeof(pd->team1Name));
  strncpy(pd->team1Score, doc["team1_score"], sizeof(pd->team1Score));
  strncpy(pd->team2Name, doc["team2_name"], sizeof(pd->team2Name));
  strncpy(pd->team2Score, doc["team2_score"], sizeof(pd->team2Score));
  strncpy(pd->shortDetail.msg, doc["short_detail"], sizeof(pd->shortDetail.msg));

  Serial.println(pd->shortDetail.msg);
}