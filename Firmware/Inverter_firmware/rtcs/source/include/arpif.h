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
* $FileName: arpif.h$
* $Version : 3.0.6.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   
*
*END************************************************************************/

#ifndef __arpif_h__
#define __arpif_h__

/* ARPIF_*() */
typedef struct arpif_parm {
   TCPIP_PARM           COMMON;
   _rtcs_if_handle      ihandle;
   _ip_address          PADDR;
   char        LADDR[6];
} ARPIF_PARM, _PTR_ ARPIF_PARM_PTR;

extern void ARPIF_delete
   (
      ARPIF_PARM_PTR  parms
   );
extern void ARPIF_add
   (
      ARPIF_PARM_PTR  parms
   );
extern void ARP_display_if_table(_rtcs_if_handle   ihandle);

#endif
