#include <networkClient.h>
#include <inputManager.h>
#include <widgetManager.h>
#include <displayManager.h>
#include <networkManager.h>

/*--------------------- MATRIX PANEL CONFIG -------------------------*/
#define PANEL_RES_X 64  // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 64  // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1   // Total number of panels chained one to another

MatrixPanel_I2S_DMA *display = nullptr;

// Module configuration
HUB75_I2S_CFG mxconfig(
  PANEL_RES_X,  // Module width
  PANEL_RES_Y,  // Module height
  PANEL_CHAIN   // Chain length
);

// Input
QueueHandle_t buttonQueue = xQueueCreate(10, sizeof(bool));

// Widget
Widget widget;
SemaphoreHandle_t widgetMutex = xSemaphoreCreateMutex();
QueueHandle_t widgetSwitchQueue = xQueueCreate(5, sizeof(WidgetType));

void setup() {
  // Serial
  Serial.begin(115200);
  delay(1000);

  // WiFi
  Serial.println("Attempting to connect to WiFi");
  connectWiFi();
  initLocation();

  // Matrix panel
  Serial.println("Starting display");
  mxconfig.double_buff = true;
  mxconfig.clkphase = false;
  mxconfig.gpio.e = 32;

  // Setup display with pins as pre-defined in the library
  display = new MatrixPanel_I2S_DMA(mxconfig);
  display->begin();
  display->setBrightness8(50);
  display->clearScreen();
  display->cp437(true);
  display->setTextWrap(false);

  xTaskCreatePinnedToCore(buttonTask, "ButtonTask", 2048, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(appTask, "AppTask", 2048, &widget, 2, NULL, 1);
  xTaskCreatePinnedToCore(displayTask, "DisplayTask", 4096, &widget, 2, NULL, 1);
  xTaskCreatePinnedToCore(networkTask, "NetworkTask", 16384, &widget, 1, NULL, 0);
}

void loop() {

}