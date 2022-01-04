/*HEADER********************************************************************
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
* $FileName: lwtimer_prv.h$
* $Version : 3.8.0.1$
* $Date    : Feb-20-2012$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   light weight timer component's internal use.
*
*END************************************************************************/

#ifndef __lwtimer_prv_h__
#define __lwtimer_prv_h__ 1


/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/*! The timer validity check value */
#define LWTIMER_VALID             (_mqx_uint)(0x6C777469)    /* "lwti" */


/*--------------------------------------------------------------------------*/
/*                       PROTOTYPE DEFINITIONS                              */

#ifdef __cplusplus
extern "C" {
#endif


extern void _lwtimer_isr_internal(void);

#endif

#ifdef __cplusplus
}

#endif /* __lwtimer_prv_h__ */
/* EOF */

