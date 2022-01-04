#ifndef __telnet_h__
#define __telnet_h__
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
* $FileName: telnet.h$
* $Version : 3.8.5.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains the definitions for the Telnet
*   client and server.
*
*END************************************************************************/

/* telnet command codes */
#define TELCMD_IP       '\xf4'   /* Interrupt process */
#define TELCMD_AO       '\xf5'   /* Abort Output */
#define TELCMD_AYT      '\xf6'   /* Are You There */
#define TELCMD_EC       '\xf7'   /* Erase Char */
#define TELCMD_EL       '\xf8'   /* Erase Line */
#define TELCMD_GA       '\xf9'   /* Process ready for input */
#define TELCMD_WILL     '\xfb'
#define TELCMD_WONT     '\xfc'
#define TELCMD_DO       '\xfd'
#define TELCMD_DONT     '\xfe'
#define TELCMD_IAC      '\xff'   /* TELNET command sequence header */

/* telnet mandatory control codes */
#define TELCC_NULL      '\0'
#define TELCC_LF        '\n'
#define TELCC_CR        '\r'

/* telnet optional control codes */
#define TELCC_BELL      '\x07'   /* BELL */
#define TELCC_BS        '\x08'   /* BACKSPACE */
#define TELCC_HT        '\x09'   /* HORIZONTAL TAB */
#define TELCC_VT        '\x0b'   /* VERTICAL TAB */
#define TELCC_FF        '\x0c'   /* FORM FEED */


#define TELOPT_BINARY   0        /* Use 8 bit binary transmission */
#define TELOPT_ECHO     1        /* Echo data received */
#define TELOPT_SGA      3        /* Suppress Go-ahead signal */

#define TELNET_MAX_OPTS 4

/* Terminal options */
#define TEROPT_CRLF_MODE 1       /* New line endings style CR LF as opposed to CR NULL */
#define TEROPT_RAW_MODE  2       /* Indicates that raw mode without any char translation is used */


/* set the proper stream for the device */
#define IO_IOCTL_SET_STREAM             0x0050L
#define IO_IOCTL_SET_ECHO               0x0051L
#define IO_IOCTL_GET_ECHO               0x0052L
#define IO_IOCTL_SET_INBINARY           0x0053L
#define IO_IOCTL_GET_INBINARY           0x0054L
#define IO_IOCTL_SET_OUTBINARY          0x0055L
#define IO_IOCTL_GET_OUTBINARY          0x0056L
#define IO_IOCTL_SET_BINARY             0x0057L
#define IO_IOCTL_GET_BINARY             0x0058L
#define IO_IOCTL_SET_CRLF               0x0059L
#define IO_IOCTL_GET_CRLF               0x005aL
#define IO_IOCTL_SET_RAW                0x005bL
#define IO_IOCTL_GET_RAW                0x005cL

#endif
/* EOF */
