#define EEPROM_BYTES 1024 // change to 4096 for Mega

#include <EEPROM.h>

void setup()
{
  for (int i = 0; i < EEPROM_BYTES; i++)
    EEPROM.write(i, 0xFF);
    
  // turn the LED on when we're done
  digitalWrite(13, HIGH);
}

void loop()
{
}
