#ifndef PIT_H_
  #define PIT_H_

void PIT0_init(unsigned int period);
void PIT0_int_enable(void);
void PIT0_int_disable(void);
void PIT1_init(unsigned int period );

#endif
