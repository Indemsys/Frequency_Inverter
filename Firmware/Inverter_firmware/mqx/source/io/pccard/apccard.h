#ifndef _apccard_h_
#define _apccard_h_
/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: apccard.h$
* $Version : 3.8.8.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   The file contains the structure definitions
*   public to the PC Card drivers
*
*END************************************************************************/

#include "ioctl.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/* Error codes */
#define APCCARD_OK                   0
#define APCCARD_INVALID_HANDLE       1
#define APCCARD_INVALID_SLOT         2
#define APCCARD_INVALID_VOLTAGE      3

/* Slot definitions */
#define APCCARD_SLOT_A         0x100
#define APCCARD_SLOT_B         0x101

/* Voltage levels */
#define APCCARD_VOLTAGE_HI_Z   0x1000
#define APCCARD_VOLTAGE_12V    0x1001
#define APCCARD_VOLTAGE_5V     0x1002
#define APCCARD_VOLTAGE_33V    0x1003
#define APCCARD_VOLTAGE_0V     0x1004

/* Address space specifiers */
#define APCCARD_ATTRIB_SPACE   0x200
#define APCCARD_COMMON_SPACE   0x201
#define APCCARD_IO_SPACE       0x202

/* IOCTL Commands */
#define APCCARD_IOCTL_IS_CARD_INSERTED          _IO(IO_TYPE_APCCARD,0x01)
#define APCCARD_IOCTL_SET_CRD_INSERT_CALLBACK   _IO(IO_TYPE_APCCARD,0x02)
#define APCCARD_IOCTL_SET_CRD_REMOVE_CALLBACK   _IO(IO_TYPE_APCCARD,0x03)
#define APCCARD_IOCTL_GET_ADDR_SPACE            _IO(IO_TYPE_APCCARD,0x06)
#define APCCARD_IOCTL_FREE_ADDR_SPACE           _IO(IO_TYPE_APCCARD,0x07)
#define APCCARD_IOCTL_SENSE_VOLTAGE             _IO(IO_TYPE_APCCARD,0x08)
#define APCCARD_IOCTL_VCC_ENABLE                _IO(IO_TYPE_APCCARD,0x09)
#define APCCARD_IOCTL_VPP_ENABLE                _IO(IO_TYPE_APCCARD,0x0A)
#define APCCARD_IOCTL_RESET                     _IO(IO_TYPE_APCCARD,0x0B)
#define APCCARD_IOCTL_VCC_DISABLE               _IO(IO_TYPE_APCCARD,0x0C)
#define APCCARD_IOCTL_VPP_DISABLE               _IO(IO_TYPE_APCCARD,0x0D)
#define APCCARD_IOCTL_IDENTIFY                  _IO(IO_TYPE_APCCARD,0x0E)
#define APCCARD_IOCTL_POWERUP_CARD              _IO(IO_TYPE_APCCARD,0x0F)
#define APCCARD_IOCTL_POWERDOWN_CARD            _IO(IO_TYPE_APCCARD,0x10)
#define APCCARD_IOCTL_WAIT_TILL_READY           _IO(IO_TYPE_APCCARD,0x11)
#define APCCARD_IOCTL_CARD_READY                _IO(IO_TYPE_APCCARD,0x12)
#define APCCARD_IOCTL_READ_TUPLE                _IO(IO_TYPE_APCCARD,0x13)

/* CIS offsets */
#define IO_APCCARD_TUPLE_CODE_CIS_OFFSET        0x00
#define IO_APCCARD_TUPLE_LINK_CIS_OFFSET        0x02
#define IO_APCCARD_TUPLE_DATA_CIS_OFFSET        0x04

/* Some tuple codes */
#define IO_APCCARD_TUPLE_CISTPL_NULL            0x00
#define IO_APCCARD_TUPLE_CISTPL_DEVICE          0x01
#define IO_APCCARD_TUPLE_CISTPL_FUNCID          0x21
#define IO_APCCARD_TUPLE_CISTPL_FUNCE           0x22
#define IO_APCCARD_TUPLE_CISTPL_CFTABLE_ENTRY   0x1B
#define IO_APCCARD_TUPLE_END_OF_CIS             0xFF

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/

/* This structure is used in the ioctl command APCCARD_IOCTL_GET_ADDR_SPACE */
typedef struct apccard_addr_info_struct
{
   uint_32 OFFSET;      /* Offset from PCMCIA base               */
   uint_32 TYPE;        /* Address type: Attrib, common, io      */
   uint_32 BANK_SIZE;   /* bank size from 1 to 64 megabytes      */
   uint_32 PORT_SIZE;   /* Port size: 8 or 16                    */
   pointer ADDRESS;     /* Filled in by driver. Absolute address */
} APCCARD_ADDR_INFO_STRUCT, _PTR_ APCCARD_ADDR_INFO_STRUCT_PTR;


/*-----------------------------------------------------------------------
**                      FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
