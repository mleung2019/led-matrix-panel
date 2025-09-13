#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include "wifiClient.h"

WiFiUDP udp;
const int localPort = 5005;

uint16_t displayFrame[PANEL_PIXELS];
bool frameReady = false;

uint16_t tempFrame[PANEL_PIXELS];
uint32_t packetMask = 0;
uint16_t currentFrameID = 0xFFFF;

void connectWiFi(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    if (millis() - start > 15000) {
      // avoid hanging indefinitely; try again
      Serial.println("WiFi connect timeout, retrying...");
      start = millis();
      WiFi.begin(ssid, password);
    }
  }
  udp.begin(localPort);
  Serial.print("Connected to WiFi, local port: ");
  Serial.println(localPort);
}

void fetchGallery() {
  int packetSize;
  while ((packetSize = udp.parsePacket()) > 0) {
    uint8_t packet[1300];
    int len = udp.read(packet, 1300);

    uint16_t frameID = (packet[0] << 8) | packet[1];
    uint8_t totalPackets = packet[2];
    uint8_t packetIdx = packet[3];

    // Reset if new frame
    if (frameID != currentFrameID) {
      packetMask = 0;
      currentFrameID = frameID;
    }

    int payloadLen = len - 4;
    int offset = packetIdx * PAYLOAD_SIZE;
    if (offset + payloadLen <= FRAME_SIZE) {
      memcpy(((uint8_t *)tempFrame) + offset, packet + 4, payloadLen);
      packetMask |= (1UL << packetIdx);
    }

    uint32_t completeMask = (1UL << totalPackets) - 1;
    if (packetMask == completeMask) {
      memcpy(displayFrame, tempFrame, FRAME_SIZE);
      packetMask = 0;
      frameReady = true;
    }
  }
}

