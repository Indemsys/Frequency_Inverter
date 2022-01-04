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
* $FileName: init_iodebug.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the initialization definition for debug driver
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"


#if defined(__CC_ARM)
/* ITM - KEIL default*/
const IODEBUG_INIT_STRUCT _bsp_iodebug_init = { 
    IODEBUG_MODE_ITM,       /* Driver mode */ 
    0,                      /* Length of buffered data */ 
    '\n'                    /* Default flush character */
};


#else
/* SEMIHOST - CW, IAR */
const IODEBUG_INIT_STRUCT _bsp_iodebug_init = { 
    IODEBUG_MODE_SEMIHOST,  /* Driver mode */ 
    127,                    /* Length of buffered data */ 
    '\n'                    /* Default flush character */
};
#endif 

/*
** CW : If semihost used, data length should be less/eq than 1023
** IAR: If semihost used, data length should be less/eq than 127
*/
