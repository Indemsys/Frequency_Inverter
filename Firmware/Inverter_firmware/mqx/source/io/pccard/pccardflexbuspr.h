#ifndef _pccardflexbuspr_h_
#define _pccardflexbuspr_h_
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
* $FileName: pccardflexbuspr.h$
* $Version : 3.8.10.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   The file contains the structure definitions
*   private to the PC Card FlexbBus
*
*END************************************************************************/

/* default options */
#ifndef BSP_PCCARDFLEXBUS_WIDTH
#define BSP_PCCARDFLEXBUS_WIDTH              1
#endif

#ifndef BSP_PCCARDFLEXBUS_ADDR_SHIFT
#define BSP_PCCARDFLEXBUS_ADDR_SHIFT         0
#endif

#ifndef BSP_PCCARDFLEXBUS_CARD_PRESENT
#define BSP_PCCARDFLEXBUS_CARD_PRESENT       229
#endif

#ifndef BSP_PCCARDFLEXBUS_SLOT_EMPTY
#define BSP_PCCARDFLEXBUS_SLOT_EMPTY         173
#endif

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#define PCCARDFLEXBUS_REGISTER_MASK          (0x00000000<<BSP_PCCARDFLEXBUS_ADDR_SHIFT)
#define PCCARDFLEXBUS_COMMON_MEM_MASK        (0x00001000<<BSP_PCCARDFLEXBUS_ADDR_SHIFT)
#define PCCARDFLEXBUS_CARD_PRESENT_MASK      (0x00002000<<BSP_PCCARDFLEXBUS_ADDR_SHIFT)



/*
** Timing parameters
*/

/* Minimum time to wait from VCC application to when the card is ready (ms) */
#define PCCARDFLEXBUS_RESET_ALLOWED_TIME  (300 + BSP_ALARM_RESOLUTION)

/* 
** State conditions 
*/
#define PCCARDFLEXBUS_CARD_REMOVED    0
#define PCCARDFLEXBUS_CARD_INSERTED   1
#define PCCARDFLEXBUS_ACCESS_ALLOWED  5


/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/

/*
** Run time state information for a PC Card slot
*/
typedef struct io_pccardflexbus_struct
{
   PCCARDFLEXBUS_INIT_STRUCT INIT_DATA;
   boolean                CARD_IN;
   uint_32                STATE;
   volatile uchar _PTR_   PCMCIA_BASE;
   volatile uchar _PTR_   ATTRIB_PTR;
   TIME_STRUCT            START_TIME;
} IO_PCCARDFLEXBUS_STRUCT, _PTR_ IO_PCCARDFLEXBUS_STRUCT_PTR;


#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_pccardflexbus_open(FILE_DEVICE_STRUCT_PTR, char _PTR_, char _PTR_);
extern _mqx_int _io_pccardflexbus_close(FILE_DEVICE_STRUCT_PTR);
extern _mqx_int _io_pccardflexbus_ioctl(FILE_DEVICE_STRUCT_PTR, _mqx_uint, pointer);

extern boolean   _io_pccardflexbus_card_detect(IO_PCCARDFLEXBUS_STRUCT_PTR);
extern uint_32   _io_pccardflexbus_read_tuple(volatile uchar _PTR_, uint_32, uchar_ptr);
extern void      _io_pccardflexbus_card_wait_till_ready(IO_PCCARDFLEXBUS_STRUCT_PTR);
extern void      _io_pccardflexbus_wait(IO_PCCARDFLEXBUS_STRUCT_PTR, uint_32);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
