#ifndef __select_h__
#define __select_h__
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
* $FileName: select.h$
* $Version : 3.8.6.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the defines, externs and data
*   structure definitions required by the select function.
*
*END************************************************************************/


#define RTCS_SELECT_MAX_FDS 32
#define RTCS_SELECT_ERROR   (-1)

#define select   RTCS_select
#define FD_CLR   RTCS_FD_CLR
#define FD_ISSET RTCS_FD_ISSET
#define FD_SET   RTCS_FD_SET
#define FD_ZERO  RTCS_FD_ZERO


typedef struct {
    uint_32 COUNT;
    uint_32 FDS[RTCS_SELECT_MAX_FDS];
} fd_set, _PTR_ fd_set_ptr;


struct timeval { 
    uint_32    tv_sec;         /* seconds */
    uint_32    tv_usec;        /* microseconds */
};

extern boolean RTCS_FD_CLR(uint_32 fd, fd_set _PTR_ set_ptr ); 
extern boolean RTCS_FD_ISSET(uint_32 fd, fd_set _PTR_ set_ptr ); 
extern boolean RTCS_FD_SET(uint_32 fd, fd_set _PTR_ set_ptr ); 
extern boolean RTCS_FD_ZERO(fd_set _PTR_ set_ptr );
extern boolean RTCS_FD_COPY(fd_set _PTR_ src_ptr, fd_set _PTR_ dest_ptr );
extern int_32  RTCS_select(int_32 n, fd_set _PTR_ readfds, fd_set _PTR_ writefds, fd_set _PTR_ exceptfds, struct timeval *timeout); 

#endif

/* EOF */
