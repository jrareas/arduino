#include <dht_nonblocking.h>
#include <LowPower.h>
#include <avr/sleep.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int DHT_SENSOR_PIN = 7;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

#define interrupPin 2 // pin to receive the interrupt signal
#define DHT_POWER_PIN 6 // this is to power the device using an Arduino pin. That way, the device will go off when Arduino is sleeping

float temperature;
float humidity;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(DHT_POWER_PIN, OUTPUT);
  pinMode(interrupPin, INPUT_PULLUP);
  
  //digitalWrite(DHT_POWER_PIN,HIGH);
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  Serial.println("All set up");
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
  if(ret == true) {
    Serial.print("Found after ");
    Serial.print(i);
    Serial.println(" attempts");
  }
  return ret;
}

void wakeUp() {
  Serial.println("Something woke me up");
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(interrupPin));
}

void goingToSleep() {
  attachInterrupt(digitalPinToInterrupt(interrupPin), wakeUp, LOW);
  digitalWrite(LED_BUILTIN,LOW);
  sleep_enable();
  sleep_cpu();
  
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
}
void loop() {
  //first power the device
  // wait it to get stable for x seconds
  digitalWrite(DHT_POWER_PIN,HIGH);
  digitalWrite(LED_BUILTIN,HIGH);
  if (read100TimesOrReady() == true) {
    Serial.print("T: ");
    Serial.print(temperature);
    Serial.print(" C");
    Serial.print("   H: "); 
    Serial.print(humidity);
    Serial.println(" %");
    digitalWrite(DHT_POWER_PIN,LOW);
    Serial.println(" going to sleep");
    delay(100);
    //set some time to update stuff in the serial monitor
    goingToSleep();    
  }
}
