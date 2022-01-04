/*HEADER*********************************************************************
*
* Copyright (c) 2008-2011 Freescale Semiconductor;
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
* $FileName: mqx_macros.h$
* $Version : 3.8.1.0$
* $Date    : Sep-1-2011$
*
* Comments:
*
*   This file contains generic macros
*
*END************************************************************************/

#ifndef __mqx_macros_h__
#define __mqx_macros_h__


/*--------------------------------------------------------------------------*/
/*      MACROS FOR CONVENIENT DATA STRUCTURE DECLARATION/MANIPULATION       */

#ifndef RESERVED_REGISTER
   #define RESERVED_REGISTER(start,next) uchar R_ ## start[next-start]
#endif

#ifndef ELEMENTS_OF
   #define ELEMENTS_OF(x) ( sizeof(x)/sizeof(x[0]) )
#endif

#ifndef MEM_ALIGN
   #define MEM_ALIGN(x,pow)  ( ( ((uint_32)x) + ((pow)-1)) & ~((pow)-1) )
#endif

#endif
