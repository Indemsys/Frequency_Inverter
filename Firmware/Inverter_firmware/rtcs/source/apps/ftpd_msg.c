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
* $FileName: ftpd_msg.c$
* $Version : 3.8.9.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains an implementation of an
*   FTP server.
*
*END************************************************************************/


/* Response messages */
const char ftpmsg_sending[]   = "150 Opening data connection for RETR (%ld,%ld,%ld,%ld,%hd)\r\n";
const char ftpmsg_typeok[]    = "200 Type OK\r\n";
const char ftpmsg_portok[]    = "200 Port command okay\r\n";
const char ftpmsg_banner[]    = "220 RTCS FTP Server Ready\r\n";
const char ftpmsg_bye[]       = "221 Goodbye!\r\n";
const char ftpmsg_rxok[]      = "226 File received OK - %ld bytes in %ld.%03ld sec\r\n";
const char ftpmsg_txok[]      = "226 File sent OK - %ld bytes in %ld.%03ld sec\r\n";
const char ftpmsg_logged[]    = "230 Logged in\r\n";
const char ftpmsg_pass[]      = "230 Password ok, User logged in.\r\n";
const char ftpmsg_noconn[]    = "425 Data connection refused\r\n";
const char ftpmsg_writefail[] = "426 write error\r\n";
const char ftpmsg_writeeof[]  = "426 device full\r\n";
const char ftpmsg_badcmd[]    = "500 Unknown command\r\n";
const char ftpmsg_badtype[]   = "501 Unknown type \"%s\"\r\n";
const char ftpmsg_badport[]   = "501 Bad port syntax\r\n";
const char ftpmsg_badsyntax[] = "501 Incorrect command syntax\r\n";
const char ftpmsg_unimp[]     = "502 Command not implemented\r\n";
const char ftpmsg_cantopen[]  = "550 Can't open file \"%s\"\r\n";

const char ftpmsg_size[]            = "226 File size is %d.\r\n";
const char ftpmsg_syntax_error[]    = "501 Syntax error in parameters or arguments.\r\n";
const char ftpmsg_need_password[]   = "331 User name okay, need password.\r\n";   
const char ftpmsg_not_logged[]      = "530 Not logged in.\r\n";
const char ftpmsg_goodbye[]         = "221 Goodbye.\r\n";
const char ftpmsg_no_fs[]           = "550 Requested action not taken. File system not mounted.\r\n";
const char ftpmsg_pasv_mode[]       = "227 Entering Passive Mode (%ld,%ld,%ld,%ld,%hd,%hd).\r\n";
const char ftpmsg_site_info[]       = "200 RTCS FTPd: No site specific information.\r\n";
const char ftpmsg_bad_option[]      = "501 Option not supported.\r\n";
const char ftpmsg_syst[]            = "215 MQX\r\n";
const char ftpmsg_help_start[]      = "214- The following commands are recognized:\r\n";
const char ftpmsg_help_end[]        = "214 Direct comments to Embedded Access Inc.\r\n";
const char ftpmsg_pwd_error[]       = "521 Error getting current directory.\r\n";
const char ftpmsg_cd_error[]        = "521 Error changing directory.\r\n";
const char ftpmsg_rmdir_error[]     = "521 Error removing directory.\r\n";
const char ftpmsg_delete_error[]    = "521 Error deleting file.\r\n";
const char ftpmsg_mkdir_error[]     = "521 Error creating directory.\r\n";
const char ftpmsg_ok[]              = "200 OK.\r\n";
const char ftpmsg_type_ascii[]      = "200 Type ASCII.\r\n";
const char ftpmsg_type_image[]      = "200 Type Binary.\r\n";
const char ftpmsg_type_unknown[]    = "501 Unknown type.\r\n";
const char ftpmsg_no_file[]         = "550 Requested action not taken. File unavailable.\r\n";
const char ftpmsg_opening_datacon[] = "150 Opening data connection.\r\n";
const char ftpmsg_tx_complete[]     = "226 Transfer complete.\r\n";
const char ftpmsg_no_datacon[]      = "425 Data connection refused.\r\n";
const char ftpmsg_no_memory[]       = "550 Requested action not taken. Memory unavailable.\r\n";

/* EOF*/
