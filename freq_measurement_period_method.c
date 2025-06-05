#include "freq_measurement_period_method.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this variable is responsible for choosing the method of measurement: no_limit_lower_frequency method or limited_lower_frequency_method

bool choose_method; // if this variable is set to true then we are using no_limit_lower_frequency method and vice versa; if the required function is not called we'll be using no_limit_lower_frequency method by default

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following variables in this section will try to measure frequency of max 250khz and min 3.8hz
//  it uses some variables in the other method. variables such as first_capture and second_capture
volatile uint32_t sum_of_the_difference_between_first_captures_and_second_captures_within_a_full_timer_count;
volatile uint32_t number_of_first_capture_subtracted_by_second_capture_within_a_full_timer_count;
volatile bool timer_counter_full = false;
volatile bool capture_still_on = false;  // this flag is to help us stop reading or updating after capture interrupt stops.
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

volatile uint32_t overflowcount = 0;
volatile uint8_t no_of_captures;
volatile uint32_t first_capture;
volatile uint32_t second_capture;
volatile bool first_capture_done = false;
volatile bool second_capture_done = false;
volatile uint32_t no_of_overflow_counts_between_first_capture_and_second_capture;
float timer_one_period = 4e-6;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool measurement_started = false; // This variable will help us learn when measurement has started;
volatile uint32_t start_time;
volatile uint32_t last_measurement_occurrence = 0;
uint32_t elapsed_time_milliseconds = 0;

void setTimerOneParameters()
{
    TCCR1B = 0;
    TCCR1A = 0;
    TCNT1 = 0;
    ICR1 = 0;
    TCCR1B = 0b11000011; // enable input capture noise canceller, enable input capture on rising edge. set prescalar to 64
    TIMSK1 = 0b00100001; // enable input capture interrupt, enable timer overflow interrupt
}

ISR(TIMER1_CAPT_vect)
{

    last_measurement_occurrence = micros();
    no_of_captures++;
    capture_still_on = true;
    

    if (!measurement_started)
    {

        start_time = micros();
        measurement_started = true;
        elapsed_time_milliseconds = 0;
    }

    if (no_of_captures == 1)
    {

        first_capture = ICR1;

        if (choose_method == true)
        {
            first_capture_done = true;
        }
    }

    if (no_of_captures == 2)
    {

        second_capture = ICR1;

        if (choose_method == false)
        {

            sum_of_the_difference_between_first_captures_and_second_captures_within_a_full_timer_count += (second_capture - first_capture);
            number_of_first_capture_subtracted_by_second_capture_within_a_full_timer_count++;

            second_capture = 0;
            first_capture = 0;
            no_of_captures = 0;
        }

        if (choose_method == true)
        {

            no_of_overflow_counts_between_first_capture_and_second_capture = overflowcount;
            overflowcount = 0;
            first_capture_done = false;
            second_capture_done = true;
        }
    }
}

ISR(TIMER1_OVF_vect)
{

    if (choose_method == true)
    {

        if (first_capture_done)
        {
            overflowcount++;
        }
    }

    if (choose_method == false)
    {
        timer_counter_full = true;
        second_capture = 0;
        first_capture = 0;
        no_of_captures = 0;
    }
}

void setMeasurementMethod(bool freq_measurement_method)
{
    /* if you set this functions parameter to true you will use the method that has no low frequency limit*/

    if (freq_measurement_method)
    {
       choose_method = true;
    }
    else if (!freq_measurement_method)
    {

        choose_method = false;
    }

}

struct Counts_time_and_frequency getCountandFrequency()
{ // this function is used to calculate frequency and period of no_lower_limit method

    struct Counts_time_and_frequency counts_time_and_frequency_buffer = {0};

    if (second_capture_done)
    {

        counts_time_and_frequency_buffer.number_of_counts_between_first_capture_and_second_capture = (65536 - first_capture) + ((no_of_overflow_counts_between_first_capture_and_second_capture - 1) * 65536) + second_capture; // the idea is to get the number of ticks between first capture and second capture
        counts_time_and_frequency_buffer.time_between_rising_edges = (double)counts_time_and_frequency_buffer.number_of_counts_between_first_capture_and_second_capture * timer_one_period;
        counts_time_and_frequency_buffer.frequency = 1 / counts_time_and_frequency_buffer.time_between_rising_edges;
        counts_time_and_frequency_buffer.filled_buffer = true;

        second_capture_done = false;
        no_of_captures = 0;

        return counts_time_and_frequency_buffer;
    }

    return counts_time_and_frequency_buffer;
}

struct period_and_frequency getPeriodAndfrequency()
{ // this function is used to calculate frequency and period of limited_lower_limit method

    struct period_and_frequency period_and_frquency_buffer = {0};

    if (timer_counter_full && capture_still_on)
    {

        period_and_frquency_buffer.period = (double)(sum_of_the_difference_between_first_captures_and_second_captures_within_a_full_timer_count / number_of_first_capture_subtracted_by_second_capture_within_a_full_timer_count) * timer_one_period;
        period_and_frquency_buffer.frequency = 1 / period_and_frquency_buffer.period;
        period_and_frquency_buffer.filled_buffer = true;
        timer_counter_full = false;
        capture_still_on = false; 
        sum_of_the_difference_between_first_captures_and_second_captures_within_a_full_timer_count = 0;
        number_of_first_capture_subtracted_by_second_capture_within_a_full_timer_count = 0;

        return period_and_frquency_buffer;
    }

    return period_and_frquency_buffer;
}

uint32_t RPM_Count(uint32_t frequency, uint16_t resolution)
{

    uint32_t RPM; // revolution per minute
    uint32_t RPS; // revolution per second

    RPS = frequency / resolution; // calculate revoultion per secon

    RPM = RPS * 60UL; // calculate revolution per minute

    return RPM;

    /* this RPM calculation can be done in one line: RPM = ((frequency* 60)/ resolution) */
}

struct Time displayTime(uint32_t elapsedTimeInMs)
{

    static uint32_t time = 0;

    time = elapsedTimeInMs / 1000UL; // convert from milliseconds to seconds

    struct Time time_to_display;

    time_to_display.hour = time / 3600;

    time_to_display.minutes = (time % 3600) / 60;

    time_to_display.seconds = time % 60;

    time_to_display.time_filled = true;

    return time_to_display;
}

struct Time count_time(uint32_t maximum_wait_time_to_reset_after_last_measurement_occurrence_in_microseconds)
{

    /* maximum_wait_time_to_reset_after_last_measurement_occurrence_in_microseconds is the time we have to wait after every measurement ends for the time display to clear
     60000ms = 1s
     60000 * 5 = 300000ms = 300 seconds = 5 minutes
     1ms = 1000 microseconds
     300000ms =  300000000 microseconds = 5 minutes;
      60 seconds = 1 minute;
      300 seconds = 5 minutes;
    */

    struct Time time_to_display = {0};
    uint32_t current_micros, last_occurrence;

    noInterrupts();
    current_micros = micros();
    last_occurrence = last_measurement_occurrence;
    interrupts();

    if (measurement_started)
    {
        // Update every second without blocking
        if (current_micros - start_time >= 1000000)
        {
            start_time = current_micros;
            elapsed_time_milliseconds += 1000;
        }

        time_to_display = displayTime(elapsed_time_milliseconds);

        if (current_micros - last_occurrence >= maximum_wait_time_to_reset_after_last_measurement_occurrence_in_microseconds)
        {
            measurement_started = false;
        }
    }

    return time_to_display;
}

struct FinalMeasurementParameters getMeasurements(uint32_t delay_period, uint16_t encoder_resolution, uint32_t max_reset_time)
{

    struct FinalMeasurementParameters finalMeasurementParameters = {0};

    if (choose_method == true)
    {

        struct Counts_time_and_frequency printable_buffer = getCountandFrequency();
        uint32_t RPM = RPM_Count(printable_buffer.frequency, encoder_resolution);
        struct Time time_to_display = count_time(max_reset_time);

        if (printable_buffer.filled_buffer == true)
        {

            finalMeasurementParameters.frequency = printable_buffer.frequency;
            finalMeasurementParameters.RPM = RPM;

            if (time_to_display.time_filled == true) finalMeasurementParameters.time_to_display = time_to_display;

            finalMeasurementParameters.buffer_filled = true;
        }
    }

    if (choose_method == false)
    {

        struct period_and_frequency printable_buffer_two = getPeriodAndfrequency();
        struct Time time_to_display = count_time(max_reset_time);
        uint32_t RPM = RPM_Count(printable_buffer_two.frequency, encoder_resolution);

        if (printable_buffer_two.filled_buffer == true)
        {

              finalMeasurementParameters.frequency = printable_buffer_two.frequency;
              finalMeasurementParameters.RPM = RPM;

            if (time_to_display.time_filled == true) finalMeasurementParameters.time_to_display = time_to_display;

            finalMeasurementParameters.buffer_filled = true;
        }
    }

    delay(delay_period);

    return finalMeasurementParameters;
}

struct FreqMeasure freqMeasurement = {.begin = setTimerOneParameters, .getFrequencyRPMTime = getMeasurements, .setMeasurementMethod = setMeasurementMethod};