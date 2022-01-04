#ifndef __SIN_COS_GENERATOR
  #define __SIN_COS_GENERATOR

#define PH_MAX    0x100000000ull

void Gen_start(unsigned int freq);
void Gen_update_freq(unsigned int freq);
void Get_generator_sample(Frac32 *psin, Frac32 *pcos);

#endif