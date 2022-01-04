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
* $FileName: psp_comp.h$
* $Version : 3.8.16.0$
* $Date    : Oct-5-2012$
*
* Comments:
*
*   This file determines which compiler is running, then includes
*   the compiler specific header file
*
*END************************************************************************/

#ifndef __psp_comp_h__
#define __psp_comp_h__

/* Include compiler specific header file */
#ifndef __ASM__
#include "psp_cpudef.h"
#include "comp.h"

#define PSP_TASK_PARAM(stack_start_ptr) (stack_start_ptr->PARAMETER)

#endif /* __ASM__ */

#endif /* __psp_comp_h__ */
/* EOF */
