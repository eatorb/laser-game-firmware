#include "game/handler.h"

#include <ArduinoJson.h>

#include "config.h"
#include "ir/ir.h"
#include "state.h"

void handleTrigger(uint32_t nowMs) {
  if (deviceRuntimeState.state != DeviceState::IN_GAME) return;
  if (digitalRead(PIN_TRIGGER) != LOW) return;
  if ((nowMs - deviceRuntimeState.lastShotTimeMs) < SHOT_COOLDOWN_MS) return;

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

void handleMqttCommand(const char* topic, const uint8_t* payload, unsigned int length) {
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.printf("[MQTT] Invalid JSON on %s\n", topic);
    return;
  }

  const char* type = doc["type"];
  if (!type) return;

  const uint32_t nowMs = millis();

  if (strcmp(type, "game_start") == 0) {
    beginGame(nowMs);
    Serial.println("[GAME] Started");

  } else if (strcmp(type, "game_end") == 0) {
    resetRuntimeState();
    Serial.println("[GAME] Ended");

  } else if (strcmp(type, "death") == 0) {
    markPlayerDead(nowMs);
    Serial.println("[GAME] Dead");

  } else if (strcmp(type, "respawn") == 0) {
    beginRespawn(nowMs);
    Serial.println("[GAME] Respawn");

  } else if (strcmp(type, "friendly_fire") == 0) {
    Serial.println("[GAME] Friendly fire");
  }
}