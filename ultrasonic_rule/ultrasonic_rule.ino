int ECHO=2;
int TRIGGER=3;
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int switchModePin = 5;
int switchReadPin = 6;
int trigPin = 2;
int echoPin = 3;
int currentMode = 1; // CM

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  } else {
    Serial.println(F("SSD1306 allocation success"));
  }
  initDisplay();
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  writeDisplay("Wait...", 0, 22);

  pinMode(switchModePin, INPUT);
  pinMode(switchReadPin, INPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  
  initDisplay();
  getCurrentMode();
}

void initDisplay() {
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
}

void writeDisplay(String text, int position_x, int position_y) {
  display.setCursor(position_x, position_y);
  display.println(text);
  display.display();

}
void switchMode() {
  if (currentMode == 1) {
    currentMode = 2;
  } else {
    currentMode = 1;
  }
  Serial.print("New mode:");
  Serial.println(currentMode);
  getCurrentMode();
}

void getCurrentMode() {
  String currentModeStr = "in";
  if (currentMode == 1) {
    currentModeStr = "cm";
  }
  initDisplay();
  display.setTextSize(1);
  writeDisplay(currentModeStr, 110, 3);
  display.setTextSize(2);
  writeDisplay("Ready...", 0, 22);
  display.display();
}
void loop() {
  int readMode = digitalRead(switchModePin);
  if (readMode == HIGH && millis() - time > debounce) {
    Serial.println("Read button");
    switchMode();
    
    time = millis();
  }
  // put your main code here, to run repeatedly:
  delay(10);
}
