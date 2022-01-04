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
* $FileName: log_prv.h$
* $Version : 3.8.4.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   Log component.
*
*END*************************************************************************/

#ifndef __log_prv_h__
#define __log_prv_h__ 1


/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

#define LOG_VALID           ((_mqx_uint)(0x6c6f6720))   /* "log " */

/* Control bits in the control flags */
#define LOG_ENABLED         (0x1000)


/*--------------------------------------------------------------------------*/
/*                      DATA STRUCTURE DEFINITIONS                          */

/*  LOG HEADER STRUCT */
/*!
 * \cond DOXYGEN_PRIVATE
 *  
 * \brief This structure is stored at the front of each log to provide information
 * about the current state of the log.
 */
typedef struct log_header_struct
{
    /*! \brief Control flags for the log. */
    uint_32       FLAGS;

    /*! \brief The sequence number for next write. */
    _mqx_uint     NUMBER;

    /*! \brief Where next log is to be written. */
    _mqx_uint_ptr LOG_WRITE;

    /*! \brief Where first log is to be read. */
    _mqx_uint_ptr LOG_READ;

    /*! \brief Where last log was written. */
    _mqx_uint_ptr LAST_LOG;

    /*! \brief Starting address of data. */
    _mqx_uint_ptr LOG_START;

    /*! \brief Ending address of data. */
    _mqx_uint_ptr LOG_END;

    /*! \brief The next log to read. */
    _mqx_uint_ptr LOG_NEXT;

    /*! \brief Current size of data. */
    _mqx_uint     SIZE;

    /*! \brief Maximum size of data. */
    _mqx_uint     MAX;

    /*! \brief Variable array of data. */
    _mqx_uint     DATA[1];

} LOG_HEADER_STRUCT, _PTR_ LOG_HEADER_STRUCT_PTR;
/*! \endcond */


/*  LOG COMPONENT STRUCT */
/*!
 * \cond DOXYGEN_PRIVATE
 * 
 * \brief  This structure is used to store information required for log retrieval.
 * 
 * Its address is stored in the kernel component field of the kernel data structure.
 */
typedef struct log_component_struct
{
    /*! \brief A validation stamp to verify handle correctness. */
    _mqx_uint             VALID;

    /*! \brief The address of the log headers. */
    LOG_HEADER_STRUCT_PTR LOGS[LOG_MAXIMUM_NUMBER];

} LOG_COMPONENT_STRUCT, _PTR_ LOG_COMPONENT_STRUCT_PTR;
/*! \endcond */

#endif
/* EOF */
