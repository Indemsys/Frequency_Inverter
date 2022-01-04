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
* $FileName: mqx_ioc.h$
* $Version : 3.5.11.0$
* $Date    : Jan-22-2010$
*
* Comments:
*
*   This include file is used to define constants and data types for the
*   support functions for MQX I/O Components.
*
*END************************************************************************/
#ifndef __mqx_ioc_h__
#define __mqx_ioc_h__ 1

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */


/*
 * The IO component indexes, used to index into the component
 * arrays to access component specific data
 */
#define IO_SUBSYSTEM_COMPONENT        (0)
#define IO_RTCS_COMPONENT             (1)
#define IO_SNMP_COMPONENT             (2)
#define IO_MFS_COMPONENT              (3)
#define IO_USB_COMPONENT              (4)
#define IO_SHELL_COMPONENT            (5)
#define IO_ENET_COMPONENT             (6)
#define IO_EDS_COMPONENT              (7)
//#define IO_LAPB_COMPONENT             (2)  // obsolete
//#define IO_LAPD_COMPONENT             (3)  // obsolete
//#define IO_SDLC_COMPONENT             (4)  // obsolete
//#define IO_HDLC_COMPONENT             (5)
//#define IO_MFS_COMPONENT              (6)
//#define IO_CAN_COMPONENT              (7)
//#define IO_PPP_COMPONENT              (8)
//#define IO_SNMP_COMPONENT             (9)
//#define IO_EDS_COMPONENT              (10)
//#define IO_USB_COMPONENT              (11)
//#define IO_SHELL_COMPONENT            (12)

/* The maximum number of IO components */
#define MAX_IO_COMPONENTS                  (8)

/*--------------------------------------------------------------------------*/
/*                        DATATYPE DECLARATIONS                             */


/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifndef __ASM__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern pointer _mqx_get_io_component_handle(_mqx_uint);
extern pointer _mqx_set_io_component_handle(_mqx_uint, pointer);
extern uint_32 _mqx_link_io_component_handle(_mqx_uint, pointer, pointer *);
extern uint_32 _mqx_unlink_io_component_handle(_mqx_uint, pointer, pointer *);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ASM__ */

#endif
/* EOF */
