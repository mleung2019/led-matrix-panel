#ifndef WIFICLIENT_H
#define WIFICLIENT_H

#define PANEL_PIXELS 64*64
#define FRAME_SIZE PANEL_PIXELS*2
#define PAYLOAD_SIZE 1400

extern uint16_t displayFrame[PANEL_PIXELS];

void connectWiFi(const char *ssid, const char *password);
void fetchGallery();

#endif
