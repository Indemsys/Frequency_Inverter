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
* $FileName: timer_mcf5225.c$
* $Version : 3.8.1.0$
* $Date    : May-11-2011$
*
* Comments:
*
*   This file contains timer functions for use with a MCF5208.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "timer_pit.h"


const PIT_TIMER_INIT_STRUCT _bsp_timers[MCF5225_NUM_TIMERS] = {
   {MCF5225_INT_PIT0, BSP_PIT0_INT_LEVEL, BSP_PIT0_INT_SUBLEVEL},
   {MCF5225_INT_PIT1, BSP_PIT1_INT_LEVEL, BSP_PIT1_INT_SUBLEVEL}
};


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_pit_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding PIT
*
*END*----------------------------------------------------------------------*/

pointer _bsp_get_pit_base_address (uint_8 timer)
{
  volatile MCF5225_STRUCT _PTR_ reg_ptr = _PSP_GET_IPSBAR();
  if (timer >= MCF5225_NUM_TIMERS) return NULL;
  return (pointer)&reg_ptr->PIT[timer];
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_pit_vector
* Returned Value   : Vector number upon success, 0 upon failure
* Comments         :
*    This function returns desired interrupt vector number for specified PIT
*
*END*----------------------------------------------------------------------*/

uint_32 _bsp_get_pit_vector (uint_8 timer)
{
  if (timer >= MCF5225_NUM_TIMERS) return 0;
  return _bsp_timers[timer].INTERRUPT;
}
