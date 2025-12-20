#include "clockw.h"

void drawClock(ClockData *cd) {
  // TIME
  String timeStr = cd->rtc.getTime("%I:%M %p");
  drawCenteredText(timeStr.c_str(), 1);

  // CLOCK
  drawCenterCircle(31, 31, 21, 0x8c51);

  int hour = cd->rtc.getHour();
  int min = cd->rtc.getMinute();
  int sec = cd->rtc.getSecond();

  // TICK MARKS
  for (int i = 0; i < 12; i++) {
    float hrAngle = i * 30;
    if (i % 3 == 0) drawCenterTickMarks(31, 31, 16, 21, hrAngle, 0x18e3);
    else drawCenterTickMarks(31, 31, 18, 21, hrAngle, 0x39a7);
  }

  // HOUR
  float hrAngle = (hour % 12) * 30 + min * 0.5;
  drawCenterHand(31, 31, 12, hrAngle, 0x0000);

  // MINUTE
  float minAngle = min * 6;
  drawCenterHand(31, 31, 19, minAngle, 0x0000);

  // SECONDS
  float secAngle = sec * 6;
  drawCenterHand(31, 31, 19, secAngle, 0xf800);
  
  // DATE
  String dateStr = cd->rtc.getTime("%a %m/%d");
  drawCenteredText(dateStr.c_str(), 56);
}

void drawCenterCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  display->fillCircle(x0, y0, r, color);
  display->fillCircle(x0, y0+1, r, color);
  display->fillCircle(x0+1, y0, r, color);
  display->fillCircle(x0+1, y0+1, r, color);
}

void drawCenterTickMarks(int cx, int cy,
                   int innerRadius,
                   int outerRadius,
                   float angleDeg,
                   uint16_t color) 
{
  drawTickMarks(cx, cy, innerRadius, outerRadius, angleDeg, color);
  drawTickMarks(cx, cy+1, innerRadius, outerRadius, angleDeg, color);
  drawTickMarks(cx+1, cy, innerRadius, outerRadius, angleDeg, color);
  drawTickMarks(cx+1, cy+1, innerRadius, outerRadius, angleDeg, color);
}

void drawTickMarks(int cx, int cy,
                   int innerRadius,
                   int outerRadius,
                   float angleDeg,
                   uint16_t color) 
{
  float angleRad = (angleDeg - 90) * DEG_TO_RAD;

  int x1 = cx + cos(angleRad) * innerRadius;
  int y1 = cy + sin(angleRad) * innerRadius;

  int x2 = cx + cos(angleRad) * outerRadius;
  int y2 = cy + sin(angleRad) * outerRadius;

  display->drawLine(x1, y1, x2, y2, color);
}

void drawCenterHand(int cx, int cy, int length, float angleDeg, uint16_t color) {
  drawHand(cx, cy, length, angleDeg, color);
  drawHand(cx, cy+1, length, angleDeg, color);
  drawHand(cx+1, cy, length, angleDeg, color);
  drawHand(cx+1, cy+1, length, angleDeg, color);
}

void drawHand(int cx, int cy, int length, float angleDeg, uint16_t color) {
  float angleRad = (angleDeg - 90) * DEG_TO_RAD;

  int x = cx + cos(angleRad) * length;
  int y = cy + sin(angleRad) * length;

  display->drawLine(cx, cy, x, y, color);  
}

int parseClock(ClockData *cd, StaticJsonDocument<1024> doc) {
  cd->rtc.setTime(
    (int) doc["second"],
    (int) doc["minute"],
    (int) doc["hour"],
    (int) doc["day"],
    (int) doc["month"],
    (int) doc["year"]
  );
  cd->rtc.offset = 0.5;

  return 0;
}