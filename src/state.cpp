#include "state.h"

DeviceRuntimeState deviceRuntimeState = {
    DeviceState::BOOT,
    MAX_HEALTH,
    false,
    0U,
    0U,
};

void resetRuntimeState() {
  deviceRuntimeState.state = DeviceState::BOOT;
  deviceRuntimeState.hp = MAX_HEALTH;
  deviceRuntimeState.gameActive = false;
  deviceRuntimeState.lastShotTimeMs = 0U;
  deviceRuntimeState.respawnStartedAtMs = 0U;
}

void beginGame(uint32_t nowMs) {
  deviceRuntimeState.state = DeviceState::IN_GAME;
  deviceRuntimeState.hp = MAX_HEALTH;
  deviceRuntimeState.gameActive = true;
  deviceRuntimeState.lastShotTimeMs = nowMs;
  deviceRuntimeState.respawnStartedAtMs = 0U;
}

void markPlayerDead(uint32_t nowMs) {
  deviceRuntimeState.state = DeviceState::DEAD;
  deviceRuntimeState.gameActive = true;
  deviceRuntimeState.respawnStartedAtMs = nowMs;
}

void beginRespawn(uint32_t nowMs) {
  deviceRuntimeState.state = DeviceState::RESPAWNING;
  deviceRuntimeState.hp = MAX_HEALTH;
  deviceRuntimeState.gameActive = true;
  deviceRuntimeState.respawnStartedAtMs = nowMs;
}
