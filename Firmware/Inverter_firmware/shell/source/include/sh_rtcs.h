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
* $FileName: sh_rtcs.h$
* $Version : 3.8.11.0$
* $Date    : Oct-1-2012$
*
* Comments:
*
*   This file contains the RTCS shell.
*
*END************************************************************************/

#ifndef __sh_rtcs_h__
#define __sh_rtcs_h__

#include "ppp.h"

#define MAX_HOSTNAMESIZE     64


#define eaddrassign(p,x)   ((p)[0] = (x)[0], \
                            (p)[1] = (x)[1], \
                            (p)[2] = (x)[2], \
                            (p)[3] = (x)[3], \
                            (p)[4] = (x)[4], \
                            (p)[5] = (x)[5]  \
                           )

#define eaddriszero(p)   ( ((p)[0] == 0) && \
                           ((p)[1] == 0) && \
                           ((p)[2] == 0) && \
                           ((p)[3] == 0) && \
                           ((p)[4] == 0) && \
                           ((p)[5] == 0)    \
                          )

/*
** Function prototypes 
*/
#ifdef __cplusplus
extern "C" {
#endif

typedef struct shell_ppp_link { 
   char*           PPP_CONNECTION_DEVICE;   /* system device name, like "ittyd:" */
   MQX_FILE_PTR    PPP_CON_DEV_HANDLE;      /* Open serial device. */
   MQX_FILE_PTR    PPP_DEV_HANDLE;          /* Open our ppp driver on p_dev_file connection device. */
  _ppp_handle      PPP_HANDLE;              /* PPP connection handle. */
  _rtcs_if_handle  PPP_IF_HANDLE;           /* PPP interface. */
  _iopcb_handle    PPP_IO_DRIVER_HANDLE;    /* An iopcb driver. */
  _ip_address      PPP_LOCAL_ADDRESS;       /* Address will be assigned by remote host for client mode. */
  _ip_address      PPP_REMOTE_ADDRESS;      /* Address will be assigned by remote host for client mode. */
  _ip_address      PPP_GATE_ADDR;
   char*           LOCAL_NAME;              /* Board as client login name. */
   char*           LOCAL_PASSWORD;          /* Board as client password.  */
   char*           REMOTE_NAME;             /* Board as server connect login name. */
   char*           REMOTE_PASSWORD;         /* Board as server connecr password. */
   PPP_SECRET      rsecrets[2];             /* Save here PAP information. */
   LWSEM_STRUCT    PPP_SEM;                 /* PPP semafor. */
   uint_32         SERVER_FLAG;             /* Set it to "1" if we start PPP like server or "0" if we start PPP like client. */
} SHELL_PPP_LINK, _PTR_ SHELL_PPP_LINK_PTR;


   

extern boolean Shell_parse_ip_address( char_ptr arg, _ip_address _PTR_ ipaddr_ptr);
extern boolean Shell_parse_netmask( char_ptr arg, _ip_address _PTR_ ipaddr_ptr);

extern int_32 Shell_netstat(int_32 argc, char_ptr argv[] );
extern int_32 Shell_EDS(int_32 argc, char_ptr argv[] ); 
extern int_32 Shell_ping(int_32 argc, char_ptr argv[] ); 
extern int_32 Shell_Telnet_client(int_32 argc, char_ptr argv[] );
extern int_32 Shell_FTP_client(int_32 argc, char_ptr argv[] );
extern int_32 Shell_TFTP_client(int_32 argc, char_ptr argv[] );
extern int_32 Shell_get_host_by_name(int_32 argc, char_ptr argv[] ); 
extern int_32 Shell_Telnetd(int_32 argc, char_ptr argv[] );
extern int_32 Shell_Echod(int_32 argc, char_ptr argv[] );
extern int_32 Shell_SNMPd(int_32 argc, char_ptr argv[] );
extern int_32 Shell_FTPd(int_32 argc, char_ptr argv[] );
extern int_32 Shell_TFTPd(int_32 argc, char_ptr argv[] );
extern int_32 Shell_type(int_32 argc, char_ptr argv[] );
extern int_32 Shell_exec(int_32 argc, char_ptr argv[] );
extern int_32 Shell_load(int_32 argc, char_ptr argv[] );
extern int_32 Shell_walkroute(int_32 argc, char_ptr argv[] );
extern int_32 Shell_gate(int_32 argc, char_ptr argv[] ); 
extern int_32 Shell_getroute(int_32 argc, char_ptr argv[] );


extern int_32 Shell_arpdisp(int_32 argc, char_ptr argv[] );
extern int_32 Shell_arpadd(int_32 argc, char_ptr argv[] );
extern int_32 Shell_arpdel(int_32 argc, char_ptr argv[] );
extern int_32 Shell_sendto(int_32 argc, char_ptr argv[] );



extern int_32 Shell_nat(int_32 argc, char_ptr argv[] );
extern int_32 Shell_dnat(int_32 argc, char_ptr argv[] );
extern int_32 Shell_natinfo(int_32 argc, char_ptr argv[] );
extern int_32 Shell_natinit(int_32 argc, char_ptr argv[] );
extern int_32 Shell_dnat_delet_rule(int_32 argc, char_ptr argv[] );
extern int_32 Shell_dhcptest(int_32 argc, char_ptr argv[] );

extern int_32 Shell_Clock_server_start(int_32 argc, char_ptr argv[] );

extern int_32 Shell_ipconfig(int_32 argc, char_ptr argv[] );
extern int_32 Shell_iwconfig(int_32 argc, char_ptr argv[] );

/* PPP related shell commands. */
extern int_32 Shell_ppp(int_32 argc, char_ptr argv[] );


#ifdef __cplusplus
}
#endif

#endif
/*EOF*/
