#include "ir/ir.h"

#include <Arduino.h>

#include "config.h"
#include "ir/protocol.h"
#include "network/mqtt.h"
#include "state.h"

namespace {

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

unsigned long waitForLow(unsigned long timeoutUs = IR_TIMEOUT_US) {
  unsigned long start = micros();
  while (digitalRead(PIN_IR_RECEIVE) == HIGH) {
    if (micros() - start > timeoutUs) return 0;
  }
  unsigned long tStart = micros();
  while (digitalRead(PIN_IR_RECEIVE) == LOW) {
    if (micros() - tStart > 10000UL) return 0;
  }
  return micros() - tStart;
}

unsigned long waitForHigh(unsigned long timeoutUs = 5000) {
  unsigned long start = micros();
  while (digitalRead(PIN_IR_RECEIVE) == LOW) {
    if (micros() - start > timeoutUs) return 0;
  }
  unsigned long tStart = micros();
  while (digitalRead(PIN_IR_RECEIVE) == HIGH) {
    if (micros() - tStart > timeoutUs) return timeoutUs;
  }
  return micros() - tStart;
}

uint8_t receiveByte() {
  uint8_t result = 0;
  for (int i = 7; i >= 0; i--) {
    unsigned long burstLen = waitForLow(3000);
    if (burstLen == 0) return 0;
    unsigned long pauseLen = waitForHigh(3000);
    if (pauseLen > IR_BIT_THRESH) {
      result |= (1 << i);
    }
  }
  return result;
}

}  // namespace

void setupIr() {
  ledcSetup(0, 38000, 8);
  ledcAttachPin(PIN_IR_SEND, 0);
  pinMode(PIN_IR_RECEIVE, INPUT_PULLUP);
  Serial.println("[IR] Initialized");
}

void sendShotPacket(uint8_t deviceId) {
  const uint8_t checksum = irChecksum(deviceId);

  burst(IR_START_BURST);
  delayMicroseconds(IR_START_PAUSE);
  sendByte(deviceId);
  sendByte(checksum);
  burst(IR_STOP_BURST);

  Serial.printf("[IR] Shot sent — deviceId:%u\n", deviceId);
}

void irLoop() {
  const unsigned long startBurst = waitForLow();
  if (startBurst < IR_START_MIN) return;

  waitForHigh(5000);

  const uint8_t senderId = receiveByte();
  const uint8_t checksum = receiveByte();

  if (!irValidate(senderId, checksum)) {
    Serial.printf("[IR] Checksum FAIL — senderId:0x%02X checksum:0x%02X\n",
      senderId, checksum);
    return;
  }

  if (senderId == DEVICE_ID) return;
  if (deviceRuntimeState.state != DeviceState::IN_GAME) return;

  Serial.printf("[IR] Hit received — from deviceId:%u\n", senderId);
  publishHit(senderId);
}