#include "freq_measurement_count_method.h"

volatile uint32_t Timer_one_overflow_count;
volatile uint32_t overflow_count_for_frequency_calculation; // a copy of Timer_one_overflow_count to avoid racing condition
volatile double count_period;                               // a global functio for calculating count period
volatile uint32_t milliSecondsCount;
volatile uint32_t frequency; // frequency in this case represents CPS (counts per seconds). By count we mean number of pulses per second.
volatile bool freq_counting_finished = false;
bool counting_started = false;
volatile uint32_t  count_time = 0; // the variable used to measure the duration of all continuos total ferquency count resets some minutes after frequency equals zero.
uint32_t previous_millis_count_time_reset_duration = 0;

void startCount(double set_count_period_in_ms)
{

  count_period = set_count_period_in_ms;
  Timer_one_overflow_count = 0;
  milliSecondsCount = 0;
  freq_counting_finished = false;

  cli();
  TCCR1B = 0;
  TCCR1A = 0;
  TCNT1 = 0;
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  ////////////////////// RESET ALL REGISTERS

  TCCR1B = 0b00000111; // set timer one for external clock on rising edge
  TIMSK1 = 0b00000001; // enable overflow interrupt on timer one
  TCCR2A = 0b00000010; // enable clear timer on compare wave generation (CTC) mode on timer 2
  TCCR2B = 0b00000101; // set timer two prescalar to 128: this sets timer frequency to 125,000
  TIMSK2 = 0b00000010; // enable timer 2 output compare interrupt A
  OCR2A = 124;
  sei();

  ///////////// RECONFIGURE REGISTERS
}

ISR(TIMER1_OVF_vect)
{

  Timer_one_overflow_count++;
}

ISR(TIMER2_COMPA_vect)
{

  milliSecondsCount++;
  overflow_count_for_frequency_calculation = Timer_one_overflow_count;

  if (milliSecondsCount > count_period)
  {

    uint32_t number_of_pulses_within_count_period = (overflow_count_for_frequency_calculation * 65536) + TCNT1;

    double count_period_in_seconds = count_period / 1000.0; // convert count_period variable to seconds

    double frequency_with_decimal = (double) number_of_pulses_within_count_period/ count_period_in_seconds;

    frequency = (uint32_t)frequency_with_decimal;

    if (frequency > 0)
    {
      count_time += count_period;
    }

    TCNT1 = 0;
    freq_counting_finished = true;
    Timer_one_overflow_count = 0;
    milliSecondsCount = 0;
  }
}

uint32_t RPM_Count(uint32_t frequency, uint16_t encoder_resolution)
{

  uint32_t RPM; // revolution per minute
  uint32_t RPS; // revolution per second

  RPS = frequency / encoder_resolution; // calculate revoultion per secon

  RPM = RPS * 60UL; // calculate revolution per minute

  return RPM;

  /* this RPM calculation can be done in one line: RPM = ((frequency* 60)/ resolution) */
}

struct Time displayTime(uint32_t elapsedTimeInMs)
{

  static uint32_t time = 0;

  time = elapsedTimeInMs / 1000UL; // convert from milliseconds to seconds

  struct Time time_to_display;

  time_to_display.hour = time / 3600; // 3600 seconds = 1 hour. we take only the hours.

  time_to_display.minutes = (time % 3600) / 60; // after removing the hours, the remainder seconds is converted to minutes by dividing with 60. 60 seconds = 1minute

  time_to_display.seconds = time % 60; // if you remove all the minutes, the remainder is the seconds

  return time_to_display;
}

struct FinalMeasurementParameters getRPMFrequencyAndDuration(uint16_t encoder_resolution, uint32_t reset_wait_time_in_milliseconds)
{

  //  uint32_t reset_wait_time_in_milliseconds  // this is the settime in millisseconds that dictates how long we wait before resetting countime after frequency equals zero
/*
  60000ms = 1s
  60000 * 5 = 300000ms = 300 seconds
  60 seconds = 1 minute;
  300 seconds = 5 minutes
*/

  struct FinalMeasurementParameters returnParameters = {0};

  if (freq_counting_finished)
  {

    uint32_t current_millis_count_time_reset_duration = millis();

    uint32_t RPM = RPM_Count(frequency, encoder_resolution);

    returnParameters.RPM =  RPM;    
    returnParameters.frequency = frequency; 

    if ((frequency == 0) && ((current_millis_count_time_reset_duration - previous_millis_count_time_reset_duration) > reset_wait_time_in_milliseconds))
    {

      previous_millis_count_time_reset_duration = current_millis_count_time_reset_duration;

      count_time = 0;
    }

    struct Time time_to_display = displayTime(count_time);
     returnParameters.time_to_display = time_to_display;

     returnParameters.filled_parameters = true;


    freq_counting_finished = false;

    
   
  }

  return returnParameters;
  
}


struct Freq_Counter freqCounter = { .begin = startCount, .getRPMFrequencyAndDuration = getRPMFrequencyAndDuration};