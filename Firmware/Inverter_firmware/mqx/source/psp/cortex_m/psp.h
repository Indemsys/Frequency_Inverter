/**HEADER********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: psp.h$
* $Version : 3.7.3.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file provides a generic header file for use by the mqx kernel
*   for including platform specific information and macros
*
*END************************************************************************/

#ifndef __psp_h__
#define __psp_h__

#include <mqx_cnfg.h>

#ifndef __ASM__

#include <psptypes.h>

#include <psp_time.h>
#include <psp_math.h>
#include <psp_comp.h>

#include <mqx_ioc.h>

#endif // __ASM__

#include <psp_cpu.h>



#endif
/* EOF */
