#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rf_driver;

void setup() {
  // put your setup code here, to run once:
  rf_driver.init();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  const char *msg = "Welcome";
  if (rf_driver.send((uint8_t *)msg, strlen(msg))) {
    rf_driver.waitPacketSent();
    Serial.println("message sent");    
  } else {
    Serial.println("Failed to broadcast");    
  }
  delay(1000);
}
