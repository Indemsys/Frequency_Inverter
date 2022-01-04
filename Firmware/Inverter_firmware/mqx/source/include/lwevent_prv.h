/*HEADER**********************************************************************
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
******************************************************************************
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
******************************************************************************
*
* $FileName: lwevent_prv.h$
* $Version : 3.8.4.1$
* $Date    : Feb-16-2012$
*
* Comments:
*
*   This include file is used to define constants and data types private
*   to the event component.
*
*END*************************************************************************/

#ifndef __lwevent_prv_h__
#define __lwevent_prv_h__ 1

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* Used to mark a block of memory as belonging to an event group */
#define LWEVENT_VALID                  ((_mqx_uint)(0x6C65766E))  /* "levn" */

/*--------------------------------------------------------------------------*/
/*                         ANSI C PROTOTYPES                                */
#ifdef __cplusplus
extern "C" {
#endif

_mqx_uint _lwevent_create_internal  (LWEVENT_STRUCT_PTR, _mqx_uint, boolean);
_mqx_uint _lwevent_destroy_internal (LWEVENT_STRUCT_PTR, boolean);
_mqx_uint _lwevent_wait_internal    (LWEVENT_STRUCT_PTR, _mqx_uint,
                                     boolean, MQX_TICK_STRUCT_PTR, boolean);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
