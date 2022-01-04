#ifndef __ftp_h__
#define __ftp_h__
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
* $FileName: ftp.h$
* $Version : 3.6.4.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the FTP server definitions.
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/

#define IMAGE_TYPE 0
#define ASCII_TYPE 1

/* Size configuration */
#define LINELEN      128   /* Length of command buffer */
#define BUF_SIZE     1460

/* Command table commands */
#define USER_CMD     0
#define ACCT_CMD     1
#define PASS_CMD     2
#define TYPE_CMD     3
#define NAME_CMD     4
#define QUIT_CMD     5
#define PORT_CMD     6
#define RETR_CMD     7
#define STOR_CMD     8
#define HELP_CMD     9
#define ABORT_CMD    10

/* Start CR 1415 */
#define CWD_CMD     0 
#define CDUP_CMD    1
#define LIST_CMD    2 
#define DELE_CMD    3
#define MKD_CMD     4
#define XMKD_CMD    5
#define PWD_CMD     6   
#define XPWD_CMD    7
#define RMD_CMD     8
#define XRMD_CMD    9
/* End CR 1415 */

/***************************************
**
** Prototypes
**
*/

extern void    FTPSRV_process_connection( uint_32, uint_32 );
extern boolean FTPSRV_r_ftp( uint_32 );
extern boolean FTPSRV_docommand( uint_32, char _PTR_);

extern boolean FTPSRV_r_ftpd(uint_32);
extern boolean FTPSRV_s_ftpd(uint_32);
extern int_32  FTPSRV_pport(sockaddr_in _PTR_, char _PTR_);
extern void    FTPSRV_print_help(uint_32);
extern void    FTPSRV_tputs( uint_32, char _PTR_);
/* Start CR 1415, 1802, 2137 */
extern void    FTPSRV_openDataConnection(uint_32, char _PTR_);
/* End CR 1415, 1802, 2137 */

#endif
/* EOF */
