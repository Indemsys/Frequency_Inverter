#ifndef __INT_FLASH_MCF51MM_H__
#define __INT_FLASH_MCF51MM_H__
/**HEADER********************************************************************
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
* $FileName: flash_mcf51mm.h$
* $Version : 3.8.3.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines for the internal 
*   flash driver
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
** FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

pointer _bsp_get_cfm_address(uchar);
pointer _bsp_get_pmc_address();
uint_32 _mcf51_get_rtc_cfg_state();

#ifdef __cplusplus
}
#endif

#endif /* __INT_FLASH_MCF51MM_H__ */
