#include "clockw.h"

void drawClock(ClockData *sd) {

}

int parseClock(ClockData *sd, StaticJsonDocument<1024> doc) {
  sd->rtc.setTime(
    (int) doc["second"],
    (int) doc["minute"],
    (int) doc["hour"],
    (int) doc["day"],
    (int) doc["month"],
    (int) doc["year"]
  );

  return 0;
}