/*HEADER*********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: watchdog_prv.h$
* $Version : 3.8.4.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   watchdog component.
*
*END************************************************************************/
#ifndef __watchdog_prv_h__
#define __watchdog_prv_h__ 1

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* The number of tasks in the alarm table */
#define WATCHDOG_TABLE_SIZE   (16)

/* The watchdog validation number */
#define WATCHDOG_VALID        (_mqx_uint)(0x77646f67)     /* "wdog" */

/*--------------------------------------------------------------------------*/
/*                       DATATYPE DEFINITIONS                               */


/*  WATCHDOG ALARM TABLE STRUCTURE */
/*!
 * \cond DOXYGEN_PRIVATE
 * 
 * \brief Watchdog alarm table structure. 
 */
typedef struct watchdog_alarm_table_struct
{
   /*! \brief The next table if required. */
   struct watchdog_alarm_table_struct _PTR_ NEXT_TABLE_PTR;

   /*! \brief The tasks being monitored. */
   TD_STRUCT_PTR                            TD_PTRS[WATCHDOG_TABLE_SIZE];

} WATCHDOG_ALARM_TABLE_STRUCT, _PTR_ WATCHDOG_ALARM_TABLE_STRUCT_PTR;
/*! \endcond */


/* WATCHDOG COMPONENT STRUCTURE */
/*!
 * \cond DOXYGEN_PRIVATE
 * 
 * \brief Watchdog component structure. 
 */
typedef struct watchdog_component_struct
{
   /*! \brief The table of alarms. */
   WATCHDOG_ALARM_TABLE_STRUCT ALARM_ENTRIES;

   /*! \brief Watchdog validation stamp. */
   _mqx_uint                    VALID;
   
   /*! \brief The function to call when the watchdog expires. */
   WATCHDOG_ERROR_FPTR          ERROR_FUNCTION;
   
   /*! \brief The old timer interrupt handler. */
   INT_ISR_FPTR                 TIMER_INTERRUPT_HANDLER;

   /*! \brief The interrupt vector. */
   _mqx_uint                    INTERRUPT_VECTOR;

} WATCHDOG_COMPONENT_STRUCT, _PTR_ WATCHDOG_COMPONENT_STRUCT_PTR;
/*! \endcond */

/*--------------------------------------------------------------------------*/
/*                       PROTOTYPE DEFINITIONS                              */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern void     _watchdog_cleanup(TD_STRUCT_PTR);
extern void     _watchdog_isr(pointer);
extern boolean  _watchdog_start_internal(MQX_TICK_STRUCT_PTR);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __watchdog_prv_h__ */
/* EOF */

