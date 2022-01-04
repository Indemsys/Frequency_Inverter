#include "App.h"



static T_ADC_state adc_state;
static T_ADC_res   adc_res;

signed short   smpls[MEAS_RES_ARR_SZ][TST_SMPLS_ARR_SZ];
unsigned int   arr_pos;


/*-------------------------------------------------------------------------------------------------------------
  Обработчик прерывания PDB (Программируемой задержки синхронизирующей PWM и ADC)
  Принимает результаты ADC
-------------------------------------------------------------------------------------------------------------*/
static void PDB0_isr(pointer user_isr_ptr)
{
  Led_control(LED2, 1);
  PDB0_SC &= ~(BIT(7) + BIT(6));
  PDB0_CH0S = 0;
  PDB0_CH1S = 0;
  PDB0_CH2S = 0;
  PDB0_CH3S = 0;
  PDB0_SC |= BIT(7);

  adc_res.smpl_ii_w    = ADC0_RA;
  adc_res.smpl_v_bus   = ADC0_RB;
  adc_res.smpl_ii_v    = ADC1_RA;
  adc_res.smpl_temper  = ADC1_RB;
  adc_res.smpl_ii_u    = ADC2_RA;
  adc_res.smpl_v_u     = ADC2_RB;
  adc_res.smpl_15v     = ADC3_RA;
  adc_res.smpl_5v      = ADC3_RB;
  
  // Фильтруем токи от постоянной составляющей
  adc_res.ii_w = adc_res.smpl_ii_w - (adc_res.ii_w_offs >> 12);
  adc_res.ii_w_offs += adc_res.ii_w;       

  adc_res.ii_v = adc_res.smpl_ii_v - (adc_res.ii_v_offs >> 12);
  adc_res.ii_v_offs += adc_res.ii_v;       

  adc_res.ii_u = adc_res.smpl_ii_u - (adc_res.ii_u_offs >> 12);
  adc_res.ii_u_offs += adc_res.ii_u;       

  // Фильтруем напряжения с помощью бегущего среднего на 16 отсчетов
  adc_res.v_bus_acc -= adc_res.v_bus_arr[adc_res.aai]; 
  adc_res.v_bus_arr[adc_res.aai] = adc_res.smpl_v_bus;
  adc_res.v_bus_acc += adc_res.v_bus_arr[adc_res.aai];
  adc_res.aai++;
  if ( adc_res.aai >= FILTR_AVER_SZ ) adc_res.aai = 0;
  adc_res.v_bus = adc_res.v_bus_acc/FILTR_AVER_SZ;

 

  // Запись отсчетов в тестовый массив
  if ( arr_pos < TST_SMPLS_ARR_SZ )
  {
/*
    smpls[0][arr_pos] = adc_res.smpl_ii_w   ;
    smpls[1][arr_pos] = adc_res.smpl_v_bus  ;
    smpls[2][arr_pos] = adc_res.smpl_ii_v   ;
    smpls[3][arr_pos] = adc_res.smpl_temper ; 
    smpls[4][arr_pos] = adc_res.smpl_ii_u   ; 
    smpls[5][arr_pos] = adc_res.smpl_v_u    ; 
    smpls[6][arr_pos] = adc_res.smpl_15v    ; 
    smpls[7][arr_pos] = adc_res.smpl_5v     ;
*/ 
    smpls[0][arr_pos] = adc_res.ii_w        ;
    smpls[1][arr_pos] = adc_res.ii_v        ;
    smpls[2][arr_pos] = adc_res.ii_u        ;
    smpls[3][arr_pos] = adc_res.v_bus       ; 
    smpls[4][arr_pos] = adc_res.smpl_temper ; 
    smpls[5][arr_pos] = adc_res.smpl_15v    ; 
    smpls[6][arr_pos] = adc_res.smpl_5v     ; 
    smpls[7][arr_pos] = adc_res.smpl_ii_u   ;

    arr_pos++;
    if ( arr_pos == (TST_SMPLS_ARR_SZ/2) )
    {
      MC_set_events(SMPL_ARR1_FULL);
    }
    else if ( arr_pos == TST_SMPLS_ARR_SZ )
    {
      MC_set_events(SMPL_ARR2_FULL);
      arr_pos = 0;
    }
  }
  Led_control(LED2, 0);
}


/*-------------------------------------------------------------------------------------------------------------
 Включаем тактирование всех ADC
-------------------------------------------------------------------------------------------------------------*/
void ADC_switch_on_all(void)
{
  SIM_SCGC3 |= BIT(28); // Тактирование ADC3
  SIM_SCGC3 |= BIT(27); // Тактирование ADC1
  SIM_SCGC6 |= BIT(28); // Тактирование ADC2
  SIM_SCGC6 |= BIT(27); // Тактирование ADC0
}

/*-------------------------------------------------------------------------------------------------------------
  Первоначальная инициализация ADC 
 
 
  // Самое короткое время преобразования
  // ADLSMP = 0
  // ADICLK = 0
  // 3 ADCK cycles + 5 bus clock cycles

  // 13-и битное дифференциальное
  // 30 ADCK cycles

  // ADLSMP = 0
  // 0 ADCK cycles

  // ADHSC = 0
  // 0 ADCK cycles

  // В сумме: 3 ADCK cycles + 5 bus clock cycles +  30 ADCK cycles + 0 ADCK cycles + 0 ADCK cycles = 33*4 + 5 = 137 тактов шины = 2.28 мкс

-------------------------------------------------------------------------------------------------------------*/
void ADC_init(ADC_MemMapPtr ADC)
{
  // Выбор частоты тактирования, разрядность результата
  // Частота тактирования для 16-и битного результата не должна быть больше 12 МГц, для 13-и битного 18 МГц
  // Устанавливаем 15 МГц
  ADC->CFG1 = 0
              + LSHIFT(0, 7) // ADLPC.  Low power configuration. The power is reduced at the expense of maximum clock speed. 0 Normal power configuration
              + LSHIFT(2, 5) // ADIV.   Clock divide select. 00 The divide ratio is 1 and the clock rate is input clock. 10 The divide ratio is 4 and the clock rate is (input clock)/4.
              + LSHIFT(0, 4) // ADLSMP. Sample time configuration. 0 Short sample time.
              + LSHIFT(1, 2) // MODE.   Conversion mode selection. 01 When DIFF=0: It is single-ended 12-bit conversion; when DIFF=1, it is differential 13-bit conversion with 2's complement output.
              + LSHIFT(0, 0) // ADICLK. Input clock select. 00 Bus clock.
  ;
  // Выбор канала между A и B, установка времени выборки
  ADC->CFG2 = 0
              + LSHIFT(0, 4) // MUXSEL.  0 ADxxa channels are selected.
              + LSHIFT(0, 3) // ADACKEN. Asynchronous clock output enable
              + LSHIFT(0, 2) // ADHSC.   High speed configuration. 0 Normal conversion sequence selected.
              + LSHIFT(0, 0) // ADLSTS.  Long sample time select
  ;

  // Регистр статуса и управления. Выбор типа тригера, управление функцией сравнения, разрешение DMA, выбор типа опоры
  ADC->SC2 = 0
             + LSHIFT(0, 7) // ADACT.   Read only. 1 Conversion in progress.
             + LSHIFT(0, 6) // ADTRG.   Conversion trigger select. 0 Software trigger selected.
             + LSHIFT(0, 5) // ACFE.    Compare function enable. 0 Compare function disabled.
             + LSHIFT(0, 4) // ACFGT.   Compare function greater than enable
             + LSHIFT(0, 3) // ACREN.   Compare function range enable
             + LSHIFT(0, 2) // DMAEN.   DMA enable
             + LSHIFT(0, 0) // REFSEL.  Voltage reference selection. 00 Default voltage reference pin pair (external pins VREFH and VREFL)
  ;
  // Регистр статуса и управления. Управление и статус калибровки, управление усреднением, управление непрерывным преобразованием
  ADC->SC3 = 0
             + LSHIFT(0, 7) // CAL.     CAL begins the calibration sequence when set.
             + LSHIFT(0, 3) // ADCO.    Continuous conversion enable
             + LSHIFT(0, 2) // AVGE.
             + LSHIFT(0, 0) // AVGS.
  ;

  // Регистры значений для функции сравнения
  //ADC->CV1 = 0;
  //ADC->CV2 = 0;

  // Регист корректировки смещения
  //ADC->OFS = 0;

  // Регистры корректировки усиления для положительной и отрицательной сторон дифференциального преобразования в формате с фиксированной точкой 15,14
  // Записываються вручную
  //ADC->PG = 0x8200;
  //ADC->MG = 0x8200;


  ADC->PGA = 0
             + LSHIFT(0, 23) // PGAEN.   1 PGA enabled.
             + LSHIFT(0, 21) // PGACHPb. 1 Chopping disabled.
             + LSHIFT(0, 20) // PGALPb.  1 PGA runs in normal power mode.
             + LSHIFT(0, 16) // PGAG     PGA gain setting. PGA gain = 2^(PGAG)
             + LSHIFT(0, 14) // PGAOFSM  1 PGA runs in offset measurement mode.
  ;


  // Контрольный регистр B
  // Результат управляемый этим регистром будет находиться в регистре ADC->RB
  // Используеться только при включенном аппаратном триггере
  ADC->SC1[1] = 0
                + LSHIFT(0, 7) // COCO. Read only. 1 Conversion completed.
                + LSHIFT(0, 6) // AIEN. 1 Conversion complete interrupt enabled.
                + LSHIFT(1, 5) // DIFF. 1 Differential conversions and input channels are selected.
                + LSHIFT(0, 0) // ADCH. Input channel select. 11111 Module disabled.
                                //                             00000 When DIFF=0, DADP0 is selected as input; when DIFF=1, DAD0 is selected as input.
  ;
  // Контрольный регистр A
  // Результат управляемый этим регистром будет находиться в регистре ADC0_RA
  // Запись в данный регист либо прерывает текущее преобразование либо инициирует новое (если установлен флаг софтварныого тригера - ADTRG = 0)
  ADC->SC1[0] = 0
                + LSHIFT(0, 7) // COCO. Read only. 1 Conversion completed.
                + LSHIFT(0, 6) // AIEN. 1 Conversion complete interrupt enabled.
                + LSHIFT(0, 5) // DIFF. 1 Differential conversions and input channels are selected.
                + LSHIFT(16, 0) // ADCH. Input channel select. 11111 Module disabled.
                               //                             00000 When DIFF=0, DADP0 is selected as input; when DIFF=1, DAD0 is selected as input.
  ;

}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
int ADC_calibrating(ADC_MemMapPtr ADC)
{

  unsigned short tmp;

  ADC->SC1[0] = 0
                + LSHIFT(0, 7) // COCO. Read only. 1 Conversion completed.
                + LSHIFT(0, 6) // AIEN. 1 Conversion complete interrupt enabled.
                + LSHIFT(1, 5) // DIFF. 1 Differential conversions and input channels are selected.
                + LSHIFT(0, 0) // ADCH. Input channel select. 11111 Module disabled.
                               //                             00000 When DIFF=0, DADP0 is selected as input; when DIFF=1, DAD0 is selected as input.
  ;
  // Устанавливаем частоту ADC 3.75 MHz
  ADC->CFG1 = 0
              + LSHIFT(0, 7) // ADLPC.  Low power configuration. The power is reduced at the expense of maximum clock speed. 0 Normal power configuration
              + LSHIFT(3, 5) // ADIV.   Clock divide select. 11 - The divide ratio is 8 and the clock rate is (input clock)/8.
              + LSHIFT(0, 4) // ADLSMP. Sample time configuration. 0 Short sample time.
              + LSHIFT(1, 2) // MODE.   Conversion mode selection. 01 When DIFF=0: It is single-ended 12-bit conversion; when DIFF=1, it is differential 13-bit conversion with 2's complement output.
              + LSHIFT(1, 0) // ADICLK. Input clock select. 00 Bus clock.
  ;
  ADC->CFG2 = 0
              + LSHIFT(0, 4) // MUXSEL.  0 ADxxa channels are selected.
              + LSHIFT(0, 3) // ADACKEN. Asynchronous clock output enable
              + LSHIFT(0, 2) // ADHSC.   High speed configuration. 0 Normal conversion sequence selected.
              + LSHIFT(0, 0) // ADLSTS.  Long sample time select
  ;
  // Регистр статуса и управления. Выбор типа тригера, управление функцией сравнения, разрешение DMA, выбор типа опоры
  ADC->SC2 = 0
             + LSHIFT(0, 7) // ADACT.   Read only. 1 Conversion in progress.
             + LSHIFT(0, 6) // ADTRG.   Conversion trigger select. 0 Software trigger selected.
             + LSHIFT(0, 5) // ACFE.    Compare function enable. 0 Compare function disabled.
             + LSHIFT(0, 4) // ACFGT.   Compare function greater than enable
             + LSHIFT(0, 3) // ACREN.   Compare function range enable
             + LSHIFT(0, 2) // DMAEN.   DMA enable
             + LSHIFT(0, 0) // REFSEL.  Voltage reference selection. 00 Default voltage reference pin pair (external pins VREFH and VREFL)
  ;
  ADC->SC3 = 0
             + LSHIFT(1, 7) // CAL.     CAL begins the calibration sequence when set.
             + LSHIFT(1, 6) // CALF.    Read Only. 1 Calibration failed.
             + LSHIFT(0, 3) // ADCO.    Continuous conversion enable
             + LSHIFT(1, 2) // AVGE.    1 Hardware average function enabled.
             + LSHIFT(3, 0) // AVGS.    11 32 samples averaged.
  ;

  // Ожидать завершения калибровки
  while (ADC->SC3 & BIT(7));

  tmp = (ADC->CLP0 + ADC->CLP1 + ADC->CLP2 + ADC->CLP3 + ADC->CLP4 + ADC->CLPS) / 2;
  ADC->PG = tmp | 0x8000;

  tmp = (ADC->CLM0 + ADC->CLM1 + ADC->CLM2 + ADC->CLM3 + ADC->CLM4 + ADC->CLMS) / 2;
  ADC->MG = tmp | 0x8000;

  if ( ADC->SC3 & BIT(6) )
  {
    return MQX_ERROR;
  }
  else
  {
    return MQX_OK;
  }
}

/*-------------------------------------------------------------------------------------------------------------
  Установить номер канала и признак дифференциальнф-простой для указанного регистра управления у ADC  
-------------------------------------------------------------------------------------------------------------*/
void ADC_set_channel(ADC_MemMapPtr ADC,int ch, int diff, int reg)
{
  reg  = reg & 1;
  diff = diff & 1;
  ch   = ch & 0x1F;

  ADC->SC1[reg] = 0
                + LSHIFT(0, 7)    // COCO. Read only. 1 Conversion completed.
                + LSHIFT(0, 6)    // AIEN. 1 Conversion complete interrupt enabled.
                + LSHIFT(diff, 5) // DIFF. 1 Differential conversions and input channels are selected.
                + LSHIFT(ch, 0)   // ADCH. Input channel select. 11111 Module disabled.
                                  //                             00000 When DIFF=0, DADP0 is selected as input; when DIFF=1, DAD0 is selected as input.
;
}

/*-------------------------------------------------------------------------------------------------------------
  Активировать хардварный триггер 
-------------------------------------------------------------------------------------------------------------*/
void ADC_activate_hrdw_trig(ADC_MemMapPtr ADC)
{
  ADC->SC2 |= BIT(6); // ADTRG.   Conversion trigger select. 0 Software trigger selected.
}

/*-------------------------------------------------------------------------------------------------------------
  Активировать софтварный триггер 
-------------------------------------------------------------------------------------------------------------*/
void ADC_activate_soft_trig(ADC_MemMapPtr ADC)
{
  ADC->SC2 &= ~BIT(6); // ADTRG.   Conversion trigger select. 0 Software trigger selected.
}


/*-------------------------------------------------------------------------------------------------------------
  Конфигурировать триггеры для всех ADC   
 
 
 Синхронизируем так чтобы измерения проводить когда каналы T0C0=0, T0C1=1, T0C2=0, T0C3=1, T0C4=0, T0C5=1
 В это время все нижние ключи открыты 
 У нас T0C0, T0C2, T0C4 принимают значение 0 когда счетчик не дошел до компаратора (FTM0_CxSC.ELSB=1,FTM0_CxSC.ELSA=1)
 Обновление компараторов надо делать в верхней точке, чтобы измерение не приходилось на фронт или спад импульсов 
-------------------------------------------------------------------------------------------------------------*/
void PDB_configure_ADC_triggers(void)
{
  SIM_SCGC6 |= BIT(22); // Разрешаем тактирование PDB
  SIM_SOPT7 = 0;        // Триггерами для всех ADC служит PDB


  // Первый претриггер канала работает без задержки, второй с задержкой на время преобразования ADC
  PDB0_CH0C1 = 0
               + LSHIFT(2, 8) // Установка битов подключения претриггеров к задержкам. 0-й претригер в байпасе, 1-й претрегер после задержки
               + LSHIFT(3, 0) // Установка битов разрешения выходов претриггеров
  ;
  PDB0_CH1C1 = 0
               + LSHIFT(2, 8) // Установка битов подключения претриггеров к задержкам
               + LSHIFT(3, 0) // Установка битов разрешения выходов претриггеров
  ;
  PDB0_CH2C1 = 0
               + LSHIFT(2, 8) // Установка битов подключения претриггеров к задержкам
               + LSHIFT(3, 0) // Установка битов разрешения выходов претриггеров
  ;
  PDB0_CH3C1 = 0
               + LSHIFT(2, 8) // Установка битов подключения претриггеров к задержкам
               + LSHIFT(3, 0) // Установка битов разрешения выходов претриггеров
  ;
  PDB0_CH0DLY1 = 138; // 2.3 мкс в тактах системной шины  
  PDB0_CH1DLY1 = 138; // 2.3 мкс в тактах системной шины  
  PDB0_CH2DLY1 = 138; // 2.3 мкс в тактах системной шины  
  PDB0_CH3DLY1 = 138; // 2.3 мкс в тактах системной шины  

  PDB0_IDLY    = 138 + 138; // Время после которого произойдет прерывание PDB0 и можно будет забрать результаты ADC
/*
  // Первый претриггер канала работает без задержки, второй с задержкой на время преобразования ADC
  PDB0_CH0C1 = 0
               + LSHIFT(3, 8) // Установка битов подключения претриггеров к задержкам. 0-й претригер после задержки, 1-й претрегер после задержки
               + LSHIFT(3, 0) // Установка битов разрешения выходов претриггеров
  ;
  PDB0_CH1C1 = 0
               + LSHIFT(3, 8) // Установка битов подключения претриггеров к задержкам
               + LSHIFT(3, 0) // Установка битов разрешения выходов претриггеров
  ;
  PDB0_CH2C1 = 0
               + LSHIFT(3, 8) // Установка битов подключения претриггеров к задержкам
               + LSHIFT(3, 0) // Установка битов разрешения выходов претриггеров
  ;
  PDB0_CH3C1 = 0
               + LSHIFT(3, 8) // Установка битов подключения претриггеров к задержкам
               + LSHIFT(3, 0) // Установка битов разрешения выходов претриггеров
  ;


  #define DELAYC (1) 
  PDB0_CH0DLY0 = DELAYC;//138; // 2.3 мкс в тактах системной шины  
  PDB0_CH1DLY0 = DELAYC;//138; // 2.3 мкс в тактах системной шины  
  PDB0_CH2DLY0 = DELAYC;//138; // 2.3 мкс в тактах системной шины  
  PDB0_CH3DLY0 = DELAYC;//138; // 2.3 мкс в тактах системной шины  

  PDB0_CH0DLY1 = DELAYC + 143;//138; // 2.3 мкс в тактах системной шины  
  PDB0_CH1DLY1 = DELAYC + 143;//138; // 2.3 мкс в тактах системной шины  
  PDB0_CH2DLY1 = DELAYC + 143;//138; // 2.3 мкс в тактах системной шины  
  PDB0_CH3DLY1 = DELAYC + 143;//138; // 2.3 мкс в тактах системной шины  

  PDB0_IDLY    = DELAYC + 143 + 143; // Время после которого произойдет прерывание PDB0 и можно будет забрать результаты ADC
*/


  _int_install_isr(INT_PDB0, PDB0_isr, 0);
  // Разрешить прерывание только после установки вектора! Иначе можем уйти в непрерывный вызов ISR по дефолтному вектору
  _bsp_int_init(INT_PDB0, PDB0_ISR_PRIO, 0, TRUE);


  PDB0_SC = 0
            + LSHIFT(0,   18) // LDMOD.     00 The internal registers are loaded with the values from their buffers immediately after 1 is written to LDOK.
            + LSHIFT(0,   17) // PDBEIE.    0 PDB sequence error interrupt disabled.
            + LSHIFT(0,   16) // SWTRIG.    Software Trigger
            + LSHIFT(0,   15) // DMAEN.     0 DMA disabled
            + LSHIFT(0,   12) // PRESCALER. 000 Counting uses the peripheral clock divided by multiplication factor selected by MULT.
            + LSHIFT(0x8,  8) // TRGSEL.    1000 FTM0 Init and Ext Trigger Outputs
            + LSHIFT(1,    7) // PDBEN.     1 PDB enabled
            + LSHIFT(0,    6) // PDBIF.     PDB Interrupt Flag
            + LSHIFT(1,    5) // PDBIE.     1 PDB interrupt enabled
            + LSHIFT(0,    2) // MULT.      00 Multiplication factor is 1
            + LSHIFT(0,    1) // CONT.      1 PDB operation in Continuous mode
            + LSHIFT(1,    0) // LDOK.      Writing 1 to this bit updates the internal registers
  ;
}

/*-------------------------------------------------------------------------------------------------------------
  
-------------------------------------------------------------------------------------------------------------*/
void PDB_set_delays(int delay1, int delay2)
{
  PDB0_SC &= ~(BIT(7) + BIT(6));
  PDB0_CH0S = 0;
  PDB0_CH1S = 0;
  PDB0_CH2S = 0;
  PDB0_CH3S = 0;

  PDB0_CH0DLY1 = delay1;  
  PDB0_CH1DLY1 = delay1;  
  PDB0_CH2DLY1 = delay1;  
  PDB0_CH3DLY1 = delay1;  

  PDB0_IDLY    = delay1 + delay2; // Время после которого произойдет прерывание PDB0 и можно будет забрать результаты ADC

  PDB0_SC |= BIT(7) + BIT(0);
}


/*-------------------------------------------------------------------------------------------------------------
  
-------------------------------------------------------------------------------------------------------------*/
void PDB_activate_ADC_triggers(void)
{
  PDB0_SC |= BIT(7);
}
/*-------------------------------------------------------------------------------------------------------------
  
-------------------------------------------------------------------------------------------------------------*/
void PDB_deactivate_ADC_triggers(void)
{
  PDB0_SC &= ~BIT(7);
}
/*-------------------------------------------------------------------------------------------------------------
  
-------------------------------------------------------------------------------------------------------------*/
signed short ADC_get_sample(ADC_MemMapPtr ADC, INT32U channel, INT32U diff, INT32U aver)
{
  if ( aver != 0 )
  {
    ADC->SC3 = 0
               + LSHIFT(0, 7) // CAL.     CAL begins the calibration sequence when set.
               + LSHIFT(0, 6) // CALF.    Read Only. 1 Calibration failed.
               + LSHIFT(0, 3) // ADCO.    Continuous conversion enable
               + LSHIFT(1, 2) // AVGE.    1 Hardware average function enabled.
               + LSHIFT(((aver - 1) & 0x03), 0) // AVGS.    11 32 samples averaged.
    ;
  }
  else
  {
    ADC->SC3 = 0
               + LSHIFT(0, 7) // CAL.     CAL begins the calibration sequence when set.
               + LSHIFT(0, 6) // CALF.    Read Only. 1 Calibration failed.
               + LSHIFT(0, 3) // ADCO.    Continuous conversion enable
               + LSHIFT(0, 2) // AVGE.    1 Hardware average function enabled.
               + LSHIFT(0, 0) // AVGS.    11 32 samples averaged.
    ;
  }
  ADC->SC1[0] = 0
                + LSHIFT(0, 7) // COCO. Read only. 1 Conversion completed.
                + LSHIFT(0, 6) // AIEN. 1 Conversion complete interrupt enabled.
                + LSHIFT((diff    & 0x01), 5) // DIFF. 1 Differential conversions and input channels are selected.
                + LSHIFT((channel & 0x1F), 0) // ADCH. Input channel select. 11111 Module disabled.
  ;

  // Ожидать завершения преобразования
  while ((ADC->SC1[0] & BIT(7)) == 0);

  return ADC->R[0];
}

/*-------------------------------------------------------------------------------------------------------------
  Подготовка всех ADC к работе
-------------------------------------------------------------------------------------------------------------*/
void ADCs_prepare(void)
{
  ADC_switch_on_all();
  adc_state.adc0_cal_res = ADC_calibrating(ADC0_BASE_PTR);
  adc_state.adc1_cal_res = ADC_calibrating(ADC1_BASE_PTR);
  adc_state.adc2_cal_res = ADC_calibrating(ADC2_BASE_PTR);
  adc_state.adc3_cal_res = ADC_calibrating(ADC3_BASE_PTR);
  ADC_init(ADC0_BASE_PTR);
  ADC_init(ADC1_BASE_PTR);
  ADC_init(ADC2_BASE_PTR);
  ADC_init(ADC3_BASE_PTR);

  ADC_set_channel(ADC0_BASE_PTR, SIG_II_W_CHANNEL     ,0,0);
  ADC_set_channel(ADC0_BASE_PTR, SIG_V_BUS_CHANNEL    ,0,1);
                                                      
  ADC_set_channel(ADC1_BASE_PTR, SIG_II_V_CHANNEL     ,0,0);
  ADC_set_channel(ADC1_BASE_PTR, SIG_TEMPER_CHANNEL   ,0,1);
                                                      
  ADC_set_channel(ADC2_BASE_PTR, SIG_II_U_CHANNEL     ,0,0);
  ADC_set_channel(ADC2_BASE_PTR, SIG_V_U_CHANNEL      ,0,1);
                                                      
  ADC_set_channel(ADC3_BASE_PTR, SIG_15V_MEAS_CHANNEL ,0,0);
  ADC_set_channel(ADC3_BASE_PTR, SIG_5V_MEAS_CHANNEL  ,0,1);

  ADC_activate_hrdw_trig(ADC0_BASE_PTR);
  ADC_activate_hrdw_trig(ADC1_BASE_PTR);
  ADC_activate_hrdw_trig(ADC2_BASE_PTR);
  ADC_activate_hrdw_trig(ADC3_BASE_PTR);
  PDB_configure_ADC_triggers();
}


/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
T_ADC_state *ADC_get_state(void)
{
  return &adc_state;
}

/*-------------------------------------------------------------------------------------------------------------
  Получить массив отсчетов  
-------------------------------------------------------------------------------------------------------------*/
void ADC_get_arr_smp(unsigned int indx, signed short *arr)
{
  INT32U i;

  for (i=0;i<MEAS_RES_ARR_SZ;i++)
  {
    arr[i]  = smpls[i][indx]; 
  }
}
/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void ADC_reset_smpl_arr_pos(void)
{
  arr_pos = 0;
}
/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
int ADC_is_smpl_arr_ready(void)
{
  if ( arr_pos == TST_SMPLS_ARR_SZ)
    return 1;
  else
    return 0;
}
/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void ADC_get_smpl_arr(signed short **ptr)
{
  int i;
  for (i=0;i<MEAS_RES_ARR_SZ;i++)
  {
    ptr[i] = &smpls[i][0]; 
  }
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void ADC_print_regs(ADC_MemMapPtr ADC)
{
  printf("SC1A=%04X ", ADC->SC1[0]);
  printf("SC1B=%04X ", ADC->SC1[1]);
  printf("CFG1=%04X ", ADC->CFG1);
  printf("CFG2=%04X ", ADC->CFG2);
  printf(" SC2=%04X ", ADC->SC2);
  printf(" SC3=%04X ", ADC->SC3);
  printf("\r\n");

  printf(" CV1=%04X ", ADC->CV1);
  printf(" CV2=%04X ", ADC->CV2);
  printf(" OFS=%04X ", ADC->OFS);
  printf("  PG=%04X ", ADC->PG);
  printf("  MG=%04X ", ADC->MG);
  printf("\r\n");

  printf("CLPD=%04X ", ADC->CLPD);
  printf("CLPS=%04X ", ADC->CLPS);
  printf("CLP4=%04X ", ADC->CLP4);
  printf("CLP3=%04X ", ADC->CLP3);
  printf("CLP2=%04X ", ADC->CLP2);
  printf("CLP1=%04X ", ADC->CLP1);
  printf("CLP0=%04X ", ADC->CLP0);
  printf("\r\n");

  printf("CLMD=%04X ", ADC->CLMD);
  printf("CLMS=%04X ", ADC->CLMS);
  printf("CLM4=%04X ", ADC->CLM4);
  printf("CLM3=%04X ", ADC->CLM3);
  printf("CLM2=%04X ", ADC->CLM2);
  printf("CLM1=%04X ", ADC->CLM1);
  printf("CLM0=%04X ", ADC->CLM0);
  printf("\r\n");

  printf("PGA =%08X ", ADC->PGA);
  printf("RA  =%08X ", ADC->R[0]);
  printf("RB  =%08X ", ADC->R[1]);
  printf("\r\n");

}
