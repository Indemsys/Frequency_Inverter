/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2011 Embedded Access Inc.;
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
* $FileName: linflexd.h$
* $Version :  3.8.3.0$
* $Date    :  Sep-26-2012$
*
* Comments:
*   This include file provides the register defintons for the LinFlexd Uart.
*
*
*END************************************************************************/

#ifndef __linflexd_h__
#define __linflexd_h__ 1


/*--------------------------------------------------------------------------*/
/*
**                    CONSTANT DEFINITIONS
*/

#define LINFLEXD_LINCR1_INIT        (1<<0)

#define LINFLEXD_LINIER_SZIE        (1<<15)
#define LINFLEXD_LINIER_OCIE        (1<<14)
#define LINFLEXD_LINIER_BEIE        (1<<13)
#define LINFLEXD_LINIER_CEIE        (1<<12)
#define LINFLEXD_LINIER_HEIE        (1<<11)
#define LINFLEXD_LINIER_FEIE        (1<<8)
#define LINFLEXD_LINIER_BOIE        (1<<7)
#define LINFLEXD_LINIER_LSIE        (1<<6)
#define LINFLEXD_LINIER_WUIE        (1<<5)
#define LINFLEXD_LINIER_DBFIE       (1<<4)
#define LINFLEXD_LINIER_DBEIETOIE   (1<<3)
#define LINFLEXD_LINIER_DRIE        (1<<2)
#define LINFLEXD_LINIER_DTIE        (1<<1)
#define LINFLEXD_LINIER_HRIE        (1<<0)

#if defined(PSP_MPXD10)
#define LINFLEXD_UARTCR_TDFLTFC_MASK 0x6000
#define LINFLEXD_UARTCR_RDFLRFC_MASK 0xc00
#else
#define LINFLEXD_UARTCR_TDFLTFC_MASK 0xe000
#define LINFLEXD_UARTCR_RDFLRFC_MASK 0x1c00
#endif

#define LINFLEXD_UARTCR_TDFLTFC_SHIFT 13
#define LINFLEXD_UARTCR_RDFLRFC_SHIFT 10

#if defined(PSP_MPXD10)
  #define LINFLEXD_UARTCR_RESERVED_25    (1<<9)
  #define LINFLEXD_UARTCR_RESERVED_24    (1<<8)
  #define LINFLEXD_UARTCR_RESERVED_23    (1<<7)
  #define LINFLEXD_UARTCR_RESERVED_22    (1<<6)
#else /* PXD10 doesn't support these bits */
#define LINFLEXD_UARTCR_RFBM    (1<<9)
#define LINFLEXD_UARTCR_TFBM    (1<<8)
#define LINFLEXD_UARTCR_WL1     (1<<7)
#define LINFLEXD_UARTCR_PC1     (1<<6)
#endif

#define LINFLEXD_UARTCR_RXEN    (1<<5)
#define LINFLEXD_UARTCR_TXEN    (1<<4)
#if defined(PSP_MPXD10)
#define LINFLEXD_UARTCR_OP     (1<<3)
#else
#define LINFLEXD_UARTCR_PC0     (1<<3)
#endif

#define LINFLEXD_UARTCR_PCE     (1<<2)
#define LINFLEXD_UARTCR_WL0     (1<<1)
#define LINFLEXD_UARTCR_UART    (1<<0)

#define LINFLEXD_UARTSR_SZF     (1<<15)
#define LINFLEXD_UARTSR_OCF     (1<<14)
#define LINFLEXD_UARTSR_PE3     (1<<13)
#define LINFLEXD_UARTSR_PE2     (1<<12)
#define LINFLEXD_UARTSR_PE1     (1<<11)
#define LINFLEXD_UARTSR_PE0     (1<<10)
#define LINFLEXD_UARTSR_RMB     (1<<9)
#define LINFLEXD_UARTSR_FEF     (1<<8)
#define LINFLEXD_UARTSR_BOF     (1<<7)
#define LINFLEXD_UARTSR_RPS     (1<<6)
#define LINFLEXD_UARTSR_WUF     (1<<5)
#define LINFLEXD_UARTSR_4       (1<<4)
  
#if defined(PSP_MPXD10)
#define LINFLEXD_UARTSR_RESERVED_28 (1<<3)
#else
#define LINFLEXD_UARTSR_TO      (1<<3)
#endif
  
#define LINFLEXD_UARTSR_DRFRFE  (1<<2)
#define LINFLEXD_UARTSR_DTFTFF  (1<<1)
#define LINFLEXD_UARTSR_NF      (1<<0)
#define LINFLEXD_UARTSR_PE      (LINFLEXD_UARTSR_PE0|LINFLEXD_UARTSR_PE1|LINFLEXD_UARTSR_PE2|LINFLEXD_UARTSR_PE3)


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

typedef volatile struct linflexd_reg_struct
{
    uint_32   LINCR1;  // 0x0000 LIN control register 1
    uint_32   LINIER;  // 0x0004 LIN interrupt enable register
    uint_32   LINSR;   // 0x0008 LIN status register
    uint_32   LINESR;  // 0x000C LIN error status register
    uint_32   UARTCR;  // 0x0010 UART mode control register
    uint_32   UARTSR;  // 0x0014 UART mode status register
    uint_32   LINTCSR; // 0x0018 LIN timeout control status register
    uint_32   LINOCR;  // 0x001C LIN output compare register
    uint_32   LINTOCR; // 0x0020 LIN timeout control register
    uint_32   LINFBRR; // 0x0024 LIN fractional baud rate register
    uint_32   LINIBRR; // 0x0028 LIN integer baud rate register
    uint_32   LINCFR;  // 0x002C LIN checksum field register
    uint_32   LINCR2;  // 0x0030 LIN control register 2
    uint_32   BIDR;    // 0x0034 Buffer identifier register
    uchar     BDRL[4]; // 0x0038 Buffer data register least significant
    uchar     BDRM[4]; // 0x003C Buffer data register most significant
    uint_32   IFER;    // 0x0040 Identifier filter enable register
    uint_32   IFMI;    // 0x0044 Identifier filter match index
    uint_32   IFMR;    // 0x0048 Identifier filter mode register
    uint_32   IFCR0;   // 0x004C Identifier filter control register 0
    uint_32   IFCR1;   // 0x0050 Identifier filter control register 1
    uint_32   IFCR2;   // 0x0054 Identifier filter control register 2
    uint_32   IFCR3;   // 0x0058 Identifier filter control register 3
    uint_32   IFCR4;   // 0x005C Identifier filter control register 4
    uint_32   IFCR5;   // 0x0060 Identifier filter control register 5
    uint_32   IFCR6;   // 0x0064 Identifier filter control register 6
    uint_32   IFCR7;   // 0x0068 Identifier filter control register 7
    uint_32   IFCR8;   // 0x006C Identifier filter control register 8
    uint_32   IFCR9;   // 0x0070 Identifier filter control register 9
    uint_32   IFCR10;  // 0x0074 Identifier filter control register 10
    uint_32   IFCR11;  // 0x0078 Identifier filter control register 11
    uint_32   IFCR12;  // 0x007C Identifier filter control register 12
    uint_32   IFCR13;  // 0x0080 Identifier filter control register 13
    uint_32   IFCR14;  // 0x0084 Identifier filter control register 14
    uint_32   IFCR15;  // 0x0088 Identifier filter control register 15
    uint_32   GCR;     // 0x008C Global control register
    uint_32   UARTPTO; // 0x0090 UART preset timeout register
    uint_32   UARTCTO; // 0x0094 UART current timeout register
    uint_32   DMATXE;  // 0x0098 DMA Tx enable register
    uint_32   DMARXE;  // 0x009C DMAx enable register

} LINFLEXD_REG_STRUCT;

typedef volatile struct linflexd_reg_struct *VLINFLEXD_REG_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

VLINFLEXD_REG_STRUCT_PTR _bsp_get_linflexd_base_address(uint_32 channel);

#ifdef __cplusplus
}
#endif

#endif

