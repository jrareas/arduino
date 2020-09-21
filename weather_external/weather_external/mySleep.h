#include <Arduino.h>

class mySleep
{
public:
  void begin(uint8_t interruptPin=2);
  void goToSleep();
private:
  
  void static wakeUp();
};
