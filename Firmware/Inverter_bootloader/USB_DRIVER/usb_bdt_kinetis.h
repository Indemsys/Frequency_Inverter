/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
******************************************************************************
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
**************************************************************************/ /*!
  *
  * @file usb_bdt_kinetis.h
  *
  * @author 
  *
  * @version 
  *
  * @date Jun-05-2009
  *
  * @brief The file contains definitions of Buffer Descriptor Table.
  *
  *****************************************************************************/

#ifndef _USBBDT_H
  #define _USBBDT_H

/* Buffer Descriptor Status Register Initialization Parameters */
  #define _BDTSTALL      (0x04)           /* Buffer Stall enable */
  #define _DATA0         (0x00)           /* DATA0 packet expected next */
  #define _DATA1         (0x40)           /* DATA1 packet expected next */
  #define _DTS           (0x08)           /* DTS Mask */
  #define _OWN           (0x80)           /* SIE owns buffer */
  #define _CPU           (0x00)           /* CPU owns buffer */
  #define _KEEP          (0x20)           /* keep bit */

  #define MAX_EP_CBL_INDEX  (64)             /* Maximum BDT Indexes */


/******************************************************************************
 * Types
 *****************************************************************************/
/* This structure is an exact replica of the BDT MAP in the USB RAM 
   The BDT_STAT defines the stat byte of the buffer descriptor vector.
   McuCtlBit structure defines the bits that have a meaning from CPU 
   point of view.SieCtlBit structure defines the bits that have a 
   meaning from USB controller point of view.      
*/

  #pragma pack(1)
typedef struct  
{
  INT8U :1;
  INT8U :1;
  INT8U bdtstall:1;            /* Buffer Stall Enable */
  INT8U dts:1;                 /* Data Toggle Synch Enable */
  INT8U ninc:1;                /* BD Keep Enable */
  INT8U keep:1;                /* Address Increment Disable */
  INT8U data:1;                /* Data Toggle Synch Value */
  INT8U own:1;                 /* USB Ownership */
}
T_EP_CBL_CBITS; 

typedef struct  
{
  INT8U    :2;
  INT8U pid:4;                 /* Packet Identifier */
  INT8U    :2;
}
T_EP_CBL_PID;

typedef union 
{
  INT8U             all;
  T_EP_CBL_CBITS    cbits;
  T_EP_CBL_PID      pack_pid;
} 
T_EP_CBL_BITS;                            /* Buffer Descriptor Status Register */

// Структура управляющего блока - элемента в таблице Buffer Descriptor Table  
typedef struct 
{
  T_EP_CBL_BITS  bits;
  INT8U          reserved1;
  INT16U         cnt;                  /* Count of bytes recieved or sent */
  INT32U         addr;                 /* Buffer Address */
} 
T_EP_CBL;              

// Структура таблицы Buffer Descriptor Table 
// В данной таблице находятся управляющие блоки конечных точек
// Для каждой конечной точки создается 4-е управляющих блока
// Назначается по два блока(четный и нечетный) для транзакций приема и транзакций передачи. 
typedef struct  
{
  T_EP_CBL ep_cbl[MAX_EP_CBL_INDEX];     /* Endpoint Descriptor */
}
T_USB_BDT;


  #pragma pack()

#endif
