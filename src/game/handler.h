#pragma once

#include <stdint.h>

void handleTrigger(uint32_t nowMs);
void processRespawn(uint32_t nowMs);
void handleMqttCommand(const char* topic, const uint8_t* payload, unsigned int length);