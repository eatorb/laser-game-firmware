#include "ir/ir.h"

#include <Arduino.h>

#include "config.h"
#include "ir/protocol.h"
#include "network/mqtt.h"
#include "state.h"

namespace {

// ── TX ───────────────────────────────────────

void burst(int us) {
  ledcWrite(0, 128);
  delayMicroseconds(us);
  ledcWrite(0, 0);
}

void sendBit(bool bitVal) {
  burst(IR_BIT_BURST);
  delayMicroseconds(bitVal ? IR_BIT_ONE : IR_BIT_ZERO);
}

void sendByte(uint8_t b) {
  for (int i = 7; i >= 0; i--) {
    sendBit((b >> i) & 1);
  }
}

// ── RX — interrupt driven ────────────────────

constexpr int MAX_TRANS = 80;

volatile uint32_t capTimes[MAX_TRANS];
volatile uint8_t  capLevels[MAX_TRANS];
volatile int      capCount    = 0;
volatile uint32_t lastTransUs = 0;

void IRAM_ATTR onRxChange() {
  uint32_t now = micros();
  if (now - lastTransUs > 5000) capCount = 0;
  lastTransUs = now;
  if (capCount < MAX_TRANS) {
    capTimes[capCount]  = now;
    capLevels[capCount] = digitalRead(PIN_IR_RECEIVE);
    capCount++;
  }
}

bool decodePacket(uint8_t& outDeviceId) {
  int n = capCount;
  if (n < 34) return false;

  uint32_t t[MAX_TRANS];
  uint8_t  l[MAX_TRANS];

  noInterrupts();
  for (int i = 0; i < n; i++) {
    t[i] = capTimes[i];
    l[i] = capLevels[i];
  }
  capCount = 0;
  interrupts();

  if (l[0] != LOW) return false;

  uint32_t hBurst = t[1] - t[0];
  uint32_t hPause = t[2] - t[1];
  if (hBurst < IR_START_MIN || hPause < 1000) return false;

  uint16_t raw = 0;
  int idx = 3;
  for (int i = 15; i >= 0; i--) {
    if (idx + 1 >= n) return false;
    uint32_t pause = t[idx + 1] - t[idx];
    if (pause > IR_BIT_THRESH) raw |= (1 << i);
    idx += 2;
  }

  uint8_t deviceId = (raw >> 8) & 0xFF;
  uint8_t checksum = raw & 0xFF;
  if (!irValidate(deviceId, checksum)) return false;

  outDeviceId = deviceId;
  return true;
}

}  // namespace

void setupIr() {
  ledcSetup(0, 38000, 8);
  ledcAttachPin(PIN_IR_SEND, 0);
  pinMode(PIN_IR_RECEIVE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_IR_RECEIVE), onRxChange, CHANGE);
  Serial.println("[IR] Initialized");
}

void sendShotPacket(uint8_t deviceId) {
  const uint8_t checksum = irChecksum(deviceId);

  // Odpoj interrupt počas TX — vlastný signál by rušil receiver
  detachInterrupt(digitalPinToInterrupt(PIN_IR_RECEIVE));

  burst(IR_START_BURST);
  delayMicroseconds(IR_START_PAUSE);
  sendByte(deviceId);
  sendByte(checksum);
  burst(IR_STOP_BURST);

  delayMicroseconds(5000);  // nechaj signál usadiť
  capCount = 0;             // zahoď čo sa zachytilo počas TX

  attachInterrupt(digitalPinToInterrupt(PIN_IR_RECEIVE), onRxChange, CHANGE);

  Serial.printf("[IR] Shot sent — deviceId:%u\n", deviceId);
}

void irLoop() {
  // Čakaj kým máme dosť tranzícií a 5ms ticho (packet skončil)
  if (capCount < 34) return;
  if (micros() - lastTransUs < 5000) return;

  uint8_t senderId = 0;
  if (!decodePacket(senderId)) return;

  if (senderId == DEVICE_ID) return;
  if (deviceRuntimeState.state != DeviceState::IN_GAME) return;

  Serial.printf("[IR] Hit received — from deviceId:%u\n", senderId);
  publishHit(senderId);
}