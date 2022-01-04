#ifndef __rtcstime_h__
#define __rtcstime_h__
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
* $FileName: rtcstime.h$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   TCP/IP internal Time Server definitions
*
*END************************************************************************/


/***************************************
**
** Type definitions
**
*/

/*
** A timer event
*/
typedef struct tcpip_event {

   uint_32                    TIME;
   boolean (_CODE_PTR_        EVENT)(struct tcpip_event _PTR_);
   pointer                    PRIVATE;

   struct tcpip_event _PTR_   NEXT;

} TCPIP_EVENT, _PTR_ TCPIP_EVENT_PTR;


/***************************************
**
** Prototypes
**
*/

void TCPIP_Event_init(void);

void TCPIP_Event_add(TCPIP_EVENT_PTR);
void TCPIP_Event_cancel(TCPIP_EVENT_PTR);

uint_32 TCPIP_Event_expire(TCPIP_EVENT_PTR);

uint_32 TCPIP_Event_time(uint_32);

#define TCPIP_Event_launch(var, time, event, private) \
         {                             \
            (var)->TIME = time;        \
            (var)->EVENT = event;      \
            (var)->PRIVATE = private;  \
            TCPIP_Event_add(var);      \
         }

#endif
/* EOF */
