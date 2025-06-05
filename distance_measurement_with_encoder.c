#include "distance_measurement_with_encoder.h"

struct DistanceMeasurement FindDistance = {
    .getDistance = getDistance,
    .getDistanceFromRevolutionCountOnly = getDistanceFromRevolutionCountOnly,
    .getPulseToDistanceRatio = getPulseToDistanceRatio,
    .resetRevolutionCountIfWheelHasStopped = resetRevolutionCountIfWheelHasStopped,
    .TimerStartUp = TimerStartUp,
};

void TimerStartUp(uint32_t encoder_resolution)
{

  TCCR1B = 0;
  TCCR1A = 0;
  TCNT1 = 0;
  /*    reset all previous timers */

  TCCR1B = 0b00001111;        // set timer one to ctc mode and and external clock
  TIMSK1 = 0b00000010;        // enable output compare interrupt on output compare channel A;
  OCR1A = encoder_resolution; // set the output compare number to resolution,
                              // so each compare interrupt equals one revolution
}

struct MeasurementParameters UsedMeasurementParameters = {0};

ISR(TIMER1_COMPA_vect)
{
  UsedMeasurementParameters.last_interrupt_time = micros();
  UsedMeasurementParameters.revolution_count++;
}

struct timerOneMeasurementResults getTimerCountOneCurrentValue()
{

  uint8_t status_register;                                      // create a buffer for atmega328 status register
  struct timerOneMeasurementResults temporal_timer_one_results; // create a buffer for the timer

  status_register = SREG; // store the current state of the status register in the status register buffer.
  /*
  note the I-bit of the  status register contains the current value of global interrupt.
  */

  cli(); // disable global interrupts

  temporal_timer_one_results.number_of_pulses_in_current_revolution = TCNT1;                // get the current value of TCNT1
  temporal_timer_one_results.revolution_count = UsedMeasurementParameters.revolution_count; // get the current number of revolutions

  SREG = status_register; // restore the value of the status register to its previous state. this will enable global interrupt if it was already enabled before the function was called.
                          // in other words, it will restore global interrupts to its previous state before the function was called

  return temporal_timer_one_results;
}

double getDistance(double pulse_to_distance_ratio, uint16_t encoder_resolution)
{ // this function converts total number of pulses to revolution count using both total number of revoltions and tcnt1 values. this should be more accurate

  struct timerOneMeasurementResults timer_one_measurement_results = getTimerCountOneCurrentValue(); // get the current values of timer_one i.e revolution count and current tcnt1 values

  uint32_t total_number_of_pulses = (timer_one_measurement_results.revolution_count * encoder_resolution) + timer_one_measurement_results.number_of_pulses_in_current_revolution; // use the timer one values to get
                                                                                                                                                                                  // to get the total number of pulses that has been registerd.
  FindDistance.get_pulses.number_of_pulses_from_get_distance = total_number_of_pulses;                                                                                             // note: each revolution count  equals the encoder resolution

  double distance_in_meters = (double)total_number_of_pulses * pulse_to_distance_ratio; // convert the total number of pulses to meters

  return distance_in_meters;
}

double getDistanceFromRevolutionCountOnly(double pulse_to_distance_ratio, uint16_t encoder_resolution)
{ // this function converts total number of pulses to revolution count using only total number of revoltions.

  struct timerOneMeasurementResults timer_one_measurement_results = getTimerCountOneCurrentValue(); // get the current values of timer_one i.e revolution count and current tcnt1 valuesthe

  uint32_t number_of_pulses = timer_one_measurement_results.revolution_count * encoder_resolution; // use the timer one values to get
                                                                                                   // to get the total number of pulses that has been registerd.
                                                                                                   // note: each revolution count  equals the encoder resolution

   FindDistance.get_pulses.number_of_pulses_from_get_distance_from_revolution = number_of_pulses;                                                                                                  

  double distance_in_meters = (double)number_of_pulses * pulse_to_distance_ratio; // convert the total number of pulses to meters

  return distance_in_meters;
}

void resetRevolutionCountIfWheelHasStopped(uint32_t revolution_reset_time_after_system_stops_in_micro_seconds)
{ // this function is to help us everything when encoder and wheel stops revolving

  cli();
  uint32_t safe_to_use_last_interrupt_time = UsedMeasurementParameters.last_interrupt_time; // store the last time there was an compare interrupt
  uint32_t current_micros = micros();                                                       // get current time
  sei();

  if (current_micros - safe_to_use_last_interrupt_time >= revolution_reset_time_after_system_stops_in_micro_seconds) // check if we have exceeded a preset time since the last interrupt time;
  {

    if (!UsedMeasurementParameters.wheel_has_stopped_moving) // this was to prevent an issue that caused the sytem not continually reset even after revolutions tarts again.
    {
      // it works by allowing reset to happen only once after revolution stops.

      // normally after revoltion stops the upper if statement becomes trues after the preset time. so the tcnt1 and revoltion count are reset multiple times before the revolution continues

      cli();
      UsedMeasurementParameters.revolution_count = 0;
      TCNT1 = 0;
      sei();

      UsedMeasurementParameters.wheel_has_stopped_moving = true;
    }
  }
  else
  {

    UsedMeasurementParameters.wheel_has_stopped_moving = false;
  }
}

double getPulseToDistanceRatio(uint16_t encoder_gear_ratio, uint16_t encoder_resolution, double wheel_diameter_in_meter)
{

  uint16_t wheel_revolution_to_encoder_pulse_ratio; // this tells us how many encoder pulses equal one wheel revolution

  /*

    NOTE: by wheel, i mean the wheel of the motor tire  we want to measure distance from. encoder_gear_ratio represents the gear ratio of our encoder to the wheel. if encoder ratio equals 48, it means:

    wheel revolution = 1, encoder_gear_ratio = 48. for every wheel revolution, we have 48 encoder revolutions.

    Now, if each encoder revolution produces 8 pulses, one wheel revolution causes 48 encoder revolutions, which means we'll get 8 * 48 pulses.

    for example:

      if one encoder revolution equal (encoder resolution) = 8 pulses, and for every wheel revolution we have 48 encoder revolution ( encoder_gear_ratio).

      then, number of pulses within one wheel revolution equal = encoder resolution * encoder_gear_ratio. = 48 * 8 = 384 pulses.

      that's to say; if each encoder revolution equals eight pulses and one wheel revolution equals 48 enconder revolutions. one wheel revolution = 384 pulse, which is the wheel_revolution_to_encoder_pulse_ratio

    */

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  wheel_revolution_to_encoder_pulse_ratio = encoder_gear_ratio * encoder_resolution;

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // now we'll calculate cicumference of the wheel.

  double wheel_circumference = PI * wheel_diameter_in_meter;

  // the circumference tell us the distance covered by each wheel revolution. for example, assuming diameter of 60mm, circumference is 185mm. so each wheel revolution equals 185mm distance covered.
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // now we'll calculate what each encoder pulse means in terms of distance covered. that's pulse_to_distance ratio;

  double pulse_to_distance_ratio_in_meters = wheel_circumference / (double)wheel_revolution_to_encoder_pulse_ratio;

  /*          Think of it this way:

              using our previous examples, each wheel revolution equals the movement of a complete wheel_circumference (distance_covered).
              As a result, every time we have one wheel revolution (185mm covered), the encoder produces wheel_revolution_to_encoder_pulse_ratio ( 384 pulses).

              if 185mm = 384 pulses,
                 x_in_mm = 1 pulse

                 x_in_mm  =  185/384

            NOTE:     i just used mm in this example, it can be Meters. in fact, I am using meters

  */

  return pulse_to_distance_ratio_in_meters;

  // everything is done using the double data_type to increase precision.
}
