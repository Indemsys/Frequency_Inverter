#ifndef __io_gpio_cpu_prv_h__
#define __io_gpio_cpu_prv_h__
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
* $FileName: io_gpio_mcf5225x_prv.h$
* $Version : 3.0.4.0$
* $Date    : May-19-2009$
*
* Comments:
*
*   The file contains definitions used in user program and/or in other
*   kernel modules to access GPIO pins
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum devices {
     DEV_INPUT,
     DEV_OUTPUT
} DEVICE_TYPE;

typedef union pin_map_struct {
    struct {
        uint_8  portte;
        uint_8  porttf;
        uint_8  porttg;
        uint_8  portth;
        uint_8  portti;
        uint_8  unusedA;
        uint_8  porttj;
        uint_8  unusedB;
        uint_8  portnq;
        uint_8  unusedC;
        uint_8  portan;
        uint_8  portas;
        uint_8  portqs;
        uint_8  unusedD;
        uint_8  portta;
        uint_8  porttc;
        uint_8  unusedE;
        uint_8  portua;
        uint_8  portub;
        uint_8  portuc;
        uint_8  portdd;
        uint_8  unusedF[3];
    } reg;
    uint_32 memory32[6];
    uint_8  memory8[6*4];
} GPIO_PIN_MAP, _PTR_ GPIO_PIN_MAP_PTR;

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
