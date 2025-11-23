#include "inputManager.h"

void buttonTask(void *parameters) {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  bool isPressed;
  bool lastState = HIGH;

  for (;;) {
    bool current = digitalRead(BUTTON_PIN);
    
    if (current == LOW && lastState == HIGH) {
      isPressed = true;
      xQueueSend(buttonQueue, &isPressed, 0);

      vTaskDelay(pdMS_TO_TICKS(250)); // debounce
    }

    lastState = current;
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}