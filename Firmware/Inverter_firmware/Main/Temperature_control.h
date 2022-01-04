#ifndef __TEMPERATURE_CONTROL
  #define __TEMPERATURE_CONTROL

_mqx_int TempCtrl_init_drivers(void);
_mqx_int TempCtrl_read_IGBT_temperature(float *temp_ptr);

#endif
