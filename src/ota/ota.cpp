#include "ota/ota.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFi.h>

#include "state.h"

void handleOtaUpdate(const char* url, uint32_t size) {
  if (deviceRuntimeState.gameActive) {
    Serial.println("[OTA] Ignored — game active");
    return;
  }

  deviceRuntimeState.state = DeviceState::OTA_UPDATING;

  Serial.printf("[OTA] Starting update from %s (%u bytes)\n", url, size);

  WiFiClient client;

  httpUpdate.onStart([]() {
    Serial.println("[OTA] Start");
  });

  httpUpdate.onProgress([](int cur, int total) {
    Serial.printf("[OTA] %d / %d bytes\n", cur, total);
  });

  httpUpdate.onError([](int err) {
    Serial.printf("[OTA] Error: %d\n", err);
  });

  t_httpUpdate_return ret = httpUpdate.update(client, url);

  switch (ret) {
    case HTTP_UPDATE_OK:
      Serial.println("[OTA] Success — rebooting");
      // ESP32 sa automaticky reštartuje
      break;

    case HTTP_UPDATE_FAILED:
      Serial.printf("[OTA] Failed: %s\n",
        httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[OTA] No update needed");
      break;
  }
}