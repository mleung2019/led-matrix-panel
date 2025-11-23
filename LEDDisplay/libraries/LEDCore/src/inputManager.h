#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <Arduino.h>

#define BUTTON_PIN 21

extern QueueHandle_t buttonQueue;

/* ----------------------- FREERTOS TASKS ----------------------- */
void buttonTask(void *parameters);

#endif