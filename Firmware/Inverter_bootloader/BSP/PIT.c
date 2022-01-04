#include "app_types.h"
#include <MK60F15.h>
#include "app_cfg.h"


volatile INT32U  g_1ms_cnt;
volatile INT32U led_cnt;
//-------------------------------------------------------------------------------------------------------
//  Periodic interrupt timer 0 initialisation
//   
//  period - задается в мкс
//-------------------------------------------------------------------------------------------------------
void PIT0_init(unsigned int period )
{
  SIM_MemMapPtr SIM = SIM_BASE_PTR;
  PIT_MemMapPtr PIT = PIT_BASE_PTR;

  /* Timer 0.1ms */
  PIT->CHANNEL[0].LDVAL = (PERIPH_CLOCK/(1000000u))*period;

  PIT->CHANNEL[0].TFLG = BIT(0); // Сбросит флаг прерывания 

  PIT->CHANNEL[0].TCTRL  = 0
              + LSHIFT(0x01, 1) // TIE | Timer Interrupt Enable Bit.| 1 Interrupt will be requested whenever TIF is set.
              + LSHIFT(0x01, 0) // TEN | Timer Enable Bit.          | 1 Timer n is active.
  ;

  Set_VIC_chanel(INT_PIT0, 2, 0);
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void PIT0_int_enable(void)
{
  PIT_MemMapPtr PIT = PIT_BASE_PTR;

  PIT->CHANNEL[0].TCTRL  = 0
              + LSHIFT(0x01, 1) // TIE | Timer Interrupt Enable Bit.| 1 Interrupt will be requested whenever TIF is set.
              + LSHIFT(0x01, 0) // TEN | Timer Enable Bit.          | 1 Timer n is active.
  ;
  return;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void PIT0_int_disable(void)
{
  PIT_MemMapPtr PIT = PIT_BASE_PTR;

  PIT->CHANNEL[0].TCTRL  = 0
              + LSHIFT(0x00, 1) // TIE | Timer Interrupt Enable Bit.| 
              + LSHIFT(0x00, 0) // TEN | Timer Enable Bit.          | 
  ;
  PIT->CHANNEL[0].TFLG = BIT(0); // Сбросит флаг прерывания 
  return;
}


//-------------------------------------------------------------------------------------------------------
//  Periodic interrupt timer 0 initialisation
//   
//  period - задается в мкс
//-------------------------------------------------------------------------------------------------------
void PIT1_init(unsigned int period )
{
  SIM_MemMapPtr SIM = SIM_BASE_PTR;
  PIT_MemMapPtr PIT = PIT_BASE_PTR;

  /* Timer 0.1ms */
  PIT->CHANNEL[1].LDVAL = (PERIPH_CLOCK/(1000000u))*period;

  PIT->CHANNEL[1].TFLG = BIT(0); // Сбросит флаг прерывания 

  PIT->CHANNEL[1].TCTRL  = 0
              + LSHIFT(0x01, 1) // TIE | Timer Interrupt Enable Bit.| 1 Interrupt will be requested whenever TIF is set.
              + LSHIFT(0x01, 0) // TEN | Timer Enable Bit.          | 1 Timer n is active.
  ;

  Set_VIC_chanel(INT_PIT1, 0, 0);
}


//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void PIT1_IRQHandler(void)
{
  PIT_MemMapPtr PIT = PIT_BASE_PTR;


  if ( PIT->CHANNEL[1].TFLG & BIT(0) )
  {
    
    PIT->CHANNEL[1].TFLG = BIT(0); // Сбросит флаг прерывания 
    
    g_1ms_cnt++;

    led_cnt++;
    if ( led_cnt> 100 )
    {
      Led_control(1, Led_state(1));
      led_cnt = 0;
    }

  }
}
