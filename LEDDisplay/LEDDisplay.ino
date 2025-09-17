#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#include "wifiClient.h"
#include "widgets.h"

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
const char *ssid = "x";
const char *password = "x";

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
}

void loop()
{
  widgetControl(display, &widget, WEATHER);
}

// void widgetControl(int widgetIdx)
// {
//   // Weather
//   if (widgetIdx == WidgetType.WEATHER_WIDGET) {
    
//     int result = fetchWeather(&widget);

//     // Refresh
//     if (!result) {
//       display->clearScreen();
//       display->setCursor(0, 0);

//       drawCenteredText(display, weather.city, 0);
//       display->println();

//       display->println(weather.statusDesc);

//       display->printf("%d", weather.currentTemp);
//       display->write(0xF8);

//       display->printf("\nH:%d", weather.highTemp);
//       display->write(0xF8);
      
//       display->printf("\nL:%d", weather.lowTemp);
//       display->write(0xF8);


//       display->drawRGBBitmap(
//         37, 16, 
//         weather.statusIcon, 
//         WEATHER_ICON_LEN, WEATHER_ICON_LEN
//       ); 
//     }
//   }
// }