#include "network/wifi.h"

#include <Arduino.h>
#include <WiFi.h>

#include "config.h"

namespace {
uint32_t lastWifiConnectAttemptMs = 0U;
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWifiConnectAttemptMs = millis();
  Serial.println("[WiFi] Connecting...");
}

void wifiLoop() {

  static bool wasConnected = false;
  const bool connected = isWifiConnected();

  if (connected && !wasConnected) {
    Serial.printf("[WiFi] Connected — IP: %s\n", WiFi.localIP().toString().c_str());
  }

  if (!connected && wasConnected) {
    Serial.println("[WiFi] Disconnected");
  }

  wasConnected = connected;

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  const uint32_t nowMs = millis();
  if ((nowMs - lastWifiConnectAttemptMs) < NETWORK_RETRY_INTERVAL_MS) {
    return;
  }

  lastWifiConnectAttemptMs = nowMs;
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("[WiFi] Reconnecting...");
}

bool isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}
