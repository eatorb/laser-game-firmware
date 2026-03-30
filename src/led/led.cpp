#include "led/led.h"

#include <Arduino.h>

#include "config.h"
#include "state.h"

static uint32_t lastToggleMs = 0U;
static bool ledOn = false;

static void setLed(bool on) {
  ledOn = on;
  digitalWrite(PIN_STATUS_LED, on ? HIGH : LOW);
}

static void blinkLed(uint32_t nowMs, uint32_t intervalMs) {
  if ((nowMs - lastToggleMs) >= intervalMs) {
    lastToggleMs = nowMs;
    setLed(!ledOn);
  }
}

void setupLed() {
  pinMode(PIN_STATUS_LED, OUTPUT);
  setLed(false);
}

void ledLoop(uint32_t nowMs) {
  switch (deviceRuntimeState.state) {
    case DeviceState::BOOT:
    case DeviceState::CONNECTING_WIFI:
      blinkLed(nowMs, 100U);
      break;

    case DeviceState::CONNECTING_MQTT:
      blinkLed(nowMs, 400U);
      break;

    case DeviceState::IDLE:
      blinkLed(nowMs, 1000U);
      break;

    case DeviceState::IN_GAME:
      setLed(true);
      break;

    case DeviceState::DEAD:
      setLed(false);
      break;

    case DeviceState::RESPAWNING:
      blinkLed(nowMs, 150U);
      break;

    case DeviceState::OTA_UPDATING:
      blinkLed(nowMs, 50U);
      break;
  }
}
