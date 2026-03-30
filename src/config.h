#pragma once

#include <stdint.h>

// Device metadata
constexpr uint8_t DEVICE_ID = 2U;
constexpr char FIRMWARE_VERSION[] = "1.1.0";

// WiFi network
constexpr char WIFI_SSID[] = "MS_DOMA";
constexpr char WIFI_PASSWORD[] = "DRIENOVO77";

// MQTT
constexpr char MQTT_HOST[] = "192.168.1.109";
constexpr uint16_t MQTT_PORT = 1884U;
constexpr char MQTT_USERNAME[] = "";
constexpr char MQTT_PASSWORD[] = "";

// MQTT topics
constexpr char MQTT_TOPIC_COMMAND[] = "game/control";
constexpr char MQTT_TOPIC_STATUS[] = "game/status";
constexpr char MQTT_TOPIC_HIT[] = "game/hits";
constexpr char MQTT_TOPIC_PLAYER_CMD[] = "game/player";
constexpr char MQTT_TOPIC_GEAR_CMD[] = "game/gear";

constexpr uint16_t MQTT_BUFFER_SIZE = 512U;
constexpr uint16_t MQTT_KEEP_ALIVE_SECONDS = 15U;

// Hardware pins (ESP32)
constexpr uint8_t PIN_IR_SEND = 25U;
constexpr uint8_t PIN_IR_RECEIVE = 26U;
constexpr uint8_t PIN_TRIGGER = 27U;
constexpr uint8_t PIN_STATUS_LED = 2U;

// Game constants
constexpr uint16_t SHOT_COOLDOWN_MS = 250U;
constexpr uint16_t RESPAWN_TIME_MS = 5000U;
constexpr uint8_t MAX_HEALTH = 100U;
constexpr uint16_t NETWORK_RETRY_INTERVAL_MS = 2000U;
constexpr uint8_t DAMAGE = 25U;
constexpr uint16_t RESPAWN_ANIMATION_MS = 2000U;