#pragma once

#include <stdint.h>

void handleTrigger(uint32_t nowMs);
void handleIrReceive();
void processRespawn(uint32_t nowMs);
