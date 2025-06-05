#include <Arduino.h>
#include "watchdog_interrupt_eeprom_save.h"
/*

this library allows you to use to store a value in eeprom after a wdt reset. only after a wdt reset. a normal reset will cause a normal reset

if this library is running  you can't use a delay that's more than the maximum wait_time. you can only use non-blocking delay

The possible wait times before before a watch dog timer reset are:

 TIME_16_MILLISECONDS,
  TIME_32_MILLISECONDS,
  TIME_64_MILLISECONDS,
  TIME_125_MILLISECONDS,
  TIME_250_MILLISECONDS,
  TIME_500_MILLISECONDS,
  TIME_1_SECOND,
  TIME_2_SECONDS,
  TIME_4_SECONDS,
  TIME_8_SECONDS

  On another hand, the possible variable types you can save are only numbers:

   UINT32_TYPE,
    UINT8_TYPE,
    UINT16_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    INT_TYPE,

*/
uint32_t increasing_value = 0;
uint32_t reset_value = 100;

void setup()
{

  Serial.begin(115200);
  setupWatchdogtimer();

  Serial.println("there was a reset");

  WatchDogInterruptUser.WatchDogTimerWithInterruptSetup(TIME_8_SECONDS);
  WatchDogInterruptUser.begin((void *)&increasing_value, UINT32_TYPE);                 // put a pointer to the value you want to save after a watchdog interrupt
  WatchDogInterruptUser.getwatchdogsavedvalue((void *)&increasing_value, UINT32_TYPE); // get the value back if there's an interrupt and reset caused by wdt.

  Serial.println("increasing_value: " + String(increasing_value));

  if(increasing_value == reset_value){

    reset_value += 100;// this is to help avoid a continous reset after the first wdt reset. that's we always increase the reset value after a reset has taken place. 
  }
  
}

void loop()
{

  static uint32_t previous_millis = 0;

 
  wdt_reset(); // feed the wdt. in other words, reset the wdt timer counter if it overflows and causes a system reset and interrupt

 
   uint32_t  current_millis = millis();

  if (current_millis  - previous_millis >= 1000)
  {
     Serial.println(increasing_value);
    increasing_value++;
    previous_millis = current_millis ;

    
  }

  if (increasing_value == reset_value)
  {

    while (1)  // cause a wdt_reset reset;
    {
    }
  }
}
