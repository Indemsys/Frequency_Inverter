#ifndef __rtcsrtos_h__
#define __rtcsrtos_h__
/**HEADER********************************************************************
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
* $FileName: rtcsrtos.h$
* $Version : 3.0.8.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   This file contains the definitions required to
*   use RTCS with MQX.
*
*END************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* MQX is the only one supported */
#include <rtcs25x.h>


/***************************************
**
** Random Number Generation
*/

void     RTCS_rand_seed (uint_32);
uint_32  RTCS_rand      (void);


/***************************************
**
** The _iopcb_handle
*/

#ifndef __IOPCB__
#define __IOPCB__

#define IOPCB_IOCTL_S_ACCM       4
#define IOPCB_IOCTL_R_ACCM       5
#define IOPCB_IOCTL_GET_IFTYPE   6
#define IOPCB_IOCTL_S_PFC        14
#define IOPCB_IOCTL_R_PFC        15
#define IOPCB_IOCTL_S_ACFC       16
#define IOPCB_IOCTL_R_ACFC       17

typedef struct _iopcb_table {
   uint_32 (_CODE_PTR_ OPEN)  (struct _iopcb_table _PTR_, void (_CODE_PTR_)(pointer), void (_CODE_PTR_)(pointer), pointer);
   uint_32 (_CODE_PTR_ CLOSE) (struct _iopcb_table _PTR_);
   PCB_PTR (_CODE_PTR_ READ)  (struct _iopcb_table _PTR_, uint_32);
   void    (_CODE_PTR_ WRITE) (struct _iopcb_table _PTR_, PCB_PTR, uint_32);
   uint_32 (_CODE_PTR_ IOCTL) (struct _iopcb_table _PTR_, uint_32, pointer);
} _PTR_ _iopcb_handle;

#define _iopcb_open(h,up,down,param)   ((h)->OPEN(h,up,down,param))
#define _iopcb_close(h)                ((h)->CLOSE(h))
#define _iopcb_read(h,flags)           ((h)->READ(h,flags))
#define _iopcb_write(h,pcb,flags)      ((h)->WRITE(h,pcb,flags))
#define _iopcb_ioctl(h,opt,val)        ((h)->IOCTL(h,opt,val))

#endif




#ifdef __cplusplus
}
#endif


#endif
/* EOF */
