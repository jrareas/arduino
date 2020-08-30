#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rf_driver;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  if (!rf_driver.init()) {
    Serial.print("Fail to init");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t buf[7];
  uint8_t buflen = sizeof(buf);
  if (rf_driver.recv(buf, &buflen)) {
    Serial.print("Roger:");
    Serial.println((char *)buf);
  }
}
