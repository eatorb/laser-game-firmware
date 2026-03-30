#include "game/handler.h"

#include <Arduino.h>

#include "config.h"
#include "ir/ir.h"
#include "state.h"

void handleTrigger(uint32_t nowMs) {
  if (deviceRuntimeState.state != DeviceState::IN_GAME) return;
  if (digitalRead(PIN_TRIGGER) != LOW) return;
  if ((nowMs - deviceRuntimeState.lastShotTimeMs) < SHOT_COOLDOWN_MS) return;

  Serial.println("[GAME] Shot fired");
  deviceRuntimeState.lastShotTimeMs = nowMs;
  sendShotPacket(DEVICE_ID);
}

void processRespawn(uint32_t nowMs) {
  if (deviceRuntimeState.state == DeviceState::DEAD) {
    if ((nowMs - deviceRuntimeState.respawnStartedAtMs) < RESPAWN_TIME_MS) return;
    beginRespawn(nowMs);
    return;
  }

  if (deviceRuntimeState.state == DeviceState::RESPAWNING) {
    if ((nowMs - deviceRuntimeState.respawnStartedAtMs) < RESPAWN_ANIMATION_MS) return;
    deviceRuntimeState.state = DeviceState::IN_GAME;
    return;
  }
}