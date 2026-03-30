#pragma once

#include <stdint.h>

void handleMqttCommand(const char* topic, const uint8_t* payload, unsigned int length);