#include "config.h"
#include "wifiClient.h"

/*--------------------- MATRIX PANEL CONFIG -------------------------*/
#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 64     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another

MatrixPanel_I2S_DMA *display = nullptr;

// Module configuration
HUB75_I2S_CFG mxconfig(
  PANEL_RES_X,   // Module width
  PANEL_RES_Y,   // Module height
  PANEL_CHAIN    // Chain length
);

// Widget
Widget widget;

// WiFi
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

void setup()
{
  // Serial
  Serial.begin(115200);
  delay(1000);

  // WiFi
  Serial.println("Attempting to connect to WiFi");
  connectWiFi(ssid, password);
  delay(200);

  // Display config
  Serial.println("Starting display");
  mxconfig.double_buff = true;
  mxconfig.clkphase = false;
  mxconfig.gpio.e = 32;
  
  display = new MatrixPanel_I2S_DMA(mxconfig);
  display->begin();  // Setup display with pins as pre-defined in the library
  display->setBrightness8(50);
  display->clearScreen();

  display->cp437(true);
  display->setTextWrap(false);

  // Primary fetching task used by the LED panel for most server requests
  xTaskCreate(
    fetchTask,
    "FetchTask",
    4096,
    (void *)&widget,
    1,     
    nullptr
  );
  xTaskCreate(
    galleryProducerTask,
    "GalleryTask",
    8192,
    (void *)&widget,
    1,
    nullptr
  );
}

int type = 0;
unsigned long lastUpdate = 0;
unsigned long updateInterval = 10000;

void loop()
{
  unsigned long now = millis();
  if (now - lastUpdate >= updateInterval) {
    lastUpdate = now;
    type = (type + 1) % 4;
  }

  widgetControl(display, &widget, (WidgetType) type);
}