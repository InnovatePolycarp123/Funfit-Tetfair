#ifndef watchdog_interrupt_eeprom_save_h
#define watchdog_interrupt_eeprom_save_h

#include <Arduino.h>
#include <avr/wdt.h>
#include <EEPROM.h>

#define LED_TEST 4
extern int eeprom_reset_flag;
extern volatile uint32_t eeprom_recovery_address;
extern volatile uint32_t eeprom_reset_flag_address;
extern volatile void *variable_to_save;



enum watchDogTime
{

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

};



extern struct WatchDogInterrupt WatchDogInterruptUser;


enum VariableType
{

    UINT32_TYPE,
    UINT8_TYPE,
    UINT16_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    INT_TYPE,
};

extern volatile enum VariableType variable_type;



struct WatchDogInterrupt{
 void (*turnOffWatchDogTimer)(void);
 void (*WatchDogTimerWithInterruptSetup)(enum watchDogTime TIME);
 void (*begin)(volatile void* save_buffer, volatile enum VariableType  variable_type);;
 void (*getwatchdogsavedvalue)(void*save_buffer, enum VariableType  variable_type);
};






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void turnOffWatchDogTimer();
void WatchDogTimerWithInterruptSetup(enum watchDogTime TIME);
void saveBeforeReset( volatile void* variable_to_save, enum VariableType   variable_type);
void getwatchdogsavedvalue(void* save_buffer, enum VariableType  variable_type);
void begin(volatile void* get_variable_to_save, volatile enum VariableType  get_variable_type);
void setupWatchdogtimer();
////////////////////////////////////////////////////////////////////////////////////////////////////////////







#endif