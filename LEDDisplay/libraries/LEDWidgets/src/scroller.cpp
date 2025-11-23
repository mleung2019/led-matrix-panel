#include <widgetTool.h>

// Update scroller position
bool updateScroller(Scroller *s) {
  TickType_t now = xTaskGetTickCount();
  if (s->active && (now - s->lastUpdate) * portTICK_PERIOD_MS >= s->updateInterval) {
    s->lastUpdate = now;
    s->x--;
    return true;
  }
  return false;
}

// When scroller info changes, we need to determine if scrolling should
// be active
void resizeScroller(Scroller *s) {
  int16_t x1, y1;
  uint16_t w, h;
  display->getTextBounds(s->msg, 0, 0, &x1, &y1, &w, &h);
  
  if (w > display->width()) {
    s->active = true;
  } else {
    s->active = false;
  }
}

// If active, scroll msg. If inactive, center msg
void drawScroller(Scroller *s, int y) {
  updateScroller(s);
  resizeScroller(s);
  s->y = y;

  display->setTextColor(s->color);
  if (s->active) {
    int16_t x1, y1;
    uint16_t w, h;
    display->getTextBounds(s->msg, 0, 0, &x1, &y1, &w, &h);

    if (s->x < -w) {
      s->x = display->width();
    }

    display->setCursor(s->x, s->y);
    display->print(s->msg);
  } else {
    drawCenteredText(s->msg, s->y);
  }
  display->setTextColor(0xFFFF);
}

// Preserve scroller struct variables when updating text
void updateText(Scroller *sDest, Scroller *sSrc) {
  Scroller sTemp = *sSrc;
  strcpy(sTemp.msg, sDest->msg);
  *sDest = sTemp;
}