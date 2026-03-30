#include <Arduino.h>

#include "game/handler.h"
#include "ir/ir.h"
#include "led/led.h"
#include "network/mqtt.h"
#include "network/wifi.h"
#include "state.h"

void setup() {
  Serial.begin(115200);
  resetRuntimeState();

  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  setupLed();
  setupWifi();
  setupMqtt();
  setupIr();
}

void loop() {
  const uint32_t nowMs = millis();

  wifiLoop();
  mqttLoop();
  irLoop();
  ledLoop(nowMs);

  if (!isWifiConnected()) {
    deviceRuntimeState.state = DeviceState::CONNECTING_WIFI;
    return;
  }

  if (!isMqttConnected()) {
    deviceRuntimeState.state = DeviceState::CONNECTING_MQTT;
    return;
  }

  if (!deviceRuntimeState.gameActive) {
    deviceRuntimeState.state = DeviceState::IDLE;
    return;
  }

  if (deviceRuntimeState.state == DeviceState::DEAD || deviceRuntimeState.state == DeviceState::RESPAWNING) {
    processRespawn(nowMs);
    return;
  }

  deviceRuntimeState.state = DeviceState::IN_GAME;
  handleTrigger(nowMs);
}
