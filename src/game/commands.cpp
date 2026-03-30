#include "game/commands.h"

#include <ArduinoJson.h>
#include <Arduino.h>

#include "state.h"
#include "ota/ota.h"

namespace {

void onGameStart(uint32_t nowMs) {
  beginGame(nowMs);
  Serial.println("[GAME] Started");
}

void onGameEnd(uint32_t nowMs) {
  (void)nowMs;
  resetRuntimeState();
  Serial.println("[GAME] Ended");
}

void onDeath(uint32_t nowMs) {
  markPlayerDead(nowMs);
  Serial.println("[GAME] Dead");
}

void onRespawn(uint32_t nowMs) {
  beginRespawn(nowMs);
  Serial.println("[GAME] Respawn");
}

void onFriendlyFire(uint32_t nowMs) {
  (void)nowMs;
  Serial.println("[GAME] Friendly fire");
}

struct CommandHandler {
  const char* type;
  void (*handle)(uint32_t nowMs);
};

constexpr CommandHandler HANDLERS[] = {
  { "game_start",    onGameStart    },
  { "game_end",      onGameEnd      },
  { "death",         onDeath        },
  { "respawn",       onRespawn      },
  { "friendly_fire", onFriendlyFire },
};

}  // namespace

void handleMqttCommand(const char* topic, const uint8_t* payload, unsigned int length) {
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload, length);

  if (err) {
    Serial.printf("[MQTT] Invalid JSON on %s\n", topic);
    return;
  }

  const char* type = doc["type"];

  if (!type) return;

  Serial.printf("[MQTT] Received command: %s\n", type);
  Serial.printf("[MQTT] Type len: %d\n", strlen(type));
  Serial.printf("[MQTT] strcmp result: %d\n", strcmp(type, "firmware_update"));

  const uint32_t nowMs = millis();

  if (strcmp(type, "firmware_update") == 0) {
    const char* url = doc["url"];
    uint32_t size   = doc["size"] | 0;
    if (url) {
      handleOtaUpdate(url, size);
    }
    return;
  }

  for (const auto& handler : HANDLERS) {
    if (strcmp(type, handler.type) == 0) {
      handler.handle(nowMs);
      return;
    }
  }

  Serial.printf("[MQTT] Unknown type: %s\n", type);
}