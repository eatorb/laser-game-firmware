#include "game/handler.h"

#include "config.h"
#include "ir/ir.h"
#include "state.h"

void handleTrigger(uint32_t nowMs) {
  if (deviceRuntimeState.state != DeviceState::IN_GAME) {
    return;
  }

  if ((nowMs - deviceRuntimeState.lastShotTimeMs) < SHOT_COOLDOWN_MS) {
    return;
  }

  deviceRuntimeState.lastShotTimeMs = nowMs;
  sendShotPacket(DEVICE_ID);
}

void handleIrReceive() {
  if (deviceRuntimeState.state != DeviceState::IN_GAME) {
    return;
  }

  // TODO: decode valid packet and apply damage/state transitions.
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
