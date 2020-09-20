#include "mytft.h"

#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h>

#include "resources.h"
#define BACKGROUND BLACK
//#define MYTFT_SERIAL

MCUFRIEND_kbv tft;
uint16_t wid, ht;
void initmytft() {
    uint16_t ID = tft.readID(); //
    if (ID == 0xD3D3) ID = 0x9481; // write-only shield
//    ID = 0x9329;                             // force ID
    tft.begin(ID);
    tft.setRotation(1);
    wid = tft.width();
    ht = tft.height();
}

void printmsg(uint16_t column, uint16_t row, int textSize, int textColor, const char *msg)
{
  tft.setTextSize(textSize);
  tft.setTextColor(RED);
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
  
  if(curvolt > 3.75)
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

void initBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    tft.fillRect(x, y, w , h, BACKGROUND);
    tft.drawRect(x, y, w , h, YELLOW);  
}

void timeBlock(String time, String date) {
  static String last_read[2] = {"", ""};
  //static String last_time = "", last_date = "";
  if ((String)time != last_read[0] || (String)date != last_read[1]) {
    initBlock(0, 0, wid , ht/2);
    battery();
    char buff[40];
    date.toCharArray(buff, 40);
    printmsg(80, 10, 3, RED ,buff);
    time.toCharArray(buff, 40);
    printmsg(80, 60, 10, RED ,buff);
    last_read[0] = (String)time;
    last_read[1] = (String)date;
  }
}

void drawTempHumidBlock(uint8_t x, uint8_t y, uint8_t startx, uint8_t starty ) {
    tft.fillRect(x, y, startx , starty, BACKGROUND);
    tft.drawRect(x, y, startx , starty, YELLOW);
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
    initBlock(wid/2, ht/2, wid/2 , ht/2);
    tft.drawRect(wid/2, ht/2, 50 , 35, YELLOW);
    
    printmsg((wid/2) + 10, 10 + (ht/2), 2, RED, "In");
    char buff[3];
    temp.toCharArray(buff, 3);
    printmsg((wid/2) + 80, 10 + (ht/2) + 10, 7, RED, buff);
    hum.toCharArray(buff, 3);
    printmsg((wid/2) + 80, 10 + (ht/2) + 80, 7, RED, buff);
  
    tempHumIcons(440, 20 + (ht/2));
    last_read[0] = (String)temp;
    last_read[1] = (String)hum;
  }
}

void outBlock(String temp, String hum)  {
  static String last_read[2] = {"", ""};
  //static String last_hum_out = "", last_temp_out = "";
  if ((String)temp != last_read[0] || (String)hum != last_read[1]) {
    initBlock(0, ht/2, wid/2 , ht/2);
    printmsg(10, 10 + (ht/2), 2, RED, "Out");
    tft.drawRect(0, ht/2, 50 , 35, YELLOW);
    tempHumIcons(200, 20 + (ht/2));
  
    char buff[3];
    temp.toCharArray(buff, 3);
    printmsg(80, 10 + (ht/2) + 10, 7, YELLOW, buff);
    hum.toCharArray(buff, 3);
    printmsg(80, 10 + (ht/2) + 80, 7, YELLOW, buff);
    last_read[0] = (String)temp;
    last_read[1] = (String)hum;
  }
}
