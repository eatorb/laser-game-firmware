#pragma once

#include <stdint.h>

void setupIr();
void irLoop();
void sendShotPacket(uint8_t deviceId);