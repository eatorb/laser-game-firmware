#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"
#include "mqtt.h"
#include "wifi.h"
#include "game/commands.h"

namespace {
  WiFiClient wifiClient;
  PubSubClient mqttClient;
  uint32_t lastMqttConnectAttemptMs = 0U;

  void mqttMessageCallback(char* topic, uint8_t* payload, unsigned int length) {
    handleMqttCommand(topic, payload, length);
  }

}

void setupMqtt() {
  mqttClient.setClient(wifiClient);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttMessageCallback);
  mqttClient.setBufferSize(MQTT_BUFFER_SIZE);
  mqttClient.setKeepAlive(MQTT_KEEP_ALIVE_SECONDS);
}

void mqttLoop() {
  mqttClient.loop();

  if (!isWifiConnected() || mqttClient.connected()) return;

  const uint32_t nowMs = millis();
  if ((nowMs - lastMqttConnectAttemptMs) < NETWORK_RETRY_INTERVAL_MS) return;

  lastMqttConnectAttemptMs = nowMs;

  char clientId[32]       = {0};
  char lwtPayload[128]    = {0};
  char playerCmdTopic[64] = {0};
  char gearCmdTopic[64] = {0};

  snprintf(clientId, sizeof(clientId),
    "device-%u", DEVICE_ID);

  snprintf(lwtPayload, sizeof(lwtPayload),
    "{\"deviceId\":%u,\"status\":\"offline\",\"firmwareVersion\":\"%s\"}",
    DEVICE_ID, FIRMWARE_VERSION);

  snprintf(playerCmdTopic, sizeof(playerCmdTopic),
    "%s/%u/command", MQTT_TOPIC_PLAYER_CMD, DEVICE_ID);

  snprintf(gearCmdTopic, sizeof(gearCmdTopic),
    "%s/%u/command", MQTT_TOPIC_GEAR_CMD, DEVICE_ID);

  const bool connected = mqttClient.connect(
    clientId,
    MQTT_USERNAME,
    MQTT_PASSWORD,
    MQTT_TOPIC_STATUS,
    1,      // LWT QoS 1
    false,  // LWT retain
    lwtPayload
  );

  if (!connected) {
    Serial.printf("[MQTT] Reconnect failed rc=%d\n", mqttClient.state());
    return;
  }

  // Subscribe
  mqttClient.subscribe(MQTT_TOPIC_COMMAND);  // game/control
  mqttClient.subscribe(playerCmdTopic);       // game/player/{id}/command
  mqttClient.subscribe(gearCmdTopic);

  // Publish online status
  char onlinePayload[128] = {0};
  snprintf(onlinePayload, sizeof(onlinePayload),
    "{\"deviceId\":%u,\"status\":\"online\",\"firmwareVersion\":\"%s\"}",
    DEVICE_ID, FIRMWARE_VERSION);
  mqttClient.publish(MQTT_TOPIC_STATUS, onlinePayload, false);

  Serial.printf("[MQTT] Connected — clientId: %s\n", clientId);
}

bool isMqttConnected() {
  return mqttClient.connected();
}

void publishStatus(const char* status) {
  if (!mqttClient.connected() || status == nullptr) return;
  mqttClient.publish(MQTT_TOPIC_STATUS, status, true);
}

void publishHit(uint8_t shooterId) {
  if (!mqttClient.connected()) return;

  char payload[128] = {0};
  snprintf(payload, sizeof(payload),
    "{\"shooter\":%u,\"victim\":%u,\"damage\":%u}",
    shooterId, DEVICE_ID, DAMAGE);

  mqttClient.publish(MQTT_TOPIC_HIT, payload, false);
  Serial.printf("[MQTT] Hit published — shooter:%u victim:%u damage:%u\n",
    shooterId, DEVICE_ID, DAMAGE);
}