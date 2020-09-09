//www.elegoo.com
//2018.10.25

#include <LiquidCrystal.h>
#include <LowPower.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS3232RTC.h>
#include <dht_nonblocking.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11
#define OLED_RESET 4

#include <Fonts/FreeSerif9pt7b.h>

#define NUMFLAKES     10

#define LOGO_HEIGHT   4
#define LOGO_WIDTH    4

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

Adafruit_SSD1306 display(OLED_RESET);

static const int DHT_SENSOR_PIN = 7;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );


const unsigned char battery_full[] PROGMEM = {
  B11111111,B11111110,
  B11111111,B11111110,
  B11111111,B11111111,
  B11111111,B11111111,
  B11111111,B11111111,
  B11111111,B11111111,
  B11111111,B11111110,
  B11111111,B11111110
};


const unsigned char battery_empty[] PROGMEM = {
  B11111111,B11111110,
  B10000000,B00000010,
  B10000000,B00000011,
  B10000000,B00000011,
  B10000000,B00000011,
  B10000000,B00000011,
  B10000000,B00000010,
  B11111111,B11111110
};


const unsigned char battery_33[] PROGMEM = {
  B11111111,B11111110,
  B11110000,B00000010,
  B11110000,B00000011,
  B11110000,B00000011,
  B11110000,B00000011,
  B11110000,B00000011,
  B11110000,B00000010,
  B11111111,B11111110
};



const unsigned char battery_half[] PROGMEM = {
  B11111111,B11111110,
  B11111110,B00000010,
  B11111110,B00000011,
  B11111110,B00000011,
  B11111110,B00000011,
  B11111110,B00000011,
  B11111110,B00000010,
  B11111111,B11111110
};



const unsigned char battery_75[] PROGMEM = {
  B11111111,B11111110,
  B11111111,B11100010,
  B11111111,B11100011,
  B11111111,B11100011,
  B11111111,B11100011,
  B11111111,B11100011,
  B11111111,B11100010,
  B11111111,B11111110
};


const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const char *weekDayName[7] = {
  "Sun","Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

DS3232RTC  rtc(false);


bool setuptime = false;

void setTimeDevice() {
  if (setuptime) {
    rtc.set(now());
  }
}
/*
 * Initialize the serial port.
 */
void setup( )
{
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  while (!Serial) ; // wait for Arduino Serial Monitor
  
  setTimeDevice();
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

void initDisplay() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  batterylevel();
  writeDateToDisplay();
  writeTimeToDisplay();
}

void writeTimeToDisplay() {
  tmElements_t tm;
  RTC.read(tm);
  display.setCursor(0,0);
  printDigits(tm.Hour);
  display.print(":");
  printDigits(tm.Minute);
  // take the seconds out until i figure out the delay issue
  //display.print(":");
  //printDigits(t.sec);
}

void writeDateToDisplay() {
  char buff[40];
  time_t t = RTC.get();

  sprintf(buff, "%s, %.2d %s, %d ",
        weekDayName[weekday(t) - 1], day(t), monthShortStr(month(t)), year(t));
  display.setCursor(0,12);
  display.print(buff);
}

void printDigits(int digits) {
  
 if (digits < 10) {
  display.print('0'); 
 }
 
 display.print(digits);
}

void goingToSleep() {
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
}
/*
 * Main program loop.
 */
void loop( )
{
  float temperature;
  float humidity;
  
  /* Measure temperature and humidity.  If the functions returns
     true, then a measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true )
  {
    initDisplay();
    display.setCursor(0, 25);
    display.print("T: ");
    display.print(temperature);
    display.print(" C");
    display.setCursor(66,25);
    display.print("H: "); 
    display.print(humidity);
    display.print(" %");
    display.display();
    goingToSleep();
  }
}

byte batterylevel()
{

  int POS = 112;
  int W = 16;
  int H = 8;
  //read the voltage and convert it to volt
  double curvolt = double( readVcc() ) / 1000;
  // check if voltge is bigger than 4.2 volt so this is a power source
  if(curvolt > 4.6)
  {
    display.drawBitmap(POS,0,battery_full, W, H, WHITE);
  }
  if(curvolt <= 4.6 && curvolt > 4.2)
  {
    display.drawBitmap(POS,0,battery_75, W, H, WHITE);
  }
  if(curvolt <= 4.2 && curvolt > 3.8)
  {
    display.drawBitmap(POS,0,battery_half, W, H, WHITE);
  }
  if(curvolt <= 3.8 && curvolt > 3.0)
  {
    display.drawBitmap(POS,0,battery_33, W, H, WHITE);
  }
  if(curvolt <= 3.0)
  {
    display.drawBitmap(POS,0,battery_empty, W, H, WHITE);
  }
  display.setCursor(50, 0);
  display.print(curvolt);
  display.setCursor(78, 0);
  display.print("V");
    
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

void drawBattery(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }
}
