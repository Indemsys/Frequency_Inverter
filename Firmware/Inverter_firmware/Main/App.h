#include <ctype.h>
#include <math.h>
#include <mqx.h>
#include <bsp.h>
#include <mutex.h>
#include <fio.h>
//#include <flexcan.h>
#include <arm_itm.h>
#include "gflib.h"
#include "gdflib.h"
#include "gmclib.h"
#include "Pins_control.h"
#include "Motor_control.h"
#include "Temperature_control.h"
#include "ADC_control.h"
#include "LCD_control.h"
#include "SIN_COS_generator.h"
#include "MonitorVT100.h"
#include "Tests.h"
#include "CAN_control.h"
#include "app_IDs.h"


#define MOTISR_IDX  1
#define CTRL_IDX    2
#define MEAS_IDX    3
#define VT100_IDX   4
#define LCD_IDX     5
#define CAN_TX_IDX  6
#define CAN_RX_IDX  7


#define MOTISR_ID_PRIO   7
#define CTRL_ID_PRIO     8
#define MEAS_ID_PRIO     9

#define CAN_RX_ID_PRIO   9
#define CAN_TX_ID_PRIO   9

#define VT100_ID_PRIO    10
#define LCD_ID_PRIO      11

#define ETM0_ISR_PRIO  3  // Приоритет процедуры прерывания узла ШИМ модуляции
#define PDB0_ISR_PRIO  3  // Приоритет процедуры прерывания узла программируемой задержки для считывания результатов АЦП
#define CAN_ISR_PRIO   3  // Приоритет процедуры прерывания от контроллера CAN шины


#define MEAS_RES_ARR_SZ 11
#define MEAS_RES_LOG_SZ 1000 // 50 сек

extern void Delay_m8(int cnt); // Задержка на (cnt+1)*8 тактов . Передача нуля недопускается

  #define  DELAY_1us  Delay_m8(15-1)
  #define  DELAY_2us  Delay_m8(2*15-1)
  #define  DELAY_4us  Delay_m8(4*15-1)
  #define  DELAY_8us  Delay_m8(8*15-1)
  #define  DELAY_30us Delay_m8(30*15-1)


void    Lock_meas_mutex(void);
void    Unlock_meas_mutex(void);
//INT32U  Reset_rms_log(void);
//INT32U  Get_rms_log_pos(void);
//void    Get_rms_log_arr(float **ptr);
float   Get_aver_curr(void);
void    Reset_aver_curr(void);
