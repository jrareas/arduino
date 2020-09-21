#include "mySleep.h"
#include <avr/sleep.h>

static uint8_t interruptPin;

void mySleep::begin(uint8_t intPin=2) {
  interruptPin = intPin;
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
}

void static mySleep::wakeUp() {
  Serial.println("Something woke me up");
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(interruptPin));
}

void mySleep::goToSleep() {
  attachInterrupt(digitalPinToInterrupt(interruptPin), mySleep::wakeUp, LOW);
  sleep_enable();
  sleep_cpu();
}
