// All the mcufriend.com UNO shields have the same pinout.
// i.e. control pins A0-A4.  Data D2-D9.  microSD D10-D13.
// Touchscreens are normally A1, A2, D7, D6 but the order varies
//
// This demo should work with most Adafruit TFT libraries
// If you are not using a shield,  use a full Adafruit constructor()
// e.g. Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#include "mytft.h"
#include <dht_nonblocking.h>
#include <Wire.h>
#include <TimeLib.h>
#define CLOCK
#ifdef CLOCK
#include <DS3232RTC.h>
#endif

#include <SPI.h>
#include <RH_NRF24.h>
#include <RHReliableDatagram.h>


#define DHT_SENSOR_TYPE DHT_TYPE_11
//#define MYTFT_SERIAL
#define RADIO
#define READIN

static const uint8_t DHT_SENSOR_PIN = 0;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

float temp;
float hum;
String str_out;
String str_humid;
String str_temp;

const unsigned char *weekDayName[7] = {
  "Sun","Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
#ifdef RADIO
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

RH_NRF24 driver;
RHReliableDatagram manager(driver, SERVER_ADDRESS);
#endif

#ifdef CLOCK
DS3232RTC  rtc(false);
#endif

void setup(void) {
  #ifdef MYTFT_SERIAL
    Serial.begin(9600);
  #endif
    Wire.begin();
    #ifdef CLOCK
    rtc.begin();
    #endif
  #ifdef RADIO
    if (!manager.init())
    #ifdef MYTFT_SERIAL
      Serial.println("init failed");
    #else
      ;
    #endif
  #endif
    initmytft();
    inBlock("?", "?");
    outBlock("?", "?");
}

void loop(void) {
  char buff[2];
  #ifdef READIN
  if (read100TimesOrReady() == true) {
    String tempstr = dtostrf(temp, 2, 0, buff);
    String humstr = dtostrf(hum, 2, 0, buff);
    #ifdef MYTFT_SERIAL
    Serial.print("T: ");
    Serial.print(temp);
    Serial.print(" C");
    Serial.print("   H: "); 
    Serial.print(hum);
    Serial.println(" %");
    Serial.println(" going to sleep");
    #endif
    inBlock(tempstr, humstr);
  }
  #endif
  #ifdef RADIO
  uint8_t data[] = "ty";
  uint8_t buf[5];
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      #ifdef MYTFT_SERIAL
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      #endif
      str_out = String((char*)buf);
      
      // Split string into two values
      for (int i = 0; i < str_out.length(); i++) {
        if (str_out.substring(i, i+1) == ":") {
          str_temp = str_out.substring(0, i);
          str_humid = str_out.substring(i+1);
          break;
        }
      }
       #ifdef MYTFT_SERIAL
      Serial.print("Out Temp: ");
      Serial.print(str_temp);
      Serial.print("  Out Hum: ");
      Serial.println(str_humid);
      #endif
      if (!manager.sendtoWait(data, sizeof(data), from))
       #ifdef MYTFT_SERIAL
        Serial.println("sendtoWait failed");
       #else
        ;
       #endif

      outBlock(str_temp, str_humid);
    }
  }
  #endif
  #ifdef CLOCK
  timeBlock(getTimeDevice(), getDate());
  #endif
}

String getTimeDevice() {
  tmElements_t tm;
  char buff[20];
  #ifdef CLOCK
  RTC.read(tm);
  sprintf(buff,"%.2d:%.2d",tm.Hour,tm.Minute);
  #endif
  return String(buff);
}

String getDate() {
  char buff[50];
  #ifdef CLOCK
  time_t t = RTC.get();
  sprintf(buff, "%s, %.2d %s, %d ",
        weekDayName[weekday(t) - 1], day(t), monthShortStr(month(t)), year(t));

  #endif
  return String(buff);
}

bool read100TimesOrReady() {
  bool ret = false;
  for (int i=0; i< 10000; i++) {
    if (measure_environment( &temp, &hum ) == true) {
      ret = true;
      break; 
    } else {
      i++;
    }
  }
  return ret;
}

/*
 * Poll for a measurement, keeping the state machine alive.  Returns
 * true if a measurement is available.
 */
static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}
