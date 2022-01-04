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
* $FileName: fio_prv.h$
* $Version : 3.8.8.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file includes the private definitions for the formatted I/O .
*
*END************************************************************************/
#ifndef __fio_prv_h__
#define __fio_prv_h__

/*--------------------------------------------------------------------------*/
/*
 *  Compiler Dependencies
 *
 *  Most compilers have adequate modf and strtod functions
 */
#ifdef NEED_MODF
#define  modf       _io_modf
#endif
#ifdef NEED_STRTOD
#define  strtod     _io_strtod
#endif

/*--------------------------------------------------------------------------*/
/*
 *                            CONSTANT DEFINITIONS
 */

/* 
 * Type definitions also used for sizing by doprint 
 * They are the maximum string size that a 32/64 bit number 
 * can be displayed as. 
 */
#define _MQX_IO_DIVISION_ADJUST_64 1000000000000000000LL
#define PRINT_OCTAL_64   (22L)
#define PRINT_DECIMAL_64 (20L)
#define PRINT_HEX_64     (16L)

#define _MQX_IO_DIVISION_ADJUST 1000000000L
#define PRINT_OCTAL   (11L)
#define PRINT_DECIMAL (10L)
#define PRINT_HEX     (8L)

#define PRINT_ADDRESS (8L)

/* Type definitions use in the control of scanline */

#define SCAN_ERROR    (-1)
#define SCAN_LLONG    (0)
#define SCAN_WLONG    (1)
#define SCAN_BLONG    (2)
#define SCAN_MLONG    (3)


/*--------------------------------------------------------------------------*/
/*
 *                            FUNCTION PROTOTYPES
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_int _io_doprint(MQX_FILE_PTR, IO_PUTCHAR_FPTR, _mqx_int max_count, char _PTR_, va_list);
extern _mqx_int _io_sputc(_mqx_int, MQX_FILE_PTR);
extern _mqx_int _io_scanline(char _PTR_, char _PTR_, va_list);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
