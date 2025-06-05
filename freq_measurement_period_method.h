#ifndef freq_measurement_period_method_h
#define freq_measurement_period_method_h

#include <Arduino.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this variable is responsible for choosing the method of measurement: no_limit_lower_frequency method or limited_lower_frequency_method

extern bool choose_method; // if this variable is set to true then we are using no_limit_lower_frequency method and vice versa; if the required function is not called we'll be using no_limit_lower_frequency method by default

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following variables in this section will try to measure frequency of max 250khz and min 3.8hz
//  it uses some variables in the other method. variables such as first_capture and second_capture
extern volatile uint32_t sum_of_the_difference_between_first_captures_and_second_captures_within_a_full_timer_count;
extern volatile uint32_t number_of_first_capture_subtracted_by_second_capture_within_a_full_timer_count;
extern volatile bool timer_counter_full;
extern volatile bool capture_still_on; // this flag is to help us stop reading or updating after capture interrupt stops.
// why? this is because we'll always have an overflow interrupt when using the second method. that's an overflow interrupt every 0.262 seconds.

// this second method is only good at low frequencies. At frequencies below 192hz it's super accurate.
// However, anything above that accuracy decreases.the accuracy keeeps reducing fro 1 to ten to hundred and then a thousand ans so on as you go. I think it can be calibrated to subtract the inaccuracies.
// But it should not be used above 22khz; it becomes more difficult to calibrate not impossible. the lower frequency limit is 8hz realistically. theoretically, it should go 3.8hz.

////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
This method  of frequency measurement ( counting number of overflows and the values in first capture and second capture to get maximum ticks between first capture and second capture. the advantage is there's no low frquency limit ) is only good at lower frequencies. it becomes inacurate as the frequency increasies.
I could only measure up to 5khz near accurate. Theoretically I should be able to measure 250khz with a prescalar of 64. this works very acuurately at lower frequencies 1hz to 2kz. from 3khz upwards accuracy reduces. you still get
accurate values but not as accurate as below 2khz for example 3.920khz will measure as 3205khz. above 5khz it enters uncharted theory.

*/

extern volatile uint32_t overflowcount;
extern volatile uint8_t no_of_captures;
extern volatile uint32_t first_capture;
extern volatile  uint32_t second_capture;
extern volatile bool first_capture_done;
extern volatile bool second_capture_done;
extern volatile uint32_t no_of_overflow_counts_between_first_capture_and_second_capture;
extern float timer_one_period;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool measurement_started; // This variable will help us learn when measurement has started;
extern volatile uint32_t start_time;
extern volatile uint32_t last_measurement_occurrence;
extern uint32_t maximum_wait_time_to_reset_after_last_measurement_occurrence; // this is the set seconds that dictates how long we wait before resetting countime after frequency equals zero
extern uint32_t elapsed_time_milliseconds;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Counts_time_and_frequency
{

    uint32_t number_of_counts_between_first_capture_and_second_capture;
    uint32_t frequency;
    double time_between_rising_edges;
    bool filled_buffer;
};

struct period_and_frequency
{

    double period;
    uint32_t frequency;
    bool filled_buffer;
};

struct Time
{
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
    bool time_filled;
};

struct FreqMeasure
{

    void (*begin)(void);
    void (*setMeasurementMethod) (bool freq_measurement_method);
    struct FinalMeasurementParameters (*getFrequencyRPMTime) (uint32_t delay_period, uint16_t encoder_resolution,  uint32_t max_reset_time);
};

struct FinalMeasurementParameters
{

    struct Time time_to_display;

    uint32_t frequency;

    uint32_t RPM;

    bool buffer_filled;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Counts_time_and_frequency getCountandFrequency();
struct period_and_frequency getPeriodAndfrequency();
struct Time displayTime(uint32_t elapsedTimeInMs);
uint32_t RPM_Count(uint32_t frequency, uint16_t resolution);
void setMeasurementMethod(bool freq_measurement_method);
void setTimerOneParameters();
struct FinalMeasurementParameters getMeasurements(uint32_t delay_period, uint16_t encoder_resolution,  uint32_t max_reset_time);
struct Time count_time(uint32_t maximum_wait_time_to_reset_after_last_measurement_occurrence );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern struct FreqMeasure freqMeasurement; 

#endif

