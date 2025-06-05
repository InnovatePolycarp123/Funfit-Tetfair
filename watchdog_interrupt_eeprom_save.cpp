#include "watchdog_interrupt_eeprom_save.h"

int eeprom_reset_flag = 0;
volatile uint32_t eeprom_recovery_address = 2;
volatile uint32_t eeprom_reset_flag_address = 0;
volatile void *variable_to_save;
volatile enum VariableType variable_type;

ISR(WDT_vect)
{

    saveBeforeReset(variable_to_save, variable_type);
    digitalWrite(LED_TEST, !digitalRead(LED_TEST));
}

void saveBeforeReset( volatile void* variable_to_save,  volatile enum VariableType variable_type)
{

    switch (variable_type)
    {

    case INT_TYPE:
    {

        int *casted_variable_to_save_pointer_int = (int *)variable_to_save;     // convert void pointer to a pointer of the expected variable type
        int casted_variable_to_save_int = *casted_variable_to_save_pointer_int; //   dereference the pointer of expected variable type and store the value in normal variable

        eeprom_reset_flag = 1; // set the eeprom reset flag

        EEPROM.put(eeprom_recovery_address, casted_variable_to_save_int); // store the variable you want to save in the eeprom;
        EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);         // store the reset flag  in the eeprom;

        break;

    } /*      the same process is implemented for every switch case. the only diffrence is the type the pointer is type casted to saved in*/

    case DOUBLE_TYPE:
    {

        double *casted_variable_to_save_pointer_double = (double *)variable_to_save;
        double casted_variable_to_save_double = *casted_variable_to_save_pointer_double;

        eeprom_reset_flag = 1;

        EEPROM.put(eeprom_recovery_address, casted_variable_to_save_double);
        EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);

        break;
    }

    case UINT32_TYPE:
    {

        uint32_t *casted_variable_to_save_pointer_uint32_t = (uint32_t *)variable_to_save;
        uint32_t casted_variable_to_save_uint32_t = *casted_variable_to_save_pointer_uint32_t;

        eeprom_reset_flag = 1;

        EEPROM.put(eeprom_recovery_address, casted_variable_to_save_uint32_t);
        EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);
        break;
    }

    case UINT16_TYPE:
    {

        uint16_t *casted_variable_to_save_pointer_uint16_t = (uint16_t *)variable_to_save;
        uint16_t casted_variable_to_save_uint16_t = *casted_variable_to_save_pointer_uint16_t;

        eeprom_reset_flag = 1;

        EEPROM.put(eeprom_recovery_address, casted_variable_to_save_uint16_t);
        EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);
        break;
    }

    case UINT8_TYPE:
    {

        uint8_t *casted_variable_to_save_pointer_uint8_t = (uint8_t *)variable_to_save;
        uint8_t casted_variable_to_save_uint8_t = *casted_variable_to_save_pointer_uint8_t;

        eeprom_reset_flag = 1;

        EEPROM.put(eeprom_recovery_address, casted_variable_to_save_uint8_t);
        EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);
        break;
    }

    case FLOAT_TYPE:
    {

        float *casted_variable_to_save_pointer_float = (float *)variable_to_save;
        float casted_variable_to_save_float = *casted_variable_to_save_pointer_float;

        eeprom_reset_flag = 1;

        EEPROM.put(eeprom_recovery_address, casted_variable_to_save_float);
        EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);
        break;
    }
    }
}

void getwatchdogsavedvalue(void *save_buffer, enum VariableType variable_type)
{

    switch (variable_type)
    {

    case INT_TYPE:
    {

        int *casted_variable_to_save_pointer_int = (int *)save_buffer; // convert void pointer to a pointer of the expected variable type

        EEPROM.get(eeprom_reset_flag_address, eeprom_reset_flag);
        int store_temp_recovery_int;

        if (eeprom_reset_flag == 1)
        {

            eeprom_reset_flag = 0;
            EEPROM.get(eeprom_recovery_address, store_temp_recovery_int);
            EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);

            *casted_variable_to_save_pointer_int = store_temp_recovery_int;
        }

        break;
    }

    case DOUBLE_TYPE:
    {

        double *casted_variable_to_save_pointer_double = (double *)save_buffer; // convert void pointer to a pointer of the expected variable type

        EEPROM.get(eeprom_reset_flag_address, eeprom_reset_flag);
        double store_temp_recovery_double;

        if (eeprom_reset_flag == 1)
        {

            eeprom_reset_flag = 0;
            EEPROM.get(eeprom_recovery_address, store_temp_recovery_double);
            EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);

            *casted_variable_to_save_pointer_double = store_temp_recovery_double;
        }

        break;
    }

    case UINT32_TYPE:
    {

        uint32_t *casted_variable_to_save_pointer_uint32_t = (uint32_t *)save_buffer; // convert void pointer to a pointer of the expected variable type

        EEPROM.get(eeprom_reset_flag_address, eeprom_reset_flag);
        uint32_t store_temp_recovery_uint32_t;

        if (eeprom_reset_flag == 1)
        {

            eeprom_reset_flag = 0;
            EEPROM.get(eeprom_recovery_address, store_temp_recovery_uint32_t);
            EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);

            *casted_variable_to_save_pointer_uint32_t = store_temp_recovery_uint32_t;
        }

        break;
    }

    case UINT16_TYPE:
    {

        uint16_t *casted_variable_to_save_pointer_uint16_t = (uint16_t *)save_buffer; // convert void pointer to a pointer of the expected variable type
        EEPROM.get(eeprom_reset_flag_address, eeprom_reset_flag);
        uint16_t store_temp_recovery_uint16_t;

        if (eeprom_reset_flag == 1)
        {

            eeprom_reset_flag = 0;
            EEPROM.get(eeprom_recovery_address, store_temp_recovery_uint16_t);
            EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);

            *casted_variable_to_save_pointer_uint16_t = store_temp_recovery_uint16_t;
        }
        break;
    }

    case UINT8_TYPE:
    {

        uint8_t *casted_variable_to_save_pointer_uint8_t = (uint8_t *)save_buffer; // convert void pointer to a pointer of the expected variable type

        EEPROM.get(eeprom_reset_flag_address, eeprom_reset_flag);
        uint8_t store_temp_recovery_uint8_t;

        if (eeprom_reset_flag == 1)
        {

            eeprom_reset_flag = 0;
            EEPROM.get(eeprom_recovery_address, store_temp_recovery_uint8_t);
            EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);

            *casted_variable_to_save_pointer_uint8_t = store_temp_recovery_uint8_t;
        }

        break;
    }

    case FLOAT_TYPE:
    {

        float *casted_variable_to_save_pointer_float = (float *)save_buffer; // convert void pointer to a pointer of the expected variable type

        EEPROM.get(eeprom_reset_flag_address, eeprom_reset_flag);
        float store_temp_recovery_float;

        if (eeprom_reset_flag == 1)
        {

            eeprom_reset_flag = 0;
            EEPROM.get(eeprom_recovery_address, store_temp_recovery_float);
            EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);

            *casted_variable_to_save_pointer_float = store_temp_recovery_float;
        }
        break;
    }
    }
}

void begin(volatile void *get_variable_to_save, volatile enum VariableType get_variable_type)
{

    variable_to_save = get_variable_to_save;
    variable_type = get_variable_type;
    pinMode(LED_TEST, OUTPUT);
}

void WatchDogTimerWithInterruptSetup(enum watchDogTime TIME)
{

    cli();
    wdt_reset();
    MCUSR &= ~(1 << WDRF);

    WDTCSR |= (1 << WDCE) | (1 << WDE); // set WDCE and WDE to one
    //- this allows you to change watchdog timer prescalar within 4 clock cycles;
    switch (TIME)
    {

    case TIME_16_MILLISECONDS:
        WDTCSR = (1 << WDIE) | (1 << WDE);
        break;

    case TIME_32_MILLISECONDS:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP0);
        break;

    case TIME_64_MILLISECONDS:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP1);
        break;

    case TIME_125_MILLISECONDS:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP0) | (1 << WDP1);
        break;

    case TIME_250_MILLISECONDS:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP2);
        break;

    case TIME_500_MILLISECONDS:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP2) | (1 << WDP0);
        break;

    case TIME_1_SECOND:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP2) | (1 << WDP1);
        break;

    case TIME_2_SECONDS:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP2) | (1 << WDP1) | (1 << WDP0);
        break;

    case TIME_4_SECONDS:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP3);
        break;

    case TIME_8_SECONDS:
        WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP3) | (1 << WDP0);
        break;
    }

    sei();
}

void turnOffWatchDogTimer()
{

    cli();
    wdt_reset();

    MCUSR &= ~(1 << WDRF); // reset the wdrf flag to allow safe turn off of watchdog timer

    WDTCSR |= (1 << WDCE) | (1 << WDE); //  set WDCE and WDE to one
    //- this allows you to change watchdog timer prescalar within 4 clock cycles

    WDTCSR = 0x00; // turnoff watchdog timer

    sei();
}

struct WatchDogInterrupt WatchDogInterruptUser;

void setupWatchdogtimer()
{


    WatchDogInterruptUser.turnOffWatchDogTimer = turnOffWatchDogTimer;

    WatchDogInterruptUser.WatchDogTimerWithInterruptSetup = WatchDogTimerWithInterruptSetup;

    WatchDogInterruptUser.getwatchdogsavedvalue = getwatchdogsavedvalue;

    WatchDogInterruptUser.begin = begin;
}