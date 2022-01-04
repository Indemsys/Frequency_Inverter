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
* $FileName: io_prv.h$
* $Version : 3.6.4.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file includes the private definitions for the I/O subsystem.
*
*END************************************************************************/
#ifndef __io_prv_h__
#define __io_prv_h__

/*--------------------------------------------------------------------------*/
/*
 *                            CONSTANT DEFINITIONS
 */


/* Flag meanings */

/* Is the stream at EOF? */
#define IO_FLAG_TEXT        (4)
#define IO_FLAG_AT_EOF      (8)

/* Maximum name check length */
#define IO_MAXIMUM_NAME_LENGTH (1024)

/*--------------------------------------------------------------------------*/
/*
 *                            DATATYPE DECLARATIONS
 */

/*
 * FILE DEVICE STRUCTURE
 *
 * This structure is used by the current I/O Subsystem to store
 * state information.
 * Use the same structure as the formatted I/O.
 */
typedef MQX_FILE FILE_DEVICE_STRUCT, _PTR_ FILE_DEVICE_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
 *                            FUNCTION PROTOTYPES
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
