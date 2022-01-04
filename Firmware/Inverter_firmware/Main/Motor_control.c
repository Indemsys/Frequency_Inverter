#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include "App.h"
#include <math.h>

extern signed short   smpls[11][TST_SMPLS_ARR_SZ];
extern unsigned int   arr_pos;


static T_MC_CBL            mc_cbl;
volatile static uint32_t   dummy;
static LWEVENT_STRUCT      evt_grp;
static pointer             mc_is_taskr_queue;

Frac32 sinv, cosv;

static void ETM0_isr(pointer user_isr_ptr);


/*-------------------------------------------------------------------------------------------------------------
  Пересчет значения PWM из Frac32 в реальное целое значение с границей  PWM_MODULO
-------------------------------------------------------------------------------------------------------------*/
static unsigned int MC_scale_PWM_ch(Frac32 pwm)
{
  Word64  lltmp;
  lltmp = (Word64)pwm * (Word64)PWM_MODULO * 2 - (Word64)PWM_MODULO * FRAC32(0.5);
  lltmp = lltmp / 0x80000000ul;

  // Здесь предотвращаем появление слишком коротких импульсов
  if ( lltmp < PWM_MARGIN_LO_LEV ) lltmp = 0;
  else if ( lltmp > (PWM_MODULO - PWM_MARGIN_HI_LEV) ) lltmp = PWM_MODULO;

//  if ( lltmp < PWM_MARGIN_LO_LEV ) lltmp = PWM_MARGIN_LO_LEV;
//  else if ( lltmp > (PWM_MODULO - PWM_MARGIN_HI_LEV) ) lltmp = PWM_MODULO - PWM_MARGIN_HI_LEV;

  return (unsigned int)lltmp;
}
/*-------------------------------------------------------------------------------------------------------------
  Расчет параметров PWM 
-------------------------------------------------------------------------------------------------------------*/
#define KSCALE  1
void MC_calculate_PWM(T_3ph_pwm *pwm_3ph_ptr)
{
  float                          angle_flt;
  MCLIB_2_COOR_SYST_ALPHA_BETA_T in_voltage;
  MCLIB_3_COOR_SYST_T            pwm_abc;
  UWord32                        sector;
  Word64                         lltmp;
  Frac32                         x;

  Get_generator_sample(&in_voltage.f32Beta, &in_voltage.f32Alpha);

  // Промасштабировать амплитуду пропорционально частоте
  if ( mc_cbl.mot_freq < MAX_MOT_FREQ )
  {
    if ( mc_cbl.direction == MOVING_UP )
    {
      x = FRAC32((float)mc_cbl.ll_mot_freq * mc_cbl.pwm_scale / ((float)(MAX_MOT_FREQ) * (float)(1ull << 32))); // Уменьшаем амплитуду вектора пропорционально частоте
    }
    else
    {
      x = FRAC32((float)mc_cbl.ll_mot_freq * mc_cbl.pwm_scale / ((float)(MAX_MOT_FREQ) * (float)(1ull << 32))); // Уменьшаем амплитуду вектора пропорционально частоте
    }
  }
  else
  {
    if ( mc_cbl.direction == MOVING_UP )
    {
      x = FRAC32(mc_cbl.pwm_scale);
    }
    else
    {
      x = FRAC32(mc_cbl.pwm_scale);
    }
  }

  in_voltage.f32Beta  = F32Mul(x, in_voltage.f32Beta);
  in_voltage.f32Alpha = F32Mul(x, in_voltage.f32Alpha);

  sector = GMCLIB_SvmStd(&pwm_abc, &in_voltage);

  pwm_3ph_ptr->pwm_a = MC_scale_PWM_ch(pwm_abc.f32A);
  if ( mc_cbl.direction == 0 )
  {
    pwm_3ph_ptr->pwm_b = MC_scale_PWM_ch(pwm_abc.f32B);
    pwm_3ph_ptr->pwm_c = MC_scale_PWM_ch(pwm_abc.f32C);
  }
  else
  {
    pwm_3ph_ptr->pwm_b = MC_scale_PWM_ch(pwm_abc.f32C);
    pwm_3ph_ptr->pwm_c = MC_scale_PWM_ch(pwm_abc.f32B);
  }

  //printf("%d,%d,%d,%d,%d,%d,%d\r\n",mc_cbl.mot_freq, x, in_voltage.f32Beta, in_voltage.f32Alpha, pwm_3ph_ptr->pwm_a, pwm_3ph_ptr->pwm_b, pwm_3ph_ptr->pwm_c );
}

/*-------------------------------------------------------------------------------------------------------------
  Инициализация PWM
-------------------------------------------------------------------------------------------------------------*/
void MC_init_PWM(void)
{

  mc_cbl.up_accel_pwm_scale   = 0.6;
  mc_cbl.up_pwm_scale         = 0.65;
  mc_cbl.up_decel_pwm_scale   = 0.6;

  mc_cbl.down_accel_pwm_scale = 0.3;
  mc_cbl.down_pwm_scale       = 0.3;
  mc_cbl.down_decel_pwm_scale = 0.6;

  mc_cbl.up_move_freq         = 50;
  mc_cbl.up_acceler_time      = 10;
  mc_cbl.up_deceler_time      = 10;

  mc_cbl.down_move_freq       = 50;
  mc_cbl.down_acceler_time    = 10;
  mc_cbl.down_deceler_time    = 10;


  _int_install_isr(INT_FTM0, ETM0_isr, &mc_cbl);
  // Разрешить прерывание только после установки вектора! Иначе можем уйти в непрерывный вызов ISR по дефолтному вектору
  _bsp_int_init(INT_FTM0, ETM0_ISR_PRIO, 0, TRUE);

  // Разрешаем тактирование ETM0
  SIM_SCGC6 |= BIT(24);
  dummy =  FTM0_SC;

  // Остановка таймера и очистка флага прерывания
  FTM0_SC = 0
            + LSHIFT(1, 5) // CPWMS. 1 -FTM counter operates in up-down counting mode.
  ;

  // Очистка флагов прерываний в каждом канале
  dummy = FTM0_C0SC;
  dummy = FTM0_C1SC;
  dummy = FTM0_C2SC;
  dummy = FTM0_C3SC;
  dummy = FTM0_C4SC;
  dummy = FTM0_C5SC;
  FTM0_C0SC = 0x00U;
  FTM0_C1SC = 0x00U;
  FTM0_C2SC = 0x00U;
  FTM0_C3SC = 0x00U;
  FTM0_C4SC = 0x00U;
  FTM0_C5SC = 0x00U;
  FTM0_C6SC = 0x00U;
  FTM0_C7SC = 0x00U;

  FTM0_MODE |= BIT(2);  // WPDIS. 1 -Write protection is disabled.
  FTM0_MODE |= BIT(0);  // FTMEN. 1 -All registers including the FTM-specific registers (second set of registers) are available for use with no restrictions.

  FTM0_MOD      = PWM_MODULO; // Установка регистра перезагрузки. Частота PWM в режиме UP-DOWN будет равна Fsys/2*MOD = 60000000/2*1875 =   16000000
  FTM0_CNTIN    = 0;    // Начальное значение счетчика
  FTM0_OUTINIT  = BIT(5) + BIT(3) + BIT(1); // Начальное состояние выходов
  FTM0_CNT      = 0;    // Запись в регистр счетчка любого значения приводит к записи значения из CNTIN и установке начального состояния выходов
  FTM0_DEADTIME = 0;    // Установка мертвого времени в 0 тактов системной частоты (Драйвер FSBB30CH60CT имеет свои задержки мертвого времени)


  FTM0_COMBINE = 0
                 + LSHIFT(0, 30) // FAULTEN3. 1 The fault control in this pair of channels is enabled.
                 + LSHIFT(0, 29) // SYNCEN3.  1 The PWM synchronization in this pair of channels is enabled.
                 + LSHIFT(0, 28) // DTEN3.    0 The deadtime insertion in this pair of channels is disabled.
                 + LSHIFT(0, 27) // DECAP3.   0 The dual edge captures are inactive.
                 + LSHIFT(0, 26) // DECAPEN3  0 The dual edge capture mode in this pair of channels is disabled.
                 + LSHIFT(0, 25) // COMP3     1 The channel (n+1) output is the complement of the channel (n) output.
                 + LSHIFT(0, 24) // COMBINE3  1 Channels (n) and (n+1) are combined.
                 + LSHIFT(1, 22) // FAULTEN2  1 The fault control in this pair of channels is enabled.
                 + LSHIFT(1, 21) // SYNCEN2   1 The PWM synchronization in this pair of channels is enabled.
                 + LSHIFT(1, 20) // DTEN2     0 The deadtime insertion in this pair of channels is disabled.
                 + LSHIFT(0, 19) // DECAP2    0 The dual edge captures are inactive.
                 + LSHIFT(0, 18) // DECAPEN2  0 The dual edge capture mode in this pair of channels is disabled.
                 + LSHIFT(1, 17) // COMP2     1 The channel (n+1) output is the complement of the channel (n) output.
                 + LSHIFT(0, 16) // COMBINE2  1 Channels (n) and (n+1) are combined.
                 + LSHIFT(1, 14) // FAULTEN1  1 The fault control in this pair of channels is enabled.
                 + LSHIFT(1, 13) // SYNCEN1   1 The PWM synchronization in this pair of channels is enabled.
                 + LSHIFT(1, 12) // DTEN1     0 The deadtime insertion in this pair of channels is disabled.
                 + LSHIFT(0, 11) // DECAP1    0 The dual edge captures are inactive.
                 + LSHIFT(0, 10) // DECAPEN1  0 The dual edge capture mode in this pair of channels is disabled.
                 + LSHIFT(1,  9) // COMP1     1 The channel (n+1) output is the complement of the channel (n) output.
                 + LSHIFT(0,  8) // COMBINE1  1 Channels (n) and (n+1) are combined.
                 + LSHIFT(1,  6) // FAULTEN0  1 The fault control in this pair of channels is enabled.
                 + LSHIFT(1,  5) // SYNCEN0   1 The PWM synchronization in this pair of channels is enabled.
                 + LSHIFT(1,  4) // DTEN0     0 The deadtime insertion in this pair of channels is disabled.
                 + LSHIFT(0,  3) // DECAP0    0 The dual edge captures are inactive.
                 + LSHIFT(0,  2) // DECAPEN0  0 The dual edge capture mode in this pair of channels is disabled.
                 + LSHIFT(1,  1) // COMP0     1 The channel (n+1) output is the complement of the channel (n) output.
                 + LSHIFT(0,  0) // COMBINE0  1 Channels (n) and (n+1) are combined.
  ;

  FTM0_INVCTRL = 0; // Инверсия в каналах отключена

  FTM0_C0SC = 0
              + LSHIFT(0, 6) // CHIE. 0 Disable channel interrupts. Use software polling.
              + LSHIFT(1, 3) // ELSB. Edge or Level Select. ELSB=1,ELSA=0 - установка низкого уровня на входе при совпадении
              + LSHIFT(1, 2) // ELSA. Edge or Level Select  ELSB=1,ELSA=1 - установка высокого уровня на входе при совпадении
              + LSHIFT(0, 0) // DMA.  0 Disable DMA transfers.
  ;
  FTM0_C1SC = 0
              + LSHIFT(0, 6) // CHIE. 0 Disable channel interrupts. Use software polling.
              + LSHIFT(1, 3) // ELSB. Edge or Level Select
              + LSHIFT(1, 2) // ELSA. Edge or Level Select
              + LSHIFT(0, 0) // DMA.  0 Disable DMA transfers.
  ;
  FTM0_C2SC = 0
              + LSHIFT(0, 6) // CHIE. 0 Disable channel interrupts. Use software polling.
              + LSHIFT(1, 3) // ELSB. Edge or Level Select
              + LSHIFT(1, 2) // ELSA. Edge or Level Select
              + LSHIFT(0, 0) // DMA.  0 Disable DMA transfers.
  ;
  FTM0_C3SC = 0
              + LSHIFT(0, 6) // CHIE. 0 Disable channel interrupts. Use software polling.
              + LSHIFT(1, 3) // ELSB. Edge or Level Select
              + LSHIFT(1, 2) // ELSA. Edge or Level Select
              + LSHIFT(0, 0) // DMA.  0 Disable DMA transfers.
  ;
  FTM0_C4SC = 0
              + LSHIFT(0, 6) // CHIE. 0 Disable channel interrupts. Use software polling.
              + LSHIFT(1, 3) // ELSB. Edge or Level Select
              + LSHIFT(1, 2) // ELSA. Edge or Level Select
              + LSHIFT(0, 0) // DMA.  0 Disable DMA transfers.
  ;
  FTM0_C5SC = 0
              + LSHIFT(0, 6) // CHIE. 0 Disable channel interrupts. Use software polling.
              + LSHIFT(1, 3) // ELSB. Edge or Level Select
              + LSHIFT(1, 2) // ELSA. Edge or Level Select
              + LSHIFT(0, 0) // DMA.  0 Disable DMA transfers.
  ;
  FTM0_C6SC = 0;
  FTM0_C7SC = 0;

  FTM0_C0V = 0;
  FTM0_C1V = 0;
  FTM0_C2V = 0;
  FTM0_C3V = 0;
  FTM0_C4V = 0;
  FTM0_C5V = 0;

  FTM0_FILTER  = 0;   // Входные фильтры запрещены

  FTM0_FLTCTRL = 0;   // Подготовим регист к программированию фильтров для входов ошибок
  FTM0_FLTPOL  = 0;   // Не меняем полярности входов ошибок. Активные - в сост. 1
  FTM0_FLTCTRL = 0;   // Фильтры и входы ошибок не используем
  FTM0_SYNC    = 0
                 + LSHIFT(1, 1)  // 1 The maximum loading point is enabled.
                 + LSHIFT(0, 0)  // 1 The minimum loading point is enabled
  ;

  FTM0_EXTTRIG = BIT(6);   // Генерацию тригеров используем для запуска PDB0 который запускает ADC

  FTM0_MODE  = 0
               + LSHIFT(1, 2)     // WPDIS. 1 -Write protection is disabled.
               + LSHIFT(1, 1)     // INIT. When a 1 is written to INIT bit the channels output is initialized according to the state of their corresponding bit in the OUTINIT register.
               + LSHIFT(1, 0)     // FTMEN. 1 -All registers including the FTM-specific registers (second set of registers) are available for use with no restrictions.
  ;
  FTM0_SYNCONF = 0
                 + LSHIFT(0, 20) // HWSOC.      1 A hardware trigger activates the SWOCTRL register synchronization.
                 + LSHIFT(0, 19) // HWINVC.     1 A hardware trigger activates the INVCTRL register synchronization.
                 + LSHIFT(0, 18) // HWOM.       1 A hardware trigger activates the OUTMASK register synchronization.
                 + LSHIFT(0, 17) // HWWRBUF.    1 A hardware trigger activates MOD, CNTIN, and CV registers synchronization.
                 + LSHIFT(0, 16) // HWRSTCNT.   1 A hardware trigger activates the FTM counter synchronization.
                 + LSHIFT(0, 12) // SWSOC.      1 The software trigger activates the SWOCTRL register synchronization.
                 + LSHIFT(0, 11) // SWINVC.     1 The software trigger activates the INVCTRL register synchronization.
                 + LSHIFT(0, 10) // SWOM.       1 The software trigger activates the OUTMASK register synchronization.
                 + LSHIFT(1,  9) // SWWRBUF.    1 The software trigger activates MOD, CNTIN, and CV registers synchronization
                 + LSHIFT(0,  8) // SWRSTCNT.   1 обновления регистров MOD, CNTIN, and CV произойдет сразу после установки флага SWSYNC. Если 0, то в точке загрузки при установленном флаге SWSYNC.
                 + LSHIFT(1,  7) // SYNCMODE.   1 Enhanced PWM synchronization is selected.
                 + LSHIFT(0,  5) // SWOC.       1 SWOCTRL регистр будет обновлен в точке зугрузки при наличии флага синхронизации, если 0 то будет обновлен сразу
                 + LSHIFT(0,  4) // INVC.       1 INVCTRL регистр будет обновлен в точке зугрузки при наличии флага синхронизации, если 0 то будет обновлен сразу
                 + LSHIFT(0,  2) // CNTINC.     1 CNTIN регистр будет обновлен в точке зугрузки при наличии флага синхронизации, если 0 то будет обновлен сразу
                 + LSHIFT(0,  1) // HWTRIGMODE. 1 FTM does not clear the TRIGj bit when the hardware trigger j is detected.
  ;

  FTM0_QDCTRL  = 0;   // Квадратурный декодер не используем

  FTM0_FMS = 0;       // Не защищаем от записи


  FTM0_CONF = 0
              + LSHIFT(0, 10) // GTBEOUT. 0 A global time base signal generation is disabled.
              + LSHIFT(0,  9) // GTBEEN.  0 Use of an external global time base is disabled.
              + LSHIFT(1,  6) // BDMMODE. 01- The channels outputs are forced to their safe value according to POLn bit
              + LSHIFT(0,  0) // NUMTOF.  0 - The TOF bit is set for each counter overflow.
  ;
/*
  Нет необходимости обновлять PWM при событии совпадения счетчика с установкой канала
  FTM0_PWMLOAD = 0
                 + LSHIFT(1, 9) // LDOK. 1 Loading updated values is enabled.
                 + LSHIFT(1, 5) // CH5SEL. 1 Include the channel in the matching process.
                 + LSHIFT(1, 4) // CH4SEL. 1 Include the channel in the matching process.
                 + LSHIFT(1, 3) // CH3SEL. 1 Include the channel in the matching process.
                 + LSHIFT(1, 2) // CH2SEL. 1 Include the channel in the matching process.
                 + LSHIFT(1, 1) // CH1SEL. 1 Include the channel in the matching process.
                 + LSHIFT(1, 0) // CH0SEL. 1 Include the channel in the matching process.
  ;
*/


  FTM0_OUTMASK = BIT(6) + BIT(7); // Установка в 1 бита в этом регистре запрещает работу соответствующего выхода и он устанавливается в состояние 0

  FTM0_SWOCTRL = 0xAAFF; // Запись в регистр слова 0x0000 позволяет выходам работать в нормальном режиме PWM
                         // Запись в регистр слова 0x00FF приводит к программной установке нулей на вcех выходах
                         // Запись слова 0xFFFF или 0x55FF приводит к установке в 1 выходов для верхних ключей и в 0 выходов для нижних ключей
                         // Запись слова 0xAAFF приводит к установке в 0 выходов для верхних ключей и в 1 выходов для нижних ключей

  // Запускаем PWM, выхода остаються в запрограммированом статическом состоянии
  FTM0_SC = 0
            + LSHIFT(0, 6) // TOIE. 1 Enable TOF interrupts. An interrupt is generated when TOF equals one.
            + LSHIFT(1, 5) // CPWMS. 1 -FTM counter operates in up-down counting mode.
            + LSHIFT(1, 3) // CLKS. 01 System clock
            + LSHIFT(0, 0) // PS. Prescale Factor Selection. 000 Divide by 1
  ;

}


/*-------------------------------------------------------------------------------------------------------------
  Старт работы PWM
-------------------------------------------------------------------------------------------------------------*/
void PWM_start(int dir, unsigned int freq)
{
  T_3ph_pwm pwm_3ph;

  mc_cbl.mot_freq  = freq;
  mc_cbl.direction = dir;

  if ( dir == MOVING_UP )
  {
    mc_cbl.pwm_scale = mc_cbl.up_accel_pwm_scale;
  }
  else
  {
    mc_cbl.pwm_scale = mc_cbl.down_accel_pwm_scale;
  }

  mc_cbl.ll_mot_freq = (unsigned long long)mc_cbl.mot_freq << 32;
  Gen_start(mc_cbl.mot_freq);
  MC_calculate_PWM(&pwm_3ph);
  FTM0_C0V = pwm_3ph.pwm_a;
  FTM0_C1V = pwm_3ph.pwm_a;
  FTM0_C2V = pwm_3ph.pwm_b;
  FTM0_C3V = pwm_3ph.pwm_b;
  FTM0_C4V = pwm_3ph.pwm_c;
  FTM0_C5V = pwm_3ph.pwm_c;

  _int_disable();
  FTM0_SYNCONF |= LSHIFT(1,  8); // Выставляем флаг для немедленного обновления регистров по флагу синхронизации
  FTM0_SYNC    |= BIT(7);        // Запускаем синхронизацию
  FTM0_CNT      = 0;             // Запись в регистр счетчка любого значения приводит к записи значения из CNTIN и установке начального состояния выходов
  FTM0_SYNCONF &= ~LSHIFT(1,  8); // Снимаем флаг немедленного обновления регистров по флагу синхронизации
  FTM0_SWOCTRL  = 0x0000;        // Запись в регистр слова 0x0000 позволяет выходам работать в нормальном режиме PWM
  FTM0_SC      &= ~BIT(7);       // Сбросим TOF
  FTM0_SC      |= BIT(6);        // Разрешаем прерывания от PWM
  _int_enable();
}

/*-------------------------------------------------------------------------------------------------------------
  Остановка работы PWM 
-------------------------------------------------------------------------------------------------------------*/
void PWM_stop(void)
{
  FTM0_SC &= ~BIT(6);    // Запрещаем прерывания от PWM
  FTM0_SWOCTRL = 0xAAFF; // Запись слова 0xAAFF приводит к установке в 0 выходов для верхних ключей и в 1 выходов для нижних ключей
}

/*-------------------------------------------------------------------------------------------------------------
 Возвращает состояние PWM
 0 - запрещен
 1 - разрешен
-------------------------------------------------------------------------------------------------------------*/
int PWM_state(void)
{
  if ( FTM0_SC & BIT(6) ) return 1;
  else return 0;
}

/*-------------------------------------------------------------------------------------------------------------
  Ожидание любого события  
-------------------------------------------------------------------------------------------------------------*/
_mqx_uint MC_get_events(_mqx_uint *events, _mqx_uint ticks, _mqx_uint events_mask)
{
  _mqx_uint res;
  res = _lwevent_wait_ticks(&evt_grp, events_mask, FALSE, ticks);
  if ( res == MQX_OK )
  {
    *events = _lwevent_get_signalled();
    _lwevent_clear(&evt_grp, *events);
  }
  else
  {
    *events = 0;
  }
  return res;
}

/*-------------------------------------------------------------------------------------------------------------
  Передать событие в модуль управления двигателем  
-------------------------------------------------------------------------------------------------------------*/
_mqx_uint MC_set_events(_mqx_uint evnt)
{
  return _lwevent_set(&evt_grp, evnt);

}
/*-------------------------------------------------------------------------------------------------------------
  Создать объект для обмена событиями 
-------------------------------------------------------------------------------------------------------------*/
void MC_create_event(void)
{
  _lwevent_create(&evt_grp, 0);
}

/*-------------------------------------------------------------------------------------------------------------
  Старт движения
  dir - MOVING_DOWN или MOVING_UP
  target_freq - целевая частота вращения (об/сек)
  time - время (сек) в течении которого частота должна подняться  до target_freq
-------------------------------------------------------------------------------------------------------------*/
void MC_start_motor_moving(INT8U dir,  INT32U target_freq, INT32U time)
{
  // Не обрабатывать команд на пуск пока разорвана цепь безопасности
  if (( PWM_state() == 0 ) && (Pin_PWM_OE_state()!=0))
  {
    PWM_start(dir, START_FREQ);
    MC_init_speed_change(MOT_START_ACTION, target_freq, time);
  }
}
/*-------------------------------------------------------------------------------------------------------------
  Остановка движения
  dir - MOVING_DOWN или MOVING_UP
  time - время (сек) в течении которого частота должна опуститься до 0
-------------------------------------------------------------------------------------------------------------*/
void MC_stop_motor_moving(INT32U time)
{
  if ( PWM_state() != 0 )
  {
    MC_init_speed_change(MOT_STOP_ACTION, 0, time);
  }
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void MC_emergency_stop_motor(void)
{
  PWM_stop();
  _int_disable();
  mc_cbl.mot_freq    = 0;
  mc_cbl.ll_mot_freq = 0;
  mc_cbl.skew_cnt    = 0;
  mc_cbl.ll_step     = 0;
  mc_cbl.action      = MOT_IDLE;
  _int_enable();
  Reset_aver_curr(); 
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void MC_get_CBL(T_MC_CBL *cbl_ptr)
{
  _int_disable();
  memcpy(cbl_ptr, &mc_cbl, sizeof(mc_cbl));
  _int_enable();
}
/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void MC_set_CBL(T_MC_CBL *cbl_ptr)
{
  _int_disable();
  memcpy(&mc_cbl, cbl_ptr, sizeof(mc_cbl));
  _int_enable();
}
/*-------------------------------------------------------------------------------------------------------------
  action - STOP_ACTION, START_ACTION 
  target_freq - частотота вращение к которой должен быть приведен двигатель
  target_time - время за которое должно произойти изменение частоты вращения, задается в десятых долях секунды
-------------------------------------------------------------------------------------------------------------*/
void MC_init_speed_change(unsigned int action,  unsigned int target_freq, unsigned int target_time)
{
  T_MC_CBL          cbl;
  float             t;
  Frac32            x;
  unsigned int      current_freq;

  MC_get_CBL(&cbl);

  if ( action == MOT_START_ACTION )
  {
    cbl.action = MOT_START_ACTION;
  }
  else if ( action == MOT_STOP_ACTION )
  {
    cbl.action = MOT_STOP_ACTION;
  }

  current_freq = cbl.mot_freq;
  if ( target_freq != current_freq  )
  {
    if ( target_time != 0 )
    {
      if ( (cbl.direction == MOVING_UP) && (action == MOT_STOP_ACTION) )
      {
        // Торможение при движении вверх
        if ( Get_aver_curr() < 9.0 )
        {
          target_freq = 5; 
        }
        else
        {
          target_freq = 15; // Повышаем конечную скорость остановки в случае большого тока 
        }
      }
      if ( (cbl.direction == MOVING_DOWN) && (action == MOT_STOP_ACTION) )
      {
        // Торможение при движении вниз
        if ( target_time > 5 )
        {
          target_time = target_time ; // Убавляем время торможения
        }
      }

      t = (float)(target_time / 10.0);
      t = t * PWM_FREQ;
      cbl.skew_cnt = (unsigned int)t;
      t = ((float)target_freq - (float)current_freq) * 0x100000000 / t;
      cbl.ll_step = (signed long long)t;
    }
    else
    {
      cbl.skew_cnt = 1;
      cbl.ll_step  = (target_freq - current_freq) * 0x100000000;
    }


    // Расчитаем дельту коэффициента масштабирования PWM при переходе от равномерного движения к ускоренному
    // Переход расчитываем за заданное в макросе PWM_SCALE_TRASITION_TIME время
    // Определим тип перехода:
    // - Ускорение при движении  вверх : увеличиваем коэффициент PWM до максимума
    // - Замедление при движении вверх : увеличиваем коэффициент PWM до максимума
    // - Ускорение при движении  вниз  : коэффициент PWM до максимума не на многобольше чем при равномерном движении
    // - Замедление при движении вниз  : увеличиваем коэффициент PWM до максимума

    if ( cbl.direction == MOVING_UP )
    {
      if ( action == MOT_START_ACTION )
      {
        // - Ускорение при движении  вверх
        cbl.pwm_scale_delta = (cbl.up_pwm_scale - cbl.pwm_scale) / (PWM_SCALE_TRASITION_TIME * PWM_FREQ);
        cbl.pwm_scale_cnt = PWM_SCALE_TRASITION_TIME * PWM_FREQ;
      }
      else if ( action == MOT_STOP_ACTION  )
      {
        // - Замедление при движении вверх
        cbl.pwm_scale_delta = (cbl.up_decel_pwm_scale - cbl.pwm_scale) / (PWM_SCALE_TRASITION_TIME * PWM_FREQ);
        cbl.pwm_scale_cnt = PWM_SCALE_TRASITION_TIME * PWM_FREQ;
      }
    }
    else
    {
      if ( action == MOT_START_ACTION  )
      {
        // - Ускорение при движении  вниз
        cbl.pwm_scale_delta = (cbl.down_pwm_scale - cbl.pwm_scale) / (PWM_SCALE_TRASITION_TIME * PWM_FREQ);
        cbl.pwm_scale_cnt = PWM_SCALE_TRASITION_TIME * PWM_FREQ;

      }
      else if ( action == MOT_STOP_ACTION  )
      {
        // - Замедление при движении вниз
        cbl.pwm_scale_delta = (cbl.down_decel_pwm_scale - cbl.pwm_scale) / (PWM_SCALE_TRASITION_TIME * PWM_FREQ);
        cbl.pwm_scale_cnt = PWM_SCALE_TRASITION_TIME * PWM_FREQ;
      }
    }

    MC_set_CBL(&cbl);
  }

  Reset_aver_curr(); // Сбрасываем средний измеренный ток за прошлый период


}


/*-------------------------------------------------------------------------------------------------------------
  Процедура обслуживания прерывания PWM
  Процедура активизирует задачу Motor_ISR_task поскольку в самой процедуре ISR нельзя использовать вычисления с плавающей точкой
-------------------------------------------------------------------------------------------------------------*/
static void ETM0_isr(pointer user_isr_ptr)
{
  Led_control(LED3, 1);

  if ( FTM0_SC & BIT(7) )
  {
    FTM0_SC &= ~BIT(7); // Сбросим TOF
  }
  FTM0_EXTTRIG &= ~BIT(7); // Сбросим триггер идущий к PDB

  _taskq_resume(mc_is_taskr_queue, FALSE);

  Led_control(LED3, 0);
}

/*-------------------------------------------------------------------------------------------------------------
  Задача инициируемая прерываниями IBV модулятора для пересчета текущих параметров ШИМ 
-------------------------------------------------------------------------------------------------------------*/
void      Motor_ISR_task(uint_32 initial_data)
{
  T_3ph_pwm                      pwm_3ph;

  mc_is_taskr_queue = _taskq_create(MQX_TASK_QUEUE_FIFO);
  if ( mc_is_taskr_queue == NULL )
  {
    _mqx_exit(0);
  }

  for (;;)
  {
    _taskq_suspend(mc_is_taskr_queue);

    Led_control(LED3, 1);

    MC_calculate_PWM(&pwm_3ph);
    FTM0_C0V = pwm_3ph.pwm_a;
    FTM0_C1V = pwm_3ph.pwm_a;
    FTM0_C2V = pwm_3ph.pwm_b;
    FTM0_C3V = pwm_3ph.pwm_b;
    FTM0_C4V = pwm_3ph.pwm_c;
    FTM0_C5V = pwm_3ph.pwm_c;
    FTM0_SYNC |= BIT(7);   //

    if ( arr_pos < TST_SMPLS_ARR_SZ )
    {
      smpls[8][arr_pos] = pwm_3ph.pwm_a;
      smpls[9][arr_pos] = pwm_3ph.pwm_b;
      smpls[10][arr_pos] = pwm_3ph.pwm_c;
    }

    // Изменение скорости вращения задается счетчиком и шагом
    if ( mc_cbl.skew_cnt != 0 )
    {
      mc_cbl.ll_mot_freq = mc_cbl.ll_mot_freq + mc_cbl.ll_step;       // Добавляем шаг
      mc_cbl.mot_freq = (mc_cbl.ll_mot_freq + 0x80000000ll) >> 32;    // Приводим к 32-х битному целому c округлением
      Gen_update_freq(mc_cbl.mot_freq);
      mc_cbl.skew_cnt--;
      if ( mc_cbl.skew_cnt == 0 )
      {
        // Фиксируем переход от ускорения к  равномерному движению.
        // Здесь определяем максимальный действующий ток в прошедшей фазе движения и корректируем масштабирование PWM для последующего движения
        if ( (mc_cbl.direction == MOVING_UP) && (mc_cbl.action == MOT_START_ACTION) )
        {
          if ( Get_aver_curr() < 9.0 )
          {
            // Корректируем масштабирование PWM до 0.5 поскольку вес не большой
            mc_cbl.pwm_scale_delta = (0.5 - mc_cbl.pwm_scale) / (PWM_SCALE_TRASITION_TIME * PWM_FREQ);
            mc_cbl.pwm_scale_cnt = PWM_SCALE_TRASITION_TIME * PWM_FREQ;
          }
          Reset_aver_curr();
        }
        if (  mc_cbl.action == MOT_STOP_ACTION ) // Если это было не торможение, то это равномерное движение
        {
          MC_emergency_stop_motor();
        }
        mc_cbl.action = MOT_UNIFORM_MOTION;
      }
    }

    // Плавно изменяем коэффициент масштабирования PWM
    if ( mc_cbl.pwm_scale_cnt != 0 )
    {
      mc_cbl.pwm_scale += mc_cbl.pwm_scale_delta;
      mc_cbl.pwm_scale_cnt--;
    }


    // Останавливаем движение если скрость снизилась до минимальной
    if ( mc_cbl.mot_freq <= MIN_FREQ )
    {
      MC_emergency_stop_motor();
      
    }
    Led_control(LED3, 0);
  }
}

T_MC_CBL* MC_get_pcbl(void)
{
  return &mc_cbl;
}
