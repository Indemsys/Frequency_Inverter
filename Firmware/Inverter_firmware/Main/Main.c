#include "App.h"

void LCD_task(uint_32 initial_data);
static void Measure_task(uint_32 initial_data);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
  /* Task Index,    Function,                 Stack,  Priority,          Name,      Attributes,                                                           Param, Time Slice */
  { MOTISR_IDX,      Motor_ISR_task,           1000,   MOTISR_ID_PRIO,    "MotIsr",    MQX_FLOATING_POINT_TASK,                                             0,     0 },
  { CTRL_IDX,        Control_task,             1500,   CTRL_ID_PRIO,      "Ctrl",      MQX_FLOATING_POINT_TASK + MQX_AUTO_START_TASK,                       0,     0 },
  { MEAS_IDX,        Measure_task,             1500,   MEAS_ID_PRIO,      "Meas",      MQX_FLOATING_POINT_TASK,                                             0,     0 },
  { VT100_IDX,       VT100_task,               3000,   VT100_ID_PRIO,     "VT100",     MQX_FLOATING_POINT_TASK + MQX_TIME_SLICE_TASK,                       0,     2 },
  { LCD_IDX,         LCD_task,                 2000,   LCD_ID_PRIO,       "LCD",       MQX_FLOATING_POINT_TASK,                                             0,     0 },
  { CAN_TX_IDX,      CAN_Tx_Task,              1500,   CAN_TX_ID_PRIO,    "CAN_TX",    0,                                                                   0,     0 },
  { CAN_RX_IDX,      CAN_Rx_Task,              1500,   CAN_RX_ID_PRIO,    "CAN_RX",    MQX_FLOATING_POINT_TASK,                                             0,     0 },
  { 0 }
};


static float INT_smpl_to_current(int smpl);
static float INT_smpl_to_15v_meas(int smpl);
static float INT_smpl_to_5v_meas(int smpl);
static float INT_smpl_to_VBUS_meas(int smpl);
static float INT_smpl_to_Phase_meas(int smpl);
static float INT_smpl_to_Temper(int smpl);
static float INT_pwm_to_voltage(int smpl);

static float FLT_smpl_to_current(float smpl);
static float FLT_smpl_to_15v_meas(float smpl);
static float FLT_smpl_to_5v_meas(float smpl);
static float FLT_smpl_to_VBUS_meas(float smpl);
static float FLT_smpl_to_Phase_meas(float smpl);
static float FLT_smpl_to_Temper(float smpl);
static float FLT_pwm_to_voltage(float smpl);

static T_meas_results  meas_results[MEAS_RES_ARR_SZ];
static float           aver_curr_rms;
static unsigned int    aver_curr_cnt;

static MUTEX_STRUCT    meas_mutex;

const T_vals_scaling vscal[MEAS_RES_ARR_SZ] =
{
  { "ii_w  ", INT_smpl_to_current,    FLT_smpl_to_current    },
  { "ii_v  ", INT_smpl_to_current,    FLT_smpl_to_current    },
  { "ii_u  ", INT_smpl_to_current,    FLT_smpl_to_current    },
  { "v_bus ", INT_smpl_to_VBUS_meas,  FLT_smpl_to_VBUS_meas  },
  { "temper", INT_smpl_to_Temper,     FLT_smpl_to_Temper     },
  { "15v   ", INT_smpl_to_15v_meas,   FLT_smpl_to_15v_meas   },
  { "5v    ", INT_smpl_to_5v_meas,    FLT_smpl_to_5v_meas    },
  { "ii_u  ", INT_smpl_to_Phase_meas, FLT_smpl_to_Phase_meas },
  { "vv_w  ", INT_pwm_to_voltage,     FLT_pwm_to_voltage     },
  { "vv_v  ", INT_pwm_to_voltage,     FLT_pwm_to_voltage     },
  { "vv_u  ", INT_pwm_to_voltage,     FLT_pwm_to_voltage     },
}; 
/*
INT32U rms_log_pos; 
float meas_rms_log[MEAS_RES_ARR_SZ][MEAS_RES_LOG_SZ];
*/
float meas_stat_res[MEAS_RES_ARR_SZ];
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
/*
INT32U  Reset_rms_log(void)
{
  rms_log_pos = 0;
  return rms_log_pos;
} 
*/ 
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
/*
INT32U  Get_rms_log_pos(void)
{
  return rms_log_pos;
} 
*/ 
/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
/*
void Get_rms_log_arr(float **ptr)
{
  int i;
  for (i=0;i<MEAS_RES_ARR_SZ;i++)
  {
    ptr[i] = &meas_rms_log[i][0]; 
  }
}
*/

/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
static void Create_meas_mutex(void)
{
  MUTEX_ATTR_STRUCT mutexattr;
  _mutatr_init(&mutexattr);
  _mutex_init(&meas_mutex, &mutexattr);
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void Control_task(uint_32 initial_data)
{
  unsigned int cycle_cnt;
  T_MC_CBL     *mc_pcbl;

  Led_control(LED1, 1);
  Led_control(LED2, 0);
  Led_control(LED3, 0);

  MC_create_event();
  Create_meas_mutex();
  TempCtrl_init_drivers();

  CAN_init(CAN0_BASE_PTR, CAN_SPEED);

  ADCs_prepare(); // Подготовка и калибровка АЦП


  _task_create(0, MEAS_IDX,  0);
  _task_create(0, VT100_IDX, 0);
  _task_create(0, LCD_IDX,   0);


  _time_delay(1000); // Делаем паузу для заряда конденсаторов
  NTC_bypass_control(1);
  _time_delay(100);  // Делаем задержку для переключения контактов реле
  Led_control(LED1, 0);

  MC_init_PWM();
  Pin_PWM_OE_int_install();
  Pin_VFO_int_install();
  IGBT_reg_control(1); // Разрешаем регистр сигналов IGBT
  _task_create(0, MOTISR_IDX, 0);

  _task_create(0, CAN_RX_IDX, (uint_32)CAN0_BASE_PTR); // Задачу CAN запускаем после готовности остальных задач к приему команд, во избежании конфликтов  
  _task_create(0, CAN_TX_IDX, (uint_32)CAN0_BASE_PTR);


  mc_pcbl = MC_get_pcbl();

  // Следим за событиями
//  MC_set_events(MOTOR_START_R);

  for (;;)
  {
    _mqx_uint events;

    if  ( MC_get_events(&events, 2, VFO_FALL | PWM_OE_RISE | MOTOR_STOP | MOTOR_START_DOWN | MOTOR_START_UP) == MQX_OK )
    {
      if ( events & VFO_FALL )
      {
        MC_emergency_stop_motor();

        mc_pcbl->motor_drv_fail = 1;
        mc_pcbl->fail_cnt++;
        _time_delay(5000); // После сигнала аварии делаем продолжительную паузу
        mc_pcbl->motor_drv_fail = 0;
      }
      else if ( events & PWM_OE_RISE )
      {
        // Разорвалась цепь безопасности
        
        MC_emergency_stop_motor();
      }
      else if ( events & MOTOR_STOP )
      {
        if ( mc_pcbl->direction == MOVING_DOWN )
        {
          MC_stop_motor_moving(mc_pcbl->down_deceler_time);
        }
        else
        {
          MC_stop_motor_moving(mc_pcbl->up_deceler_time);
        }
      }
      else if ( events & MOTOR_START_DOWN )
      {
        MC_start_motor_moving(MOVING_DOWN, mc_pcbl->down_move_freq, mc_pcbl->down_acceler_time);
      }
      else if ( events & MOTOR_START_UP )
      {
        MC_start_motor_moving(MOVING_UP, mc_pcbl->up_move_freq, mc_pcbl->up_acceler_time);
      }
    }
    else
    {
      Led_control(LED1, cycle_cnt);
      cycle_cnt++;
    }

  }


//  _task_block();
}


/*-----------------------------------------------------------------------------------------------------
  Чувствительность дачика тока 40 mV/A
-----------------------------------------------------------------------------------------------------*/
static float INT_smpl_to_current(int smpl)
{
  float s  = 0.04;    // Чувствительность датчика
  float sr = 0.6875;  // Аттенюация резистивного делителя
  return (float)smpl * 3.3 / (4096.0 * s * sr);
}
/*-----------------------------------------------------------------------------------------------------
  Чувствительность дачика тока 40 mV/A
-----------------------------------------------------------------------------------------------------*/
static float FLT_smpl_to_current(float smpl)
{
  float s  = 0.04;    // Чувствительность датчика
  float sr = 0.6875;  // Аттенюация резистивного делителя
  return smpl * 3.3 / (4096.0 * s * sr);
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float INT_smpl_to_15v_meas(int smpl)
{
  float r1 = 4700.0;
  float r2 = 33000.0;
  return (float)smpl * 3.3 * (r1 + r2) * 1.013 / (r1 * 4096.0);
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float FLT_smpl_to_15v_meas(float smpl)
{
  float r1 = 4700.0;
  float r2 = 33000.0;
  return (float)smpl * 3.3 * (r1 + r2) * 1.013 / (r1 * 4096.0);
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float INT_smpl_to_5v_meas(int smpl)
{
  float r1 = 4700.0;
  float r2 = 4700.0;
  return (float)smpl * 3.3 * (r1 + r2) / (r1 * 4096.0);
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float FLT_smpl_to_5v_meas(float smpl)
{
  float r1 = 4700.0;
  float r2 = 4700.0;
  return (float)smpl * 3.3 * (r1 + r2) / (r1 * 4096.0);
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float INT_smpl_to_VBUS_meas(int smpl)
{
  float r1 = 2200.0;
  float r2 = 300000.0;
  return (float)smpl * 3.3 * (r1 + r2) * 0.964 / (r1 * 4096.0);
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float FLT_smpl_to_VBUS_meas(float smpl)
{
  float r1 = 2200.0;
  float r2 = 300000.0;
  return (float)smpl * 3.3 * (r1 + r2) * 0.964 / (r1 * 4096.0);
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float INT_smpl_to_Phase_meas(int smpl)
{
  float r1 = 2200.0;
  float r2 = 300000.0;
  return (float)smpl * 3.3 * (r1 + r2) / (r1 * 4096.0);
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float FLT_smpl_to_Phase_meas(float smpl)
{
  float r1 = 2200.0;
  float r2 = 300000.0;
  return (float)smpl * 3.3 * (r1 + r2) / (r1 * 4096.0);
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float INT_smpl_to_Temper(int smpl)
{
  float t;
  t = (float)smpl * 3.3 / 4096.0;
  t = 25.0 - (t - 0.719) / 0.001715;
  return t;
}
/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static float FLT_smpl_to_Temper(float smpl)
{
  float t;
  t = (float)smpl * 3.3 / 4096.0;
  t = 25.0 - (t - 0.719) / 0.001715;
  return t;
}
/*-----------------------------------------------------------------------------------------------------
  
-----------------------------------------------------------------------------------------------------*/
static float INT_pwm_to_voltage(int smpl)
{
  return (float)smpl ;
}
/*-----------------------------------------------------------------------------------------------------
  
-----------------------------------------------------------------------------------------------------*/
static float FLT_pwm_to_voltage(float smpl)
{
  return (float)smpl;
}


/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/

void Lock_meas_mutex(void)
{
  _mutex_lock(&meas_mutex);
} 
 
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/

void Unlock_meas_mutex(void)
{
  _mutex_unlock(&meas_mutex);
} 
 
/*-------------------------------------------------------------------------------------------------------------
  Задача выпоняющая измерения
-------------------------------------------------------------------------------------------------------------*/
static void Measure_task(uint_32 initial_data)
{
  signed short   *arr[MEAS_RES_ARR_SZ];
  unsigned int   n;
  unsigned int   i;
  unsigned int   sindx; // Стартовый индекс буфера
  T_meas_stat    stat;


  aver_curr_rms = 0;
  aver_curr_cnt = 0;
  for (;;)
  {
    
    _mqx_uint events;

    Lock_meas_mutex();
    if  ( MC_get_events(&events, 2, SMPL_ARR1_FULL + SMPL_ARR2_FULL) == MQX_OK )
    {
      // Обработать массив отсчетов

      // Получим указатели на массивы отсчетов
      ADC_get_smpl_arr(arr);
      sindx = 0;
      if ( events == SMPL_ARR2_FULL)
      {
        sindx = TST_SMPLS_ARR_SZ/2;
      }
      // Для каждого массива находим: минимум, максимум, среднее, среднеквадратическое
      // Выполнение этого блока длиться 4.8  мс
      for (n = 0; n < MEAS_RES_ARR_SZ; n++)
      {
        stat.maxv  = arr[n][sindx];
        stat.minv  = arr[n][sindx];
        stat.averv = 0;
        stat.rmsv  = 0;
        for (i = 0; i < (TST_SMPLS_ARR_SZ/2); i++)
        {
          if ( arr[n][i+sindx] > stat.maxv ) stat.maxv = arr[n][i+sindx];
          if ( arr[n][i+sindx] < stat.minv ) stat.minv = arr[n][i+sindx];
          stat.averv += arr[n][i+sindx];
          stat.rmsv  += arr[n][i+sindx] * arr[n][i+sindx];
        }
        stat.averv = stat.averv / (TST_SMPLS_ARR_SZ/2);
        stat.rmsv  = stat.rmsv / (TST_SMPLS_ARR_SZ/2);

        meas_results[n].fmax = vscal[n].int_converter(stat.maxv);
        meas_results[n].fmin = vscal[n].int_converter(stat.minv);
        meas_results[n].favr = vscal[n].int_converter(stat.averv);
        meas_results[n].frms = vscal[n].flt_converter(sqrt(stat.rmsv)); // sqrt выполняеться 756 тактов(6.3 мкс) включая перевод из double во float

      }
/*
      if ( rms_log_pos != MEAS_RES_LOG_SZ )
      {
        for (n = 0; n < MEAS_RES_ARR_SZ; n++)
        {
          meas_rms_log[n][rms_log_pos] = meas_results[n].frms;
        }
        rms_log_pos++;
      }
      ADC_reset_smpl_arr_pos();
*/ 
      {
        float  val = (meas_results[0].frms + meas_results[1].frms + meas_results[2].frms)/3;

        _int_disable();
         aver_curr_rms += val;
         aver_curr_cnt++; 
        _int_enable();
      }

      MC_set_events(MEAS_RES_READY);  
      

      Unlock_meas_mutex();
    }
  }
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
float Get_aver_curr(void)
{
  float val;
  _int_disable();
  val = aver_curr_rms/aver_curr_cnt;
  _int_enable();
  return val;
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void  Reset_aver_curr(void)
{
  _int_disable();
  aver_curr_rms = 0;
  aver_curr_cnt = 0;
  _int_enable();
}
/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void Get_copy_meas_results(T_meas_results *mres)
{
  _task_stop_preemption();
  memcpy(mres, meas_results, sizeof(meas_results));
  _task_start_preemption();
}


/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
#define HIGH_TEMPER_BOUND  50
#define LOW_TEMPER_BOUND   40

#define STR_SZ 64
void LCD_task(uint_32 initial_data)
{
  T_meas_results     mres[8];
  float              temp;
  char               str[STR_SZ + 1];
  T_MC_CBL          *mc_pcbl;
  int                terr; // Флаг ошибки чтения температуры

  mc_pcbl = MC_get_pcbl();
  LCD_init();

  for (;;)
  {
    Get_copy_meas_results(mres);

    terr = 0;
    if (TempCtrl_read_IGBT_temperature(&temp)!=MQX_OK)
    {
      terr = 1;
    }
    
    if ( terr==0 )
    {
      // Управление вентилятором
      if ( temp > HIGH_TEMPER_BOUND )
      {
        Fun_control(1);
      }
      if ( temp < LOW_TEMPER_BOUND )
      {
        Fun_control(0);
      }
    }



    if ( mc_pcbl->motor_drv_fail == 0 )
    {
      LCD_clear_row(0);
      LCD_clear_row(1);
      if ( terr!=0 )
      {
        snprintf(str, STR_SZ, "T:----- DC:%.0f", mres[3].frms);
      }
      else
      {
        snprintf(str, STR_SZ, "T:%0.1f DC:%.0f", temp, mres[3].frms);
      }
      LCD_print_str(str, 0, 0);
      {
        float iaver = (mres[0].frms + mres[1].frms + mres[2].frms)/3;

        if ( mc_pcbl->fail_cnt != 0 )
        {
          snprintf(str, STR_SZ, "I:%4.1f Err.N:%d", iaver, mc_pcbl->fail_cnt);
        }
        else
        {
          snprintf(str, STR_SZ, "I:%4.1f ", iaver);
        }
      }
      LCD_print_str(str, 0, 1);
    }
    else
    {
      snprintf(str, STR_SZ, "****OVERLOAD****");
      LCD_print_str(str, 0, 0);
      snprintf(str, STR_SZ, "*****ERROR******");
      LCD_print_str(str, 0, 1);
    }

    _time_delay(500);
  }

}

