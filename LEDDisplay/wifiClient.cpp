#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include "wifiClient.h"

WiFiUDP udp;
const int localPort = 5005;

uint16_t tempFrame[PANEL_PIXELS];
uint16_t displayFrame[PANEL_PIXELS];
bool packetReceived[10];
uint16_t currentFrameID = 0xFFFF;

void connectWiFi(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  udp.begin(localPort);
  Serial.println("Connected to WiFi");
}

void fetchGallery() {
  while (int packetSize = udp.parsePacket()) {
    if (packetSize) {
      uint8_t packet[1500];
      int len = udp.read(packet, 1500);

      // Extract header
      uint16_t frameID = (packet[0] << 8) | packet[1];
      uint8_t packetIdx = packet[2];
      uint8_t totalPackets = packet[3];

      int payloadLen = len - 4;
      int offset = packetIdx * 1400;
      if (offset + payloadLen <= FRAME_SIZE) {
        memcpy(((uint8_t *)tempFrame) + offset, packet + 4, payloadLen);
        packetReceived[packetIdx] = true;
      }

      bool complete = true;
      for (int i = 0; i < totalPackets; i++) {
        if (!packetReceived[i]) { complete = false; break; }
      }

      if (complete) {
        // Copy assembled frame
        memcpy(displayFrame, tempFrame, FRAME_SIZE);

        // Reset packet tracker
        for (int i = 0; i < totalPackets; i++) packetReceived[i] = false;

        currentFrameID = frameID;
      }
    }
  }
}
