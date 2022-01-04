#ifndef __esdhc_prv_h__ 
#define __esdhc_prv_h__
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
* $FileName: esdhc_prv.h$
* $Version : 3.8.6.0$
* $Date    : Jul-19-2012$
*
* Comments:
*
*   This file contains definitions private to the ESDHC driver.
*
*END************************************************************************/

#include "esdhc.h"

/*--------------------------------------------------------------------------*/
/*
**                    CONSTANT DEFINITIONS
*/

#define IO_ESDHC_ATTRIBS (IO_DEV_ATTR_READ | IO_DEV_ATTR_REMOVE | IO_DEV_ATTR_SEEK | IO_DEV_ATTR_WRITE | IO_DEV_ATTR_BLOCK_MODE)

#define ESDHC_PROCTL_EMODE_BIG               (0x00)
#define ESDHC_PROCTL_EMODE_LITTLE            (0x02)

#define ESDHC_PROCTL_DTW_1BIT                (0x00)
#define ESDHC_PROCTL_DTW_4BIT                (0x01)
#define ESDHC_PROCTL_DTW_8BIT                (0x10)


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** ESDHC_INFO_STRUCT
** ESDHC run time state information
*/
typedef struct esdhc_info_struct
{
   /* The actual card status */
   uint_32                        CARD;
   
} ESDHC_INFO_STRUCT, _PTR_ ESDHC_INFO_STRUCT_PTR;


/*
** ESDHC_DEVICE_STRUCT
** ESDHC install parameters
*/
typedef struct esdhc_device_struct
{
   /* The current init values for this device */
   ESDHC_INIT_STRUCT_CPTR INIT;
   
   /* The number of opened file descriptors */
   uint_32                        COUNT;

} ESDHC_DEVICE_STRUCT, _PTR_ ESDHC_DEVICE_STRUCT_PTR;
    
/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _esdhc_open (MQX_FILE_PTR, char_ptr, char_ptr);
extern _mqx_int _esdhc_close (MQX_FILE_PTR);
extern _mqx_int _esdhc_read (MQX_FILE_PTR, char_ptr, int_32);
extern _mqx_int _esdhc_write (MQX_FILE_PTR, char_ptr, int_32);
extern _mqx_int _esdhc_ioctl (MQX_FILE_PTR, uint_32, pointer);
extern _mqx_int _esdhc_uninstall (IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif
      
#endif
/* EOF */
