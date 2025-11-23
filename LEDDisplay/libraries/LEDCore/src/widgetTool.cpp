#include "widgetTool.h"

void drawCenteredText( 
  const char* msg, int y, 
  int width, int offset
) {
  int16_t x1, y1;
  uint16_t w, h;
  display->getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);

  // Compute centered X
  if (!width) {
    width = display->width();
  }
  int x = ((width - w) / 2) + offset;

  // Draw the text
  display->setCursor(x, y);
  display->print(msg);
}