#include <string.h>
#include "app_cfg.h"
#include "USB_timer.h"

#define APP_TIMER_PERIOD  100

/* Array of Timer Objects */
TIMER_OBJECT g_TimerObjectArray[MAX_TIMER_OBJECTS];
void         Init_periodic_app_timer(void);
INT8U        Add_periodic_callback(PTIMER_OBJECT pTimerObject);
INT8U        Remove_periodic_callback(INT8U index);
void         Timer_ISR(void);


//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Init_periodic_app_timer()
{
  (void)memset(g_TimerObjectArray, (int)NULL, sizeof(g_TimerObjectArray));
  PIT0_init(APP_TIMER_PERIOD);
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
INT8U Add_periodic_callback(PTIMER_OBJECT pTimerObject)
{
  INT8U index;
  if ( pTimerObject == NULL ) return (INT8U)ERR_INVALID_PARAM;
  if ( pTimerObject->msCount == (unsigned int)INVALID_TIME_COUNT ) return (INT8U)ERR_INVALID_PARAM;

  for (index = 0; index < MAX_TIMER_OBJECTS; index++)
  {
    /* Disable Timer Interrupts */
    PIT0_int_disable();

    if ( g_TimerObjectArray[index].pfnTimerCallback == NULL )
    {
      (void)memcpy(&g_TimerObjectArray[index], pTimerObject, sizeof(TIMER_OBJECT));
      /* Enable Timer Interrupts */
      PIT0_int_enable();
      break;
    }
    /* Enable Timer Interrupts */
    PIT0_int_enable();
  }
  if ( index == MAX_TIMER_OBJECTS ) return (INT8U)ERR_TIMER_QUEUE_FULL;
  return index;
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
INT8U Remove_periodic_callback(INT8U index)
{
  if ( index >= MAX_TIMER_OBJECTS ) return (INT8U)ERR_INVALID_PARAM;
  /* Disable Timer Interrupts */
  PIT0_int_disable();
  (void)memset(&g_TimerObjectArray[index], (int)NULL, sizeof(TIMER_OBJECT));
  /* Enable Timer Interrupts */
  PIT0_int_enable();
  return (INT8U)ERR_SUCCESS;
}


//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void PIT0_IRQHandler(void)
{
  PIT_MemMapPtr PIT = PIT_BASE_PTR;
  INT8U index;

  if ( PIT->CHANNEL[0].TFLG & BIT(0) )
  {
    
    PIT->CHANNEL[0].TFLG = BIT(0); // Сбросит флаг прерывания 

    PIT0_int_disable();

    /* Call Pending Timer CallBacks */
    for (index = 0; index < MAX_TIMER_OBJECTS; index++)
    {
      PTIMER_OBJECT ptemp = &g_TimerObjectArray[index];
      if ( ptemp->pfnTimerCallback == NULL )
      {
        continue;
      }
      ptemp->msCount--;
      if ( ptemp->msCount < 0 )
      {
        PFNTIMER_CALLBACK pfnTimerCallback = ptemp->pfnTimerCallback;
#ifdef TIMER_CALLBACK_ARG
        void *parg = ptemp->arg;
#endif
        (void)Remove_periodic_callback(index);
#ifdef TIMER_CALLBACK_ARG
        pfnTimerCallback(parg);
#else
        pfnTimerCallback();
#endif
      }
    }
  }
  PIT0_int_enable();
}

