
#include <SoftwareSerial.h>

#define LED_PIN 7
int state = 0;

SoftwareSerial mySerial(2, 3); // RX, TX
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  mySerial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  state = mySerial.read();

  if(state == 0) {
    mySerial.println("Led off");
    digitalWrite(LED_PIN, LOW);
    state = 0;
  } else if(state == '1') {
    digitalWrite(LED_PIN, HIGH);
    mySerial.println("Led on");
  } else {
    mySerial.print("State is:");
    mySerial.println(state);
  }

  delay(100);
}
