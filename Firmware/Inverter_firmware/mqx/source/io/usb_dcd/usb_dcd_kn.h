#ifndef _usb_dcd_kn_h
#define _usb_dcd_kn_h 1

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
* $FileName: usb_dcd_kn.h$
* $Version : 3.8.1.0$
* $Date    : Jul-27-2011$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the DCD I/O functions.
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** KUSB_DCD_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a USB DCD is initialized.
*/
typedef struct kusb_dcd_init_struct
{
   /* Clock speed in Khz */
   uint_32                CLOCK_SPEED;
   
   /* Sequence Initiation Time */
   uint_16                TSEQ_INIT;

   /* Time Period to Debounce D+ Signal */
   uint_16                TDCD_DBNC;
   
   /* Time Period to Comparator Enabled */
   uint_16                TVDPSRC_ON;
   
   /* Time Period Before Enabling D+ Pullup */
   uint_16                TVDPSRC_CON;
   
   /* Time Before Check of D- Line */
   uint_16                CHECK_DM;

   /* Interrupt level to use if interrupt driven */
   _int_level             LEVEL;

} KUSB_DCD_INIT_STRUCT, _PTR_ KUSB_DCD_INIT_STRUCT_PTR;

typedef const KUSB_DCD_INIT_STRUCT _PTR_ KUSB_DCD_INIT_STRUCT_CPTR;
                                           

/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint_32 _kusb_dcd_polled_install (char_ptr, KUSB_DCD_INIT_STRUCT_CPTR);
extern uint_32 _kusb_dcd_int_install (char_ptr, KUSB_DCD_INIT_STRUCT_CPTR);
extern pointer _bsp_get_usb_dcd_base_address (void);
extern uint_32 _bsp_get_usb_dcd_vector (void);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
