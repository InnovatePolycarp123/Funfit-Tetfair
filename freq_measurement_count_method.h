#ifndef FREQ_MEASUREMENT_COUNT_METHOD_H
#define FREQ_MEASUREMENT_COUNT_METHOD_H

#include <Arduino.h>


extern volatile uint32_t Timer_one_overflow_count;
extern volatile uint32_t overflow_count_for_frequency_calculation;  // a copy of Timer_one_overflow_count to avoid racing condition
extern volatile double count_period;   // a global functio for calculating count period 
extern volatile uint32_t milliSecondsCount;
extern volatile uint32_t frequency; // frequency in this case represents CPS (counts per seconds). By count we mean number of pulses per second.
extern volatile bool freq_counting_finished;
extern volatile uint32_t count_time; // the variable used to measure the duration of all continuos total ferquency count resets some minutes after frequency equals zero.
extern uint32_t previous_millis_count_time_reset_duration; // this is variable that helps update the current_millis variable for resetting the time duration after we stop frequency measurement


void startCount(double set_count_period_in_ms);
uint32_t RPM_Count(uint32_t frequency, uint16_t encoder_resolution);
struct Time displayTime(uint32_t elapsedTimeInMs);
struct FinalMeasurementParameters getRPMFrequencyAndDuration( uint16_t encoder_resolution,  uint32_t reset_wait_time_in_millisecond);


struct Time {
  uint8_t hour;
  uint8_t minutes;
  uint8_t seconds;
};

struct Freq_Counter { 

    void(*begin)(double set_count_period_in_ms);
    struct FinalMeasurementParameters (*getRPMFrequencyAndDuration) (uint16_t resolution,  uint32_t reset_wait_time_in_milliseconds);


};


extern struct Freq_Counter freqCounter;

struct FinalMeasurementParameters{

      struct Time time_to_display;

      uint32_t RPM;

      uint32_t frequency;

      bool filled_parameters;

};


























#endif