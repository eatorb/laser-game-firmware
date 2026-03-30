#pragma once

#include <stdint.h>

#include "config.h"

enum class DeviceState : uint8_t {
  BOOT = 0U,
  CONNECTING_WIFI,
  CONNECTING_MQTT,
  IDLE,
  IN_GAME,
  DEAD,
  RESPAWNING,
  OTA_UPDATING,
};

struct DeviceRuntimeState {
  DeviceState state;
  uint8_t hp;
  bool gameActive;
  uint32_t lastShotTimeMs;
  uint32_t respawnStartedAtMs;
};

extern DeviceRuntimeState deviceRuntimeState;

void resetRuntimeState();
void beginGame(uint32_t nowMs);
void markPlayerDead(uint32_t nowMs);
void beginRespawn(uint32_t nowMs);
