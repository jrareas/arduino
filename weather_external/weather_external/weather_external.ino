
#include <SPI.h>
#include <RH_NRF24.h>
#include <RHReliableDatagram.h>
#include <dht_nonblocking.h>
#include "mySleep.h"

static const int DHT_SENSOR_PIN = 5;
#define DHT_POWER_PIN 6
#define NRF_POWER_PIN 4
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_TYPE_11 );

RH_NRF24 nrf24;

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
RHReliableDatagram manager(nrf24, SERVER_ADDRESS);

float temperature;
float humidity;
mySleep mySleep;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(DHT_POWER_PIN, OUTPUT);
  pinMode(NRF_POWER_PIN, OUTPUT);
  digitalWrite(NRF_POWER_PIN, HIGH);
  delay(200);

  if (!manager.init())
      Serial.println("init failed");
  mySleep.begin();
}
uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];

void sendData() {
  Serial.print("Temp:");
  Serial.println(temperature);
  Serial.print("Hum:");
  Serial.println(humidity);
  char buff[20];
  char t_str[4], h_str[4];
  dtostrf(temperature,2, 0, t_str);
  dtostrf(humidity,2, 0, h_str);
  sprintf(buff, "%s:%s",t_str, h_str);

  if (manager.sendtoWait(buff, sizeof(buff), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      Serial.println("going to sleep");
      delay(200);
      mySleep.goToSleep();
    }
    else
    {
      Serial.println("No reply, is nrf24_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
}

void loop() {
  digitalWrite(DHT_POWER_PIN, HIGH);
  digitalWrite(NRF_POWER_PIN, HIGH);
  if(read100TimesOrReady()){
    sendData();  
  }
}
bool read100TimesOrReady() {
  bool ret = false;
  int i;
  for (i=0; i< 10000; i++) {
    if (measure_environment( &temperature, &humidity ) == true) {
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
