#ifndef __snmpcfg_h__
#define __snmpcfg_h__
/**HEADER********************************************************************
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
* $FileName: snmpcfg.h$
* $Version : 3.6.5.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the definitions for configuring
*   optional features in SNMP.
*   SNMP is delivered as follows:
*   SNMPCFG_BUFFER_SIZE            512
*   SNMPCFG_NUM_COMMUNITY           2
*   SNMPCFG_COMMUNITY_LIST         "public" , "private"
*   SNMPCFG_MAX_TRAP_RECEIVERS      6
*   SNMPCFG_SEND_V2_TRAPS           NOT DEFINE
*   SNMPCFG_SYSDESCR               "RTCS version <version>"
*   SNMPCFG_SYSSERVICES            8 (end-to-end)
*   SNMPCFG_HAVE_INT64             0
*
*END************************************************************************/

/*
** SNMP has two static buffers (one for receiving and one for generating
** responses), each of size SNMPCFG_BUFFER_SIZE.  RFCs 1157 and 1906 require
** this number to be at least 484.
*/
#ifndef SNMPCFG_BUFFER_SIZE
#define SNMPCFG_BUFFER_SIZE   512
#endif

/*
** The number of communities, used in both SNMPv1 and SNMPv2c.
*/
#ifndef SNMPCFG_NUM_COMMUNITY
#define SNMPCFG_NUM_COMMUNITY 2
#endif


/* 
** The string of communities, used in both SNMPv1 and SNMPv2c.
** Communities can be added to the list at compile time
*/
#ifndef SNMPCFG_COMMUNITY_LIST
#define SNMPCFG_COMMUNITY_LIST {"public", "private"} 
#endif

/* 
** Size of TRAP Receivers table - increase if sending traps to more than 6 nodes 
*/
#ifndef SNMPCFG_MAX_TRAP_RECEIVERS
#define SNMPCFG_MAX_TRAP_RECEIVERS 6
#endif

/* 
** Send v2 traps?  If not defined, send v1 traps instead 
*/
#define SNMPCFG_SEND_V2_TRAPS

/*
** The value of the system.sysDescr variable.
*/
#ifndef SNMPCFG_SYSDESCR
#define SNMPCFG_NUMTOSTR(x)   #x
#define SNMPCFG_NUMTOSTR2(x)  SNMPCFG_NUMTOSTR(x)
#define SNMPCFG_SYSDESCR      "RTCS version " SNMPCFG_NUMTOSTR2(RTCS_VERSION_MAJOR) \
                              "." SNMPCFG_NUMTOSTR2(RTCS_VERSION_MINOR) \
                              "." SNMPCFG_NUMTOSTR2(RTCS_VERSION_REV)
#endif

/*
** The value of the system.sysServices variable.
*/
#ifndef SNMPCFG_SYSSERVICES
#define SNMPCFG_SYSSERVICES   8
#endif

/*
** SNMPCFG_HAVE_INT64 enables support for 64-bit integers.
*/
#ifndef SNMPCFG_HAVE_INT64
#define SNMPCFG_HAVE_INT64    0
#endif

/* To traverse the MIB in the SNMP task instead
   of the RTCS task, uncomment the line below.  */
// #define TRAVERSE_MIB_IN_SNMP_TASK 1
#ifdef TRAVERSE_MIB_IN_SNMP_TASK
#warning Do not access RTCS OIDs when traversing MIB in snmp task
#endif

#endif
/* EOF */
