// Choose your I2C implementation before including Tiny4kOLED.h
// The default is selected is Wire.h

// To use the Wire library:
//#include <Wire.h>

// To use the Adafruit's TinyWireM library:
#include <TinyWireM.h>

// To use the TinyI2C library from https://github.com/technoblogy/tiny-i2c
//#include <TinyI2CMaster.h>

// The blue OLED screen requires a long initialization on power on.
// The code to wait for it to be ready uses 20 bytes of program storage space
// If you are using a white OLED, this can be reclaimed by uncommenting
// the following line (before including Tiny4kOLED.h):
//#define TINY4KOLED_QUICK_BEGIN

#include <Tiny4kOLED.h>
#include "font11x16.h"
#include "ModernDos.h"

#define OLED_X 128
#define OLED_Y 64

int currentMode = 1; // CM
int switchModePin = 3;
long timeTakenButton = 0;
long timeSiwtchModeButton = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers
long duration;
long distance;
int trigPin = 4;
int echoPin = 1;
long previousDistance=0;

void setup() {
  pinMode(switchModePin, INPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);
  // Send the initialization sequence to the oled. This leaves the display turned off
  oled.begin(OLED_X, OLED_Y, sizeof(tiny4koled_init_128x64r), tiny4koled_init_128x64r);

  // Two rotations are supported,
  // The begin() method sets the rotation to 1.
  //oled.setRotation(0);

  // Some newer devices do not contain an external current reference.
  // Older devices may also support using the internal curret reference,
  // which provides more consistent brightness across devices.
  // The internal current reference can be configured as either low current, or high current.
  // Using true as the parameter value choses the high current internal current reference,
  // resulting in a brighter display, and a more effective contrast setting.
  //oled.setInternalIref(true);

  // Clear the memory before turning on the display
  oled.clear();

  // Turn on the display
  oled.on();

  // Switch the half of RAM that we are writing to, to be the half that is non currently displayed
  //oled.switchRenderFrame();
  oled.setFont(FONT8X16MDOS);
  writetDisplay("Wait...", 40, 4);
  
  oled.clear();
  getCurrentMode();
}

void getCurrentMode() {
  String currentModeStr = "Imperial";
  if (currentMode == 1) {
    currentModeStr = "  Metric";
  }

  oled.setFont(FONT8X16MDOS); 
  oled.setCursor(60, 0);
  oled.print(currentModeStr);
}
void loop() {
  int readMode = digitalRead(switchModePin);
  if (readMode == HIGH && millis() - timeSiwtchModeButton > debounce) {
    switchMode();
    previousDistance = 0; //reset measure to force recalculate it
    timeSiwtchModeButton = millis();
  }

  takeMeasurement();
  delay(500);
}

void switchMode() {
  if (currentMode == 1) {
    currentMode = 2;
  } else {
    currentMode = 1;
  }
  getCurrentMode();
}

void displayImperial(long distance) {
  String toPrint = String("IN: ");
  toPrint.concat((String)distance);
  writetDisplay(toPrint, 2, 2);
  toPrint = String("FT: ");
  toPrint.concat((String)(distance/12));
  writetDisplay(toPrint, 2, 5);
  getCurrentMode();
}

void displaMetric(long distance) {
  String toPrint = String("CM: ");
  toPrint.concat((String)distance);
  writetDisplay(toPrint, 2, 2);
  toPrint = String("M: ");
  toPrint.concat((String)(distance/100));
  writetDisplay(toPrint, 2, 5);
  getCurrentMode();
}

void takeMeasurement() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2.00; 
  if (previousDistance != distance) {
    previousDistance = distance;
    if (currentMode != 1) {
      distance = distance * 0.393701;
      displayImperial(distance);
    } else {
      displaMetric(distance);
    }
  } else {
      digitalWrite(trigPin, HIGH);
  }
}

void writetDisplay(String text, int X, int Y) {
  //text.concat("      ");
  // Position the text cursor
  // In order to keep the library size small, text can only be positioned
  // with the top of the font aligned with one of the four 8 bit high RAM pages.
  // The Y value therefore can only have the value 0, 1, 2, or 3.
  // usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  oled.setCursor(X, Y);
  
  // Write text to oled RAM (which is not currently being displayed).
  oled.print(text);

  // Write the number of milliseconds since power on.

  // Swap which half of RAM is being written to, and which half is being displayed.
  // This is equivalent to calling both switchRenderFrame and switchDisplayFrame.
  //oled.switchFrame();
}
