#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_NRF24 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

int enA = 9;
int en1 = 2;
int en2 = 4;

int enB = 5;
int en3 = 7;
int en4 = 6;

int avoidance_front = 0;
int avoidance_rear = 1;

int echoPin = 3;
int trigPin = 1;

// need to set it to false when go live because we are using serial pin and may impact readings
bool debug = false;

bool allowForward = true;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);
  pinMode(en3, OUTPUT);
  pinMode(en4, OUTPUT);
  pinMode(avoidance_front, INPUT);
  pinMode(avoidance_rear, INPUT);

  if (debug) {
    Serial.begin(9600);  
  }
  
  if (!manager.init()){
    if (debug) {
      Serial.println("init failed");  
    }
    
  } else {
    if (debug) {
      Serial.println("init Success");
    }
  }
}
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

void loop() {
  
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Move(buf);
    }
  }
  //manageAvoidance();
  delay(10);
}

void Move(uint8_t buf[2]) {
  if (debug) {
      Serial.print("X: ");
      Serial.println(buf[0]);

      Serial.print("Y: ");
      Serial.println(buf[1]);
    
  }
  int wheelSpeed = 0;
  if (buf[0] != 129) { // give a bit to make sure it considers the gap when it is not using
    // just move forward or reverse
    if ( buf[0] >= 129 ) {
      wheelSpeed = map(buf[0], 132, 255, 0, 255);
      MoveForward('Z', wheelSpeed);
    } else {
      wheelSpeed = map(buf[0], 132, 0, 0, 255);
      Reverse('Z', wheelSpeed);
    }
  } else {
    Stop('Z');
  }

  if (buf[1] != 129){
    turn(buf[1]);
  }
}

void turn(uint8_t buf) {
    int wheelSpeed = 0;
    
    if ( buf >= 129 ) { // right
      wheelSpeed = map(buf, 129, 255, 0, 255);
      analogWrite(enA,wheelSpeed);
      digitalWrite(en1, HIGH);
      digitalWrite(en2, LOW);

    } else { // left
      wheelSpeed = map(buf, 129, 0, 0, 255);
      digitalWrite(en3, HIGH);
      digitalWrite(en4, LOW);
      analogWrite(enB,wheelSpeed);
    }
}

void MoveForward(char side, int wheelSpeed) {
  if (!allowForward) {
    wheelSpeed = 0;
  }
  switch(side) {
    case 'A':
      analogWrite(enA,wheelSpeed);
      digitalWrite(en1, HIGH);
      digitalWrite(en2, LOW);
      break;
    case 'B':
      digitalWrite(en3, HIGH);
      digitalWrite(en4, LOW);
      analogWrite(enB,wheelSpeed);
      break;
    default:
    analogWrite(enA,wheelSpeed);
    analogWrite(enB,wheelSpeed);
    digitalWrite(en1, HIGH);
    digitalWrite(en2, LOW);
    digitalWrite(en3, HIGH);
    digitalWrite(en4, LOW);
  }
}

void Wheel(bool reverse , char side, int wheelSpeed) {
  if(debug) {
    Serial.println(wheelSpeed);
  }
  
  if (reverse){
    Reverse(side, wheelSpeed);
  } else {
    MoveForward(side, wheelSpeed);
  }
}

void Stop(char side) {
  switch (side){
    case 'A':
      analogWrite(enA,0);
      break;
    case 'B':
      analogWrite(enB,0);
      break;
    default:
      analogWrite(enA,0);
      analogWrite(enB,0);
  }
}

void Reverse(char side, int wheelSpeed) {

  if(side == 'A') {
    analogWrite(enA,wheelSpeed);
  } else {
    analogWrite(enB,wheelSpeed);
  }
  switch(side) {
    case 'A':
        analogWrite(enA,wheelSpeed);
      digitalWrite(en1, LOW);
      digitalWrite(en2, HIGH);
      break;
    case 'B':
      analogWrite(enB,wheelSpeed);
      digitalWrite(en3, LOW);
      digitalWrite(en4, HIGH);
      break;
    default:
      analogWrite(enA,wheelSpeed);
      analogWrite(enB,wheelSpeed);
      digitalWrite(en1, LOW);
      digitalWrite(en2, HIGH);
      digitalWrite(en3, LOW);
      digitalWrite(en4, HIGH);
  }
}

void flip(int wheelSpeed) {
  Wheel(true,'A', wheelSpeed);
  Wheel(false,'B', wheelSpeed);
}


void demoMode() {
  Wheel(false, 'Z', 255);
  delay(1000);
  Wheel(true, 'Z', 255);
  delay(1000);
  flip(255);
  delay(1000);  
}

void manageAvoidance() {
  long duration, distance;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2)/29.1;
  if (debug) {
    Serial.print("Distance: ");
    Serial.println(distance);  
  }
  if(distance > 25) {
    allowForward = false;
  }
  
  //if(distance > 25) {
  //  Wheel(true, 'Z', 255); // forward
  //} else {
  //  flip(255);
  //  delay(1000);
  //  Wheel(true, 'Z', 255); // forward
  //}
}
