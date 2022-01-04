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
* $FileName: select.c$
* $Version : 3.8.10.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the RTCS select() support functions.
*
*END************************************************************************/

#include <rtcs.h>

#include <rtcs.h>
#include <select.h>

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_FD_CLR
* Returned Values :
* Comments  :
*     Removes an fd (socket) from a set
*
*END*-----------------------------------------------------------------*/

boolean RTCS_FD_CLR(uint_32 fd, fd_set _PTR_ set_ptr ) 
{ 
    uint_32 i;
    boolean cleared = FALSE;
    
   if ((fd==0) || (set_ptr == NULL)) return FALSE;
    
    for(i=0;i<set_ptr->COUNT;i++) { 
        if (set_ptr->FDS[i] == fd) {
 /* Start CR 2143 */       
            set_ptr->FDS[i] = set_ptr->FDS[--set_ptr->COUNT];
            set_ptr->FDS[set_ptr->COUNT] = 0;
 /* End CR 2143 */           
            cleared = TRUE;   
        }
    }
    return cleared;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_FD_ISSET
* Returned Values :
* Comments  :
*     Determines if an fd (socket) is in a set
*
*END*-----------------------------------------------------------------*/

boolean RTCS_FD_ISSET(uint_32 fd, fd_set _PTR_ set_ptr ) 
{ 
   uint_32 i;
   if ((fd==0) || (set_ptr == NULL)) return FALSE;
   
    for(i=0;i<set_ptr->COUNT;i++) { 
        if (set_ptr->FDS[i] == fd) {
            return TRUE;   
        }
    }
    return FALSE;
}
    
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_FD_SET
* Returned Values :
* Comments  :
*     Adds an fd (socket) to a set.
*
*END*-----------------------------------------------------------------*/

boolean RTCS_FD_SET(uint_32 fd, fd_set _PTR_ set_ptr ) 
{ 
   uint_32  i;
   
   if ((fd==0) || (set_ptr == NULL)) return FALSE;
   
   for(i=0;i<set_ptr->COUNT;i++) { 
      if (set_ptr->FDS[i] == fd) {
         return TRUE;   
      }
   }
   if (set_ptr->COUNT<RTCS_SELECT_MAX_FDS)  {
      set_ptr->FDS[set_ptr->COUNT++] = fd;
      return TRUE;
   } else  {
      return FALSE;
   }
}
    
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_FD_ZERO
* Returned Values :
* Comments  :
*     Zeros an fd set.
*
*END*-----------------------------------------------------------------*/

boolean RTCS_FD_ZERO(fd_set _PTR_ set_ptr ) 
{ 
   uint_32 i;
    
   if (set_ptr == NULL) return FALSE;
   
   set_ptr->COUNT = 0;
   for(i=0;i<RTCS_SELECT_MAX_FDS;i++) { 
      set_ptr->FDS[i] = 0;   
   }
   return TRUE;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_FD_COPY
* Returned Values :
* Comments  :
*     Copies an fd set.
*
*END*-----------------------------------------------------------------*/

boolean RTCS_FD_COPY(fd_set _PTR_ src_ptr, fd_set _PTR_ dest_ptr ) 
{ 
   uint_32 i;
    
   if ((src_ptr == NULL) || (src_ptr == NULL)) return FALSE;
   
   dest_ptr->COUNT = src_ptr->COUNT;
   for(i=0;i<RTCS_SELECT_MAX_FDS;i++) { 
      dest_ptr->FDS[i] = src_ptr->FDS[i];   
   }
   return TRUE;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_select
* Returned Values :
* Comments  :
*     RTCS version of BSD select call.
*
*END*-----------------------------------------------------------------*/
int_32 RTCS_select
   (
      int_32         n, 
      fd_set _PTR_   readfds, 
      fd_set _PTR_   writefds, 
      fd_set _PTR_   exceptfds, 
      struct timeval *timeout
    )  
{
   fd_set iread_set, iwrite_set;
   uint_32  milliseconds = 0;
   uint_32  sock;
   int i;
   
   // readfds and writefds are supported, only
   if (((readfds != NULL) || (writefds != NULL)) && (exceptfds != NULL)) {
      return RTCS_SELECT_ERROR;
   }
   
   RTCS_FD_ZERO(&iread_set);
   RTCS_FD_ZERO(&iwrite_set);

   if (readfds != NULL) {
      RTCS_FD_COPY(readfds, &iread_set);
   }

   if (writefds != NULL) {
      RTCS_FD_COPY(writefds, &iwrite_set);
   }
   
   if (timeout == NULL)  {
      /* Block indefinetly */
      milliseconds = 0;
   } else if ((timeout->tv_sec==0) && (timeout->tv_usec==0)) {
      /* Poll */
      milliseconds = (uint_32)-1;
   } else  {
      milliseconds = timeout->tv_sec*1000 + (timeout->tv_usec+999)/1000;
   }
   
   sock = _RTCS_select(&iread_set.FDS[0], iread_set.COUNT, &iwrite_set.FDS[0], iwrite_set.COUNT, milliseconds);
   
   if (sock == 0)  {
      return 0;
   } else if (sock == RTCS_SOCKET_ERROR )  {
      return RTCS_SELECT_ERROR;
   }
   
   RTCS_FD_ZERO(readfds);
   RTCS_FD_ZERO(writefds);
   
   for (i = 0; i < iread_set.COUNT; i++) {
      if (iread_set.FDS[i]) {
         RTCS_FD_SET(iread_set.FDS[i], readfds);
      }
   }
   
   for (i = 0; i < iwrite_set.COUNT; i++) {
      if (iwrite_set.FDS[i]) {
         RTCS_FD_SET(iwrite_set.FDS[i], writefds);
      }
   }
   
   return 1;
} 

/* EOF */
