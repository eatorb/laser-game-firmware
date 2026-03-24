#pragma once

void setupMqtt();
void mqttLoop();
bool isMqttConnected();
void publishStatus(const char* status);
void publishHit(uint8_t shooterId);
