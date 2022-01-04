/*HEADER*********************************************************************
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
* $FileName: name.h$
* $Version : 3.0.5.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   name component.
*
*END************************************************************************/
#ifndef __name_h__
#define __name_h__ 1

#include <mqx_cnfg.h>
#if (! MQX_USE_NAME) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error NAME component is currently disabled in MQX kernel. Please set MQX_USE_NAME to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* Error codes */

#define NAME_TABLE_FULL             (NAME_ERROR_BASE|0x00)
#define NAME_EXISTS                 (NAME_ERROR_BASE|0x01)
#define NAME_NOT_FOUND              (NAME_ERROR_BASE|0x02)
#define NAME_TOO_LONG               (NAME_ERROR_BASE|0x03)
#define NAME_TOO_SHORT              (NAME_ERROR_BASE|0x04)

/* The maximum name size for a name component name */
#define NAME_MAX_NAME_SIZE          (32)

/* Default component creation parameters */
#define NAME_DEFAULT_INITIAL_NUMBER (8)
#define NAME_DEFAULT_GROW_NUMBER    (8)
#define NAME_DEFAULT_MAXIMUM_NUMBER (0) /* Unlimited */

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint _name_create_component(_mqx_uint, _mqx_uint, _mqx_uint);
extern _mqx_uint _name_add(char_ptr, _mqx_max_type);
extern _mqx_uint _name_delete(char_ptr);
extern _mqx_uint _name_find(char_ptr, _mqx_max_type_ptr);
extern _mqx_uint _name_find_by_number(_mqx_max_type, char_ptr);
extern _mqx_uint _name_test(pointer _PTR_, pointer _PTR_);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
