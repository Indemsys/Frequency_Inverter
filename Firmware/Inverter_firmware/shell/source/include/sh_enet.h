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
* $FileName: sh_enet.h$
* $Version : 3.0.2.0$
* $Date    : Jan-13-2009$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#ifndef __sh_enet_h__
#define __sh_enet_h__

#include <enet.h>


#define eaddrassign(p,x)   ((p)[0] = (x)[0], \
                           (p)[1] = (x)[1], \
                           (p)[2] = (x)[2], \
                           (p)[3] = (x)[3], \
                           (p)[4] = (x)[4], \
                           (p)[5] = (x)[5]  \
                          )

#define eaddriszero(p)   ( ((p)[0] == 0) && \
                           ((p)[1] == 0) && \
                           ((p)[2] == 0) && \
                           ((p)[3] == 0) && \
                           ((p)[4] == 0) && \
                           ((p)[5] == 0)    \
                          )

/*
** Function prototypes 
*/
#ifdef __cplusplus
extern "C" {
#endif

extern boolean Shell_parse_enet_address( char_ptr arg, _enet_address enet_address);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
