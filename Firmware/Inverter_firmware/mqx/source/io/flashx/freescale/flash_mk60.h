/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: flash_mk60.h$
* $Version : 3.8.11.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototype, defines for the internal 
*   flash driver.
*
*END************************************************************************/

#ifndef __flash_mk60_h__
#define __flash_mk60_h__


/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
 *----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#if BSP_TWR_K60F120M
pointer _bsp_get_ftfe_address(void);
#else
pointer _bsp_get_ftfl_address(void);
#endif
void kinetis_flash_invalidate_cache(uint_32);

#ifdef __cplusplus
}
#endif

#endif
