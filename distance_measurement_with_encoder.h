#ifndef distance_measurement_with_encoder_h
#define distance_measurement_with_encoder_h

#include <Arduino.h>

struct MeasurementParameters{

  volatile uint32_t last_interrupt_time;
  volatile uint32_t revolution_count;
  volatile bool  wheel_has_stopped_moving;

};

struct NumberOfPulses{

  uint32_t number_of_pulses_from_get_distance;
  uint32_t number_of_pulses_from_get_distance_from_revolution;

};


struct timerOneMeasurementResults {

  uint32_t revolution_count;
  uint32_t number_of_pulses_in_current_revolution;
};

void TimerStartUp(uint32_t encoder_resolution);
double getPulseToDistanceRatio(uint16_t encoder_gear_ratio, uint16_t encoder_resolution, double wheel_diameter_in_meter);
double getDistance(double pulse_to_distance_ratio, uint16_t encoder_resolution);
double getDistanceFromRevolutionCountOnly(double pulse_to_distance_ratio, uint16_t encoder_resolution);
struct timerOneMeasurementResults getTimerCountOneCurrentValue();
void resetRevolutionCountIfWheelHasStopped(uint32_t revolution_reset_time_after_system_stops_in_micro_seconds);


struct DistanceMeasurement{

   void (*TimerStartUp) (uint32_t encoder_resolution);
   double (*getPulseToDistanceRatio) (uint16_t encoder_gear_ratio, uint16_t encoder_resolution, double wheel_diameter_in_meter);
   double (*getDistance)(double pulse_to_distance_ratio, uint16_t encoder_resolution);
   double (*getDistanceFromRevolutionCountOnly)(double pulse_to_distance_ratio, uint16_t encoder_resolution);
   void (*resetRevolutionCountIfWheelHasStopped)(uint32_t revolution_reset_time_after_system_stops_in_micro_seconds);
  struct NumberOfPulses get_pulses;

};


extern struct DistanceMeasurement FindDistance; 


#endif 