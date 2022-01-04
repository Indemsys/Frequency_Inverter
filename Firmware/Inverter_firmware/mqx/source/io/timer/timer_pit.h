/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: timer_pit.h$
* $Version : 3.8.1.0$
* $Date    : May-11-2011$
*
* Comments:
*
*   This file contains definitions for the programmable interrupt timer
*   utility functions.
*
*END************************************************************************/

#ifndef __timer_pit_h__
#define __timer_pit_h__

#include <mqx.h>
#include <bsp.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct pit_timer_init_struct {
   PSP_INTERRUPT_TABLE_INDEX  INTERRUPT;
   _int_level                 LEVEL;
   _int_priority              PRIORITY;
} PIT_TIMER_INIT_STRUCT, _PTR_ PIT_TIMER_INIT_STRUCT_PTR;


/*-----------------------------------------------------------------------*/
/*                      FUNCTION PROTOTYPES
*/

extern const PIT_TIMER_INIT_STRUCT _bsp_timers[];

pointer _bsp_get_pit_base_address (uint_8);
uint_32 _bsp_get_pit_vector (uint_8);

uint_32 _pit_init_freq(uint_8, uint_32, uint_32, boolean);
uint_32 _pit_get_hwticks(pointer);
void _pit_clear_int(uint_8);

#define _pit_unmask_int(timer)  _bsp_int_enable(_bsp_timers[(timer)].INTERRUPT)
#define _pit_mask_int(timer)   _bsp_int_disable(_bsp_timers[(timer)].INTERRUPT)

#ifdef __cplusplus
}
#endif

#endif

