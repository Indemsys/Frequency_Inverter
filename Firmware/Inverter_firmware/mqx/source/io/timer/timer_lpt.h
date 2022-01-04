/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
***************************************************************************** 
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
*****************************************************************************
*
* $FileName: timer_lpt.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   Low power timer public header file.
*
*END************************************************************************/


#ifndef __timer_lpt_h__
#define __timer_lpt_h__


#include <mqx.h>
#include <bsp.h>


/*-------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

#define LPT_FLAG_CLOCK_SOURCE_MCGIRCLK      (0x00000001)
#define LPT_FLAG_CLOCK_SOURCE_LPO           (0x00000002)
#define LPT_FLAG_CLOCK_SOURCE_ERCLK32K      (0x00000004)
#define LPT_FLAG_CLOCK_SOURCE_OSCERCLK      (0x00000008)
#define LPT_FLAG_CLOCK_SOURCE_ALL           (0x0000000F)


/*-------------------------------------------------------------------------*/
/*
**                            MACRO DECLARATIONS
*/


/*-------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/


/*-------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/


#ifdef __cplusplus
extern "C" {
#endif


extern pointer  _bsp_get_lpt_base_address (uint_8);
extern uint_32  _bsp_get_lpt_vector (uint_8);
extern uint_32  _bsp_get_lpt_clock_frequency (uint_8, uint_32);
extern uint_32  _bsp_enable_lpt_clock (uint_8);

extern _mqx_int _lpt_install (uint_8, uint_32, uint_32, uint_32, void (_CODE_PTR_ isr_ptr)(void *), boolean);
extern _mqx_int _lpt_install_kernel (uint_8, uint_32, uint_32, uint_32, boolean);
extern uint_32  _lpt_init (uint_8, uint_32, uint_32, boolean);

extern uint_32  _lpt_get_hwticks (pointer);

extern void     _lpt_mask_int (uint_8);
extern void     _lpt_unmask_int (uint_8);
extern void     _lpt_clear_int (uint_8);
extern void     _lpt_run (uint_8, boolean);


#ifdef __cplusplus
}
#endif


#endif

/* EOF */
