#include "mytft.h"

#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h>

#include "resources.h"
#define BACKGROUND BLACK
//#define MYTFT_SERIAL

MCUFRIEND_kbv tft;

void initializeBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, char label[4]);
void clearArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

uint16_t wid, ht;
void initmytft() {
    uint16_t ID = tft.readID(); //
    if (ID == 0xD3D3) ID = 0x9481; // write-only shield
//    ID = 0x9329;                             // force ID
    tft.begin(ID);
    tft.setRotation(1);
    wid = tft.width();
    ht = tft.height();
    initializeBlock(0, ht/2, wid/2 , ht/2, "Out"); //out
    initializeBlock(wid/2, ht/2, wid/2 , ht/2, "In"); //in
    initializeBlock(0, 0, wid , ht/2, ""); // time
}

void printmsg(uint16_t column, uint16_t row, int textSize, int textColor, const char *msg)
{
  tft.setTextSize(textSize);
  tft.setTextColor(WHITE);
  tft.setCursor(column, row);
  tft.println(msg);
}


//read internal voltage
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

void battery() {   
  double curvolt = double( readVcc() ) / 1000;
  
  if(curvolt >= 4.2) {
    tft.drawBitmap(450, 10, battery_full, 16, 8, WHITE); 
  }
  else if(curvolt >= 3.75)
  {
    tft.drawBitmap(450, 10, battery_half, 16, 8, WHITE); 
  } 
  else {
    tft.drawBitmap(450, 10, battery_empty, 16, 8, WHITE); 
  }
  //if(curvolt <= 4.15 && curvolt > 3.9)
  //{
    //tft.drawBitmap(450, 10, battery_half, 16, 8, WHITE); 
    //return;
  //  whichBat = battery_half;
  //}
  
  //tft.drawBitmap(450, 10, battery_empty, 16, 8, WHITE);
  //if(curvolt <= 3.9 && curvolt > 3.75)
  //{
  //  tft.drawBitmap(450, 10, battery_33, 16, 8, WHITE); 
  //}
}


void timeBlock(String time, String date) {
  static String last_read[2] = {"", ""};
  //static String last_time = "", last_date = "";
  if ((String)time != last_read[0] || (String)date != last_read[1]) {
    clearArea (50,9, 380, 140); // full
    //clearArea (50,9, 380, 30); // date
    //clearArea (50,40, 380, 110); // time
    battery();
    char buff[40];
    date.toCharArray(buff, 40);
    printmsg(80, 10, 3, WHITE ,buff);
    time.toCharArray(buff, 40);
    printmsg(80, 60, 10, WHITE ,buff);
    last_read[0] = (String)time;
    last_read[1] = (String)date;
  }
}

void tempHumIcons(uint16_t x, uint16_t y) {
    tft.drawBitmap(x, y, temperature, 24,51, WHITE);
    tft.drawBitmap(x, y + 80, humidity, 24,38, WHITE);
}

void inBlock(String temp, String hum) {
  static String last_read[2] = {"", ""};
  if ((String)temp != last_read[0] || (String)hum != last_read[1]) {
    #ifdef MYTFT_SERIAL
    Serial.print("Sent T: ");
    Serial.print(temp);
    Serial.print(" H: ");
    Serial.println(hum);
    #endif
    clearArea (wid/2 + 60,ht/2 + 10, 120, 140);

    char buff[3];
    temp.toCharArray(buff, 3);
    printmsg((wid/2) + 80, 10 + (ht/2) + 10, 7, WHITE, buff);
    hum.toCharArray(buff, 3);
    printmsg((wid/2) + 80, 10 + (ht/2) + 80, 7, WHITE, buff);
  
    //tempHumIcons(440, 20 + (ht/2));
    last_read[0] = (String)temp;
    last_read[1] = (String)hum;
  }
}

void outBlock(String temp, String hum)  {
  static String last_read[2] = {"", ""};
  if ((String)temp != last_read[0] || (String)hum != last_read[1]) {
    //printmsg(10, 10 + (ht/2), 2, RED, "Out");
    
    //tempHumIcons(200, 20 + (ht/2));
    clearArea (60,ht/2 + 10, 120, 140);
    
    char buff[3];
    temp.toCharArray(buff, 3);
    printmsg(80, 10 + (ht/2) + 10, 7, YELLOW, buff);
    hum.toCharArray(buff, 3);
    printmsg(80, 10 + (ht/2) + 80, 7, YELLOW, buff);
    last_read[0] = (String)temp;
    last_read[1] = (String)hum;
  }
}

void initializeBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, char label[4]) {
  clearArea(x, y, w , h);
  tft.drawRect(x, y, w , h, YELLOW); 
  tempHumIcons(x + 200, 20 + y);
  if (strlen(label) !=0) {
    tft.drawRect(x, y, 50 , 35, YELLOW);
    printmsg(x + 10, y + 10 , 2, RED, label);
  }
}

void clearArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  tft.fillRect(x, y, w , h, BACKGROUND);
}
