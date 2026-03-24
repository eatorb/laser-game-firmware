#pragma once

#include <stdint.h>

constexpr int IR_START_BURST = 3200;
constexpr int IR_START_PAUSE = 1600;
constexpr int IR_BIT_BURST   = 300;
constexpr int IR_BIT_ONE     = 900;
constexpr int IR_BIT_ZERO    = 300;
constexpr int IR_STOP_BURST  = 300;

constexpr int IR_START_MIN  = 2000;
constexpr int IR_BIT_THRESH = 500;
constexpr int IR_TIMEOUT_US = 10000;

inline uint8_t irChecksum(uint8_t deviceId) {
  return ~deviceId;
}

inline bool irValidate(uint8_t deviceId, uint8_t checksum) {
  return (deviceId ^ checksum) == 0xFF;
}