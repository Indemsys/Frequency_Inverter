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
* $FileName: lwlog_prv.h$
* $Version : 3.8.0.1$
* $Date    : Feb-22-2012$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   Log component.
*
*END************************************************************************/

#ifndef __lwlog_prv_h__
#define __lwlog_prv_h__ 1

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

#define LWLOG_VALID          ((_mqx_uint)(0x6C776C67))   /* "lwlg" */

/* Control bits in the control flags */
#define LWLOG_ENABLED        (0x1000)

/* Types of logs */
#define LWLOG_STATIC         (1)
#define LWLOG_DYNAMIC        (2)

/*--------------------------------------------------------------------------*/
/*                      DATA STRUCTURE DEFINITIONS                          */


/* LOG HEADER STRUCT */

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This structure is stored at the front of each log to provide information
 * about the current state of the log.
 */
typedef struct lwlog_header_struct
{

    /*! \brief Control flags for the log. */
    _mqx_uint                FLAGS;

    /*! \brief The sequence number for next write. */
    _mqx_uint                NUMBER;

    /*! \brief The number of entries currently in use. */
    _mqx_uint                CURRENT_ENTRIES;

    /*! \brief The maximum number of log entries. */
    _mqx_uint                MAX_ENTRIES;

    /*! \brief How many read nexts in row have been done. */
    _mqx_uint                READS;

    /*! \brief The type of log. */
    _mqx_uint                TYPE;

    /*! \brief Pointer to structure to write to. */
    LWLOG_ENTRY_STRUCT_PTR   WRITE_PTR;
    /*! \brief Pointer to structure to read from. */
    LWLOG_ENTRY_STRUCT_PTR   READ_PTR;
    /*! \brief Pointer to oldest log entry structure. */
    LWLOG_ENTRY_STRUCT_PTR   OLDEST_PTR;

    /*! \brief First log entry. */
    LWLOG_ENTRY_STRUCT       FIRST_ENTRY;

} LWLOG_HEADER_STRUCT, _PTR_ LWLOG_HEADER_STRUCT_PTR;
/*! \endcond */


/* LW LOG COMPONENT STRUCT */

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This structure is used to store information required for log retrieval.
 *
 * Its address is stored in the kernel component field of the kernel data structure.
 */
typedef struct lwlog_component_struct
{
    /*! \brief A validation stamp to verify structure correctness. */
    _mqx_uint               VALID;

    /*! \brief The address of the log headers. */
    LWLOG_HEADER_STRUCT_PTR LOGS[LOG_MAXIMUM_NUMBER];

} LWLOG_COMPONENT_STRUCT, _PTR_ LWLOG_COMPONENT_STRUCT_PTR;
/*! \endcond */

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _lwlog_create_internal(_mqx_uint, _mqx_uint, _mqx_uint,
    LWLOG_HEADER_STRUCT_PTR);
extern _mqx_uint _lwlog_write_internal(_mqx_uint, _mqx_max_type, _mqx_max_type,
    _mqx_max_type, _mqx_max_type, _mqx_max_type, _mqx_max_type, _mqx_max_type);

#ifdef __cplusplus
}
#endif

#endif /* __lwlog_prv_h__ */
/* EOF */
