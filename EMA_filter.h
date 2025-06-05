#ifndef EMA_filter_H
#define EMA_filter_H

#include <Arduino.h>

/*
 an EMA filter is a type of IIR filter

 one advantage of using the EMA low pass filter is it has only one parameter to control:
 the alpha parameter (α).

 

The formular for EMA low pass filter is:
// Yn = α * Xn + (1 - α) * (Yn - 1);

Yn = this is our output signal sample
 Xn  = this is our input  signal sample
 (Yn - 1) = our previous output signal sample
 α = this is the parameter we change to increase or decrease low-pass filtering. 

the alpha parameter (α) ranges from 0 to one:

when α = 1, we get zero to minimal filtering
when α = 0, we get maximum filtering.


on the same note, the advantage of using the EMA high-pass filter is it has only one parameter to control:
 the beta parameter (β).


The formular for EMA high-pass filter is:
// Yn = (1/2) * (2 - β) * (Xn - (Xn - 1))  + (1 -  β ) * (Yn - 1);

Yn = this is our output signal sample
 Xn  = this is our input  signal sample
 (Yn - 1) = our previous output signal sample 
  (Xn - 1) =  our previous input signal sample 

  the beta parameter (β) ranges from 0 to one:

when β= 1, we get maximum filtering
when β = 0, we get minimal  filtering.  

As you can see, the beta parameter (β) acts the opposite of the alpha parameter (α)
 
*/

struct LOW_PASS_EMA_FILTER_PARAMETERS{
      double alpha;
      double filtered_output;
};

struct HIGH_PASS_EMA_FILTER_PARAMETERS{
      double beta;
      double filtered_output;
      double input;
};

void LOW_PASS_EMA_Begin(struct  LOW_PASS_EMA_FILTER_PARAMETERS  *filter,  double alpha);

void LOW_PASS_SetEMA_Alpha(struct  LOW_PASS_EMA_FILTER_PARAMETERS *filter,  double alpha);

double LOW_PASS_getEMA_Output(struct  LOW_PASS_EMA_FILTER_PARAMETERS *filter,  double input);


void HIGH_PASS_EMA_Begin(struct  HIGH_PASS_EMA_FILTER_PARAMETERS  *filter,  double beta);

void HIGH_PASS_SetEMA_Beta(struct  HIGH_PASS_EMA_FILTER_PARAMETERS *filter,  double beta);

double HIGH_PASS_getEMA_Output(struct  HIGH_PASS_EMA_FILTER_PARAMETERS *filter,  double input);


struct LOW_PASS_EMA_Filter{

    void (*LOW_PASS_EMA_Begin)(struct LOW_PASS_EMA_FILTER_PARAMETERS *filter,  double alpha);

    void (*LOW_PASS_SetEMA_Alpha)(struct LOW_PASS_EMA_FILTER_PARAMETERS  *filter,  double alpha);

    double (*LOW_PASS_getEMA_Output)(struct LOW_PASS_EMA_FILTER_PARAMETERS  *filter,  double input);

};


struct HIGH_PASS_EMA_Filter{

    void (*HIGH_PASS_EMA_Begin)(struct HIGH_PASS_EMA_FILTER_PARAMETERS *filter,  double beta);

    void (*HIGH_PASS_SetEMA_Beta)(struct HIGH_PASS_EMA_FILTER_PARAMETERS  *filter,  double beta);

    double (*HIGH_PASS_getEMA_Output)(struct HIGH_PASS_EMA_FILTER_PARAMETERS *filter,  double input);

};

extern struct  LOW_PASS_EMA_Filter LowPassEMAFilter;

extern struct  HIGH_PASS_EMA_Filter HIGHPassEMAFilter;

#endif
