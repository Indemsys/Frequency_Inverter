/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor
* All Rights Reserved                       
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: psp_time.h$
* $Version : 3.8.5.0$
* $Date    : Jun-7-2012$
*
* Comments:
*   This file contains the definitions for time as used by the PSP.
*
*END************************************************************************/

#ifndef __psp_time_h__
#define __psp_time_h__

/*
** Macros and defines to manipulate the tick struct
*/
#define PSP_NUMBER_OF_TICK_FIELDS   (1)
#define PSP_NUMBER_OF_HWTICK_FIELDS (1)

/* Macro to retrieve the least significant _mqx_uint from a tick struct */
#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
   #define PSP_GET_LS_TICK_FROM_TICK_STRUCT(t_ptr) \
      (((MQX_TICK_STRUCT_PTR)(t_ptr))->TICKS[0])
#else
   #define PSP_GET_LS_TICK_FROM_TICK_STRUCT(t_ptr) \
      (((MQX_TICK_STRUCT_PTR)(t_ptr))->TICKS[MQX_NUM_TICK_FIELDS-1])
#endif

/* Macro to retrieve a _mqx_uint from a tick struct */
#define PSP_GET_ELEMENT_FROM_TICK_STRUCT(t_ptr, idx) \
   (((PSP_TICK_STRUCT_PTR)(t_ptr))->TICKS[0])

/* Macro increments to incrment tick structure */
#define PSP_INC_TICKS(t_ptr)  (++(((PSP_TICK_STRUCT_PTR)(t_ptr))->TICKS[0]))
   
#define PSP_NORMALIZE_TICKS(tick_ptr)	_psp_normalize_ticks((PSP_TICK_STRUCT_PTR)(tick_ptr))

/* Macro to add two tick structures including hw ticks */
#define PSP_ADD_TICKS(a_ptr, b_ptr, r_ptr)                                    \
   _psp_add_ticks((PSP_TICK_STRUCT_PTR)(a_ptr), (PSP_TICK_STRUCT_PTR)(b_ptr), \
      (PSP_TICK_STRUCT_PTR)(r_ptr))

/* Macro to subtract two tick structures NOT including hw ticks */
#define PSP_SUB_TICKS(a_ptr, b_ptr, r_ptr)    \
   _psp_subtract_ticks((PSP_TICK_STRUCT_PTR)(a_ptr), (PSP_TICK_STRUCT_PTR)(b_ptr), \
      (PSP_TICK_STRUCT_PTR)(r_ptr))

/* Macro to subtract two tick structures NOT including hw ticks and clamp into int32 interval */
#define PSP_SUB_TICKS_INT32(a_ptr, b_ptr, o_ptr)    \
   _psp_subtract_ticks_int32((PSP_TICK_STRUCT_PTR)(a_ptr), (PSP_TICK_STRUCT_PTR)(b_ptr), \
      (boolean _PTR_)(o_ptr))

/* Macro to add a single quantity of ticks to a tick structure */
#define PSP_ADD_TICKS_TO_TICK_STRUCT(a_ptr, v, r_ptr) \
   ((PSP_TICK_STRUCT_PTR)(r_ptr))->TICKS[0] = \
   ((PSP_TICK_STRUCT_PTR)(a_ptr))->TICKS[0] + (uint_64)(v)

/* Macro to subtract a single quantity of ticks from a tick structure */
#define PSP_SUB_TICKS_FROM_TICK_STRUCT(a_ptr, v, r_ptr) \
   ((PSP_TICK_STRUCT_PTR)(r_ptr))->TICKS[0] = \
   ((PSP_TICK_STRUCT_PTR)(a_ptr))->TICKS[0] - (uint_64)(v)

/* Macro to multipy the ticks by a 32 bit quantity */
#define PSP_MULTIPLY_TICKS_BY_UINT_32(m_ptr, m, r_ptr) \
   ((PSP_TICK_STRUCT_PTR)(r_ptr))->TICKS[0] = ((PSP_TICK_STRUCT_PTR)(m_ptr))->TICKS[0] * (uint_32)(m)

/* 
** Returns  1 if a >  b
** Returns -1 if a <  b
** Returns  0 if a == b
*/
#define PSP_CMP_TICKS(a_ptr, b_ptr) \
     ((((PSP_TICK_STRUCT_PTR)(a_ptr))->TICKS[0] > ((PSP_TICK_STRUCT_PTR)(b_ptr))->TICKS[0]) ?  1 : \
     (((PSP_TICK_STRUCT_PTR)(a_ptr))->TICKS[0] < ((PSP_TICK_STRUCT_PTR)(b_ptr))->TICKS[0]) ? -1 : \
     0 )

#define PSP_TICKS_TO_TIME(tick_ptr, time_ptr) \
   _psp_ticks_to_time((PSP_TICK_STRUCT_PTR)(tick_ptr), \
      (TIME_STRUCT_PTR)(time_ptr))
#define PSP_TIME_TO_TICKS(time_ptr, tick_ptr) \
   _psp_time_to_ticks((TIME_STRUCT_PTR)(time_ptr), \
      (PSP_TICK_STRUCT_PTR)(tick_ptr))
#define PSP_TICKS_TO_XDATE(tick_ptr, xdate_ptr) \
   _psp_ticks_to_xdate((PSP_TICK_STRUCT_PTR)(tick_ptr), xdate_ptr)

#define PSP_TICKS_TO_DAYS(tick_ptr, o_ptr) \
   _psp_ticks_to_days((PSP_TICK_STRUCT_PTR)(tick_ptr), \
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_HOURS(tick_ptr, o_ptr) \
   _psp_ticks_to_hours((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_MINUTES(tick_ptr, o_ptr) \
   _psp_ticks_to_minutes((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_SECONDS(tick_ptr, o_ptr) \
   _psp_ticks_to_seconds((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_MILLISECONDS(tick_ptr, o_ptr) \
   _psp_ticks_to_milliseconds((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_MILLISECONDS_TRUNCATE(tick_ptr, o_ptr) \
   _psp_ticks_to_milliseconds_truncate((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_MICROSECONDS(tick_ptr, o_ptr) \
   _psp_ticks_to_microseconds((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_MICROSECONDS_TRUNCATE(tick_ptr, o_ptr) \
   _psp_ticks_to_microseconds_truncate((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_NANOSECONDS(tick_ptr, o_ptr) \
   _psp_ticks_to_nanoseconds((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_NANOSECONDS_TRUNCATE(tick_ptr, o_ptr) \
   _psp_ticks_to_nanoseconds_truncate((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_PICOSECONDS(tick_ptr, o_ptr) \
   _psp_ticks_to_picoseconds((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))
#define PSP_TICKS_TO_PICOSECONDS_TRUNCATE(tick_ptr, o_ptr) \
   _psp_ticks_to_picoseconds_truncate((PSP_TICK_STRUCT_PTR)(tick_ptr),\
      (boolean _PTR_)(o_ptr))

#define PSP_GET_ELAPSED_MILLISECONDS() \
   _psp_get_elapsed_milliseconds()

#define PSP_DAYS_TO_TICKS(days, tick_ptr) \
   _psp_days_to_ticks((_mqx_uint)(days), (PSP_TICK_STRUCT_PTR)(tick_ptr))
#define PSP_HOURS_TO_TICKS(hours, tick_ptr) \
   _psp_hours_to_ticks((_mqx_uint)(hours), (PSP_TICK_STRUCT_PTR)(tick_ptr))
#define PSP_MINUTES_TO_TICKS(mins, tick_ptr) \
   _psp_minutes_to_ticks((_mqx_uint)(mins), (PSP_TICK_STRUCT_PTR)(tick_ptr))
#define PSP_SECONDS_TO_TICKS(secs, tick_ptr) \
   _psp_seconds_to_ticks((_mqx_uint)(secs), (PSP_TICK_STRUCT_PTR)(tick_ptr))
#define PSP_MILLISECONDS_TO_TICKS(msecs, tick_ptr) \
   _psp_msecs_to_ticks((_mqx_uint)(msecs), (PSP_TICK_STRUCT_PTR)(tick_ptr))
#define PSP_MILLISECONDS_TO_TICKS_QUICK(msecs, tick_ptr) \
   _psp_msecs_to_ticks_quick((_mqx_uint)(msecs), (PSP_TICK_STRUCT_PTR)(tick_ptr))
#define PSP_MICROSECONDS_TO_TICKS(usecs, tick_ptr) \
   _psp_usecs_to_ticks((_mqx_uint)(usecs), (PSP_TICK_STRUCT_PTR)(tick_ptr))
#define PSP_NANOSECONDS_TO_TICKS(nsecs, tick_ptr) \
   _psp_nsecs_to_ticks((_mqx_uint)(nsecs), (PSP_TICK_STRUCT_PTR)(tick_ptr))
#define PSP_PICOSECONDS_TO_TICKS(psecs, tick_ptr) \
   _psp_psecs_to_ticks((_mqx_uint)(psecs), (PSP_TICK_STRUCT_PTR)(tick_ptr))

#define PSP_PRINT_TICKS(tick_ptr) \
   _psp_print_ticks((PSP_TICK_STRUCT_PTR)(tick_ptr))


#ifndef __ASM__

/*-----------------------------------------------------------------------*/
/*
** PSP TICK STRUCT
**
** The representation of the MQX tick struct from the PSP's
** view
**
*/
/* 
** The _Packed keyword is specific to the MetaWare compiler. It will force the
** the PSP_TICK_STRUCT to be aligned on 32 bit boundaries instead of 64. This
** will allow casting between the PSP_TICK_STRUCT and the MQX_TICK_STRUCT
*/
typedef struct psp_tick_struct
{
   /* We need 64 bits for the ticks */
   uint_64    TICKS[PSP_NUMBER_OF_TICK_FIELDS];

   /* also need 32 bits for the hw ticks */
   uint_32    HW_TICKS[PSP_NUMBER_OF_HWTICK_FIELDS];

} PSP_TICK_STRUCT, _PTR_ PSP_TICK_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                  PROTOTYPES OF PSP FUNCTIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

void      _psp_add_ticks(PSP_TICK_STRUCT_PTR, PSP_TICK_STRUCT_PTR, 
   PSP_TICK_STRUCT_PTR);
void      _psp_subtract_ticks(PSP_TICK_STRUCT_PTR, PSP_TICK_STRUCT_PTR, 
   PSP_TICK_STRUCT_PTR);
int_32    _psp_subtract_ticks_int32(PSP_TICK_STRUCT_PTR, PSP_TICK_STRUCT_PTR, 
   boolean _PTR_);
void      _psp_mul_ticks_by_32(PSP_TICK_STRUCT_PTR, uint_32,
   PSP_TICK_STRUCT_PTR);

uint_32   _psp_ticks_to_time(PSP_TICK_STRUCT_PTR, TIME_STRUCT_PTR);
boolean   _psp_ticks_to_xdate(PSP_TICK_STRUCT_PTR, MQX_XDATE_STRUCT_PTR);

uint_32   _psp_ticks_to_days(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_hours(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_minutes(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_seconds(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_milliseconds(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_milliseconds_truncate(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_microseconds(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_microseconds_truncate(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_nanoseconds(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_nanoseconds_truncate(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_picoseconds(PSP_TICK_STRUCT_PTR, boolean _PTR_);
uint_32   _psp_ticks_to_picoseconds_truncate(PSP_TICK_STRUCT_PTR, boolean _PTR_);

uint_32   _psp_get_elapsed_milliseconds(void);
void      _psp_time_to_ticks(TIME_STRUCT_PTR, PSP_TICK_STRUCT_PTR);
boolean   _psp_xdate_to_ticks(MQX_XDATE_STRUCT_PTR, PSP_TICK_STRUCT_PTR);

void      _psp_days_to_ticks(_mqx_uint, PSP_TICK_STRUCT_PTR);
void      _psp_hours_to_ticks(_mqx_uint, PSP_TICK_STRUCT_PTR);
void      _psp_minutes_to_ticks(_mqx_uint, PSP_TICK_STRUCT_PTR);
void      _psp_seconds_to_ticks(_mqx_uint, PSP_TICK_STRUCT_PTR);
void      _psp_msecs_to_ticks(_mqx_uint, PSP_TICK_STRUCT_PTR);
void      _psp_msecs_to_ticks_quick(_mqx_uint, PSP_TICK_STRUCT_PTR);
void      _psp_usecs_to_ticks(_mqx_uint, PSP_TICK_STRUCT_PTR);
void      _psp_nsecs_to_ticks(_mqx_uint, PSP_TICK_STRUCT_PTR);
void      _psp_psecs_to_ticks(_mqx_uint, PSP_TICK_STRUCT_PTR);

void      _psp_print_ticks(PSP_TICK_STRUCT_PTR);

void      _psp_normalize_ticks(PSP_TICK_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif /* __ASM__ */

#endif /*__psp_time_h__ */
/* EOF */
