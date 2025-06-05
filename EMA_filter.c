#include "EMA_filter.h"

struct HIGH_PASS_EMA_Filter HIGHPassEMAFilter = { .HIGH_PASS_EMA_Begin = HIGH_PASS_EMA_Begin,
                                                  .HIGH_PASS_getEMA_Output = HIGH_PASS_getEMA_Output,
                                                  .HIGH_PASS_SetEMA_Beta = HIGH_PASS_SetEMA_Beta

};

struct LOW_PASS_EMA_Filter LowPassEMAFilter = {.LOW_PASS_EMA_Begin = LOW_PASS_EMA_Begin,
                                               .LOW_PASS_getEMA_Output = LOW_PASS_getEMA_Output,
                                               .LOW_PASS_SetEMA_Alpha = LOW_PASS_SetEMA_Alpha

};

void LOW_PASS_EMA_Begin(struct LOW_PASS_EMA_FILTER_PARAMETERS *filter, double alpha)
{

  filter->filtered_output = 0.0L;
  LOW_PASS_SetEMA_Alpha(filter, alpha);
}

void LOW_PASS_SetEMA_Alpha(struct LOW_PASS_EMA_FILTER_PARAMETERS *filter, double alpha)
{

  if (alpha >= 1.0)
  {

    alpha = 1.0;
  }
  else if (alpha <= 0.0)
  {
    alpha = 0.0;
  }

  filter->alpha = alpha;
}

double LOW_PASS_getEMA_Output(struct LOW_PASS_EMA_FILTER_PARAMETERS *filter, double input)
{

  // Yn = α + Xn + (1 - α) * (Yn - 1);//

  filter->filtered_output = filter->alpha * input + (1.0 - filter->alpha) * filter->filtered_output;

  return filter->filtered_output;
}

void HIGH_PASS_EMA_Begin(struct HIGH_PASS_EMA_FILTER_PARAMETERS *filter, double beta)
{
  filter->filtered_output = 0.0L;
  HIGH_PASS_SetEMA_Beta(filter, beta);
  filter->input = 0.0L;
}

void HIGH_PASS_SetEMA_Beta(struct  HIGH_PASS_EMA_FILTER_PARAMETERS *filter,  double beta)
{

   if (beta >= 1.0)
  {

    beta = 1.0;
  }
  else if (beta <= 0.0)
  {
    beta = 0.0;
  }

  filter->beta = beta;
}

double HIGH_PASS_getEMA_Output(struct HIGH_PASS_EMA_FILTER_PARAMETERS *filter, double input)
{

  filter->filtered_output = 0.5 * (2.0 - filter->beta) * (input - filter->input) + (1.0 - filter->beta) * filter->filtered_output;

   filter->input = input;  // store the previous input;

   return filter->filtered_output;
}
