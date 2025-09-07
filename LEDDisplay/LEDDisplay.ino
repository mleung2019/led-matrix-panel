#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
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

const char *ssid = "REDACTED";
const char *password = "REDACTED";

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Attempting to connect to WiFi");
  connectWiFi(ssid, password);

  delay(200);

  Serial.println("Starting display");
  //mxconfig.double_buff = true; // Turn off double buffer
  mxconfig.clkphase = false;
  mxconfig.gpio.e = 32;
  
  display = new MatrixPanel_I2S_DMA(mxconfig);
  display->begin();  // Setup display with pins as pre-defined in the library
  display->setBrightness8(90);
  display->clearScreen();
}

void loop()
{
  // display->flipDMABuffer(); // Not used if double buffering isn't enabled
  // delay(25);
  // display->clearScreen();

  display->drawRGBBitmap(0, 0, convertFrame(fetchGallery()), PANEL_RES_X, PANEL_RES_Y);
  delay(1000);
}
