
#include <HC_SR04.h>


int echoPin = 3;
int trigPin = 2;
int ECHO_INT = 0;
volatile byte state = LOW;
HC_SR04 distanceSensor(trigPin, echoPin, ECHO_INT);

void setup() {
  Serial.begin(9600);
  distanceSensor.begin();
  distanceSensor.start();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(distanceSensor.isFinished()){
    // Do something with the range...
    Serial.print(distanceSensor.getRange());
    Serial.println("cm");
    distanceSensor.start();
  }
  delay(1000);
}

void blink() {
  state = !state;
}

void interrupMe() {
  long duration, distance;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2)/29.1;
  Serial.print("Distance: ");
  Serial.println(distance);  
}
