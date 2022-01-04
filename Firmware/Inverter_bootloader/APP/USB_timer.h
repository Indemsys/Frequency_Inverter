#ifndef _RTC_HEADER_H
  #define _RTC_HEADER_H
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
  #define ERR_SUCCESS             0
  #define ERR_INVALID_PARAM       -1
  #define ERR_TIMER_QUEUE_FULL	-2
  #define INVALID_TIME_COUNT		0xFFFFFFFF
/******************************************************************************
 * Types
 *****************************************************************************/

/* Timer Callback Function Prototype */
  #ifdef TIMER_CALLBACK_ARG
typedef void (*PFNTIMER_CALLBACK)(void *arg);
  #else
typedef void (*PFNTIMER_CALLBACK)(void);
  #endif
/* Timer Object Structure */
typedef struct _timer_object
{
  int msCount;                /* msec Timeout Value */
  PFNTIMER_CALLBACK pfnTimerCallback; /* Callback Function */
  #ifdef TIMER_CALLBACK_ARG
  void *arg;                          /* Callback Function Argument */
  #endif
} TIMER_OBJECT, *PTIMER_OBJECT;

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
extern void Init_periodic_app_timer(void);
extern INT8U Add_periodic_callback(PTIMER_OBJECT pTimerObject);
extern INT8U Remove_periodic_callback(INT8U index);

#endif /* _RTC_HEADER_H */
