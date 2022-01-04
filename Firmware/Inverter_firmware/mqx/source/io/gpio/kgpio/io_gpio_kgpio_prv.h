/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: io_gpio_kgpio_prv.h$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   The file contains definitions used in user program and/or in other
*   kernel modules to access GPIO pins
*
*END************************************************************************/

#ifndef __io_gpio_cpu_prv_h__
#define __io_gpio_cpu_prv_h__

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------*/
/*
**                          TYPE DEFINITIONS
*/

typedef enum devices {
     DEV_INPUT,
     DEV_OUTPUT
} DEVICE_TYPE;

typedef union pin_map_struct {
    struct {
        uint_32  porta;
        uint_32  portb;
        uint_32  portc;
        uint_32  portd;
        uint_32  porte;
    } reg;
    uint_32 memory32[5];
} GPIO_PIN_MAP, _PTR_ GPIO_PIN_MAP_PTR;

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
