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
* $FileName: log.h$
* $Version : 3.0.5.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   Log component.
*
*END************************************************************************/

#ifndef __log_h__
#define __log_h__ 1

#include <mqx_cnfg.h>

#if (!MQX_USE_LOGS) && (!defined __lwlog_h__) && (! defined (MQX_DISABLE_CONFIG_CHECK))
#error LOG component is currently disabled in MQX kernel. Please set MQX_USE_LOGS to 1 in user_config.h and recompile kernel.
#endif

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */


/* Maximum number of logs allowed */
#define LOG_MAXIMUM_NUMBER (16)

/*
 * The kernel system logging uses log number 0
 */
#define LOG_KERNEL_LOG_NUMBER      (0)

/* Configuration flags */
#define LOG_OVERWRITE              (1)

/* Types of log reads */
#define LOG_READ_NEWEST            (1)
#define LOG_READ_OLDEST            (2)
#define LOG_READ_NEXT              (3)
#define LOG_READ_OLDEST_AND_DELETE (4)

/* Error codes */
#define LOG_INVALID                (LOG_ERROR_BASE|0x00)
#define LOG_EXISTS                 (LOG_ERROR_BASE|0x01)
#define LOG_DOES_NOT_EXIST         (LOG_ERROR_BASE|0x02)
#define LOG_FULL                   (LOG_ERROR_BASE|0x03)
#define LOG_ENTRY_NOT_AVAILABLE    (LOG_ERROR_BASE|0x04)
#define LOG_DISABLED               (LOG_ERROR_BASE|0x05)
#define LOG_INVALID_READ_TYPE      (LOG_ERROR_BASE|0x06)
#define LOG_INVALID_SIZE           (LOG_ERROR_BASE|0x07)

/*--------------------------------------------------------------------------*/
/*                        DATATYPE DECLARATIONS                             */

/*                        LOG_ENTRY_STRUCT                                  */
/*!
 * \brief Header of an entry in a user log.
 *
 * The length of the entry depends on the SIZE field.
 *
 * \see _log_read
 * \see _log_write
 */

typedef struct log_entry_struct
{

   /*! \brief The size of this entry in _mqx_uints. */
   _mqx_uint    SIZE;

   /*! \brief The sequence number for the entry. */
   _mqx_uint    SEQUENCE_NUMBER;

   /*! \brief The time (in seconds) at which MQX wrote the entry. */
   uint_32      SECONDS;

   /*! \brief The time (in milliseconds) at which MQX wrote the entry. */
   uint_16      MILLISECONDS;

   /*! \brief The time (in microseconds) at which MQX wrote the entry. */
   uint_16      MICROSECONDS;

} LOG_ENTRY_STRUCT, _PTR_ LOG_ENTRY_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _log_create(_mqx_uint, _mqx_uint, uint_32);
extern _mqx_uint _log_create_component(void);
extern _mqx_uint _log_destroy(_mqx_uint);
extern _mqx_uint _log_disable(_mqx_uint);
extern _mqx_uint _log_enable(_mqx_uint);
extern _mqx_uint _log_read(_mqx_uint, _mqx_uint, _mqx_uint,
   LOG_ENTRY_STRUCT_PTR);
extern _mqx_uint _log_reset(_mqx_uint);
extern _mqx_uint _log_test(_mqx_uint _PTR_);
extern _mqx_uint _log_write(_mqx_uint, _mqx_uint, ... );

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
