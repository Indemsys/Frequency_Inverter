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
* $FileName: telnetio.c$
* $Version : 3.8.14.3$
* $Date    : May-30-2012$
*
* Comments:
*
*   This file contains the implementation of an MQX
*   telnet I/O device.
*
*END************************************************************************/

#include <rtcs.h>
#include <fio.h>
#include <serial.h>
#include "telnet.h"

#define CRLF_MODE(io_ptr)        (io_ptr->TERMINAL_OPTIONS & TEROPT_CRLF_MODE)
#define RAW_MODE(io_ptr)         (io_ptr->TERMINAL_OPTIONS & TEROPT_RAW_MODE)

typedef struct io_telnet {
    MQX_FILE_PTR  NETWORK_FD;
    MQX_FILE_PTR  CONSOLE_FD;
    uchar         REMOTE_OPTIONS[TELNET_MAX_OPTS];
    uchar         LOCAL_OPTIONS[TELNET_MAX_OPTS];
    uint_32       TERMINAL_OPTIONS;
    uint_32       FLAGS;
    int_32        STATE;
} IO_TELNET, _PTR_ IO_TELNET_PTR;


_mqx_int _io_telnet_open  (MQX_FILE_PTR, char _PTR_,  char _PTR_);
_mqx_int _io_telnet_close (MQX_FILE_PTR);
_mqx_int _io_telnet_read  (MQX_FILE_PTR, char _PTR_, _mqx_int);
_mqx_int _io_telnet_write (MQX_FILE_PTR, char _PTR_, _mqx_int);
_mqx_int _io_telnet_ioctl (MQX_FILE_PTR, _mqx_uint, pointer);

_mqx_int _io_telnet_process_char (char _PTR_, MQX_FILE_PTR);
boolean _io_telnet_echo(IO_TELNET_PTR io_ptr, char c);

/*
** Telnet Response strings
*/

static _mqx_int TELNET_go_ahead(MQX_FILE_PTR fd) {
    char buf[] = {TELCMD_IAC, TELCMD_GA};
    return _io_write(fd, buf, 2);
}

static _mqx_int TELNET_will(MQX_FILE_PTR fd, char c) {
    char buf[3] = {TELCMD_IAC, TELCMD_WILL};
    buf[2] = c;
    return _io_write(fd, buf, 3);
}

static _mqx_int TELNET_wont(MQX_FILE_PTR fd, char c) {
    char buf[3] = {TELCMD_IAC, TELCMD_WONT};
    buf[2] = c;
    return _io_write(fd, buf, 3);

}

static _mqx_int TELNET_do(MQX_FILE_PTR fd, char c) {
    char buf[3] = {TELCMD_IAC, TELCMD_DO};
    buf[2] = c;
    return _io_write(fd, buf, 3);
}

static _mqx_int TELNET_dont(MQX_FILE_PTR fd, char c) {
    char buf[3] = {TELCMD_IAC, TELCMD_DONT};
    buf[2] = c;
    return _io_write(fd, buf, 3);
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_install
* Returned Value : int_32 a task error code or MQX_OK
* Comments       : Install the telnet device
*
*END*-----------------------------------------------------------------*/

int_32 _io_telnet_install
   (
      char_ptr identifier
   )
{ /* Body */

   return _io_dev_install(identifier,
                          _io_telnet_open,
                          _io_telnet_close,
                          _io_telnet_read,
                          _io_telnet_write,
#if MQX_VERSION < 250
                          (int_32(_CODE_PTR_)(MQX_FILE_PTR, uint_32, uint_32_ptr))
#endif
                          _io_telnet_ioctl,
                          NULL );
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_open
* Returned Value : int_32 a task error code or MQX_OK
* Comments       : Open the telnet device
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_open
   (
      MQX_FILE_PTR fd_ptr,
      char _PTR_ open_name_ptr,
      char _PTR_ flags_ptr
   )
{ /* Body */
   IO_TELNET_PTR io_ptr;

   io_ptr = RTCS_mem_alloc_zero(sizeof(*io_ptr));
   if (io_ptr == NULL) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */

   _mem_set_type(io_ptr, MEM_TYPE_IO_TELNET);

   fd_ptr->DEV_DATA_PTR = io_ptr;

   io_ptr->NETWORK_FD = (MQX_FILE_PTR)flags_ptr;
   io_ptr->CONSOLE_FD = NULL;
#ifdef __MQX__
   fd_ptr->FLAGS = IO_SERIAL_ECHO;  /* To work around a bug in fgetline() */
#endif

   io_ptr->TERMINAL_OPTIONS = TEROPT_CRLF_MODE;

   /*
   ** Send WILL SGA and DO SGA options
   */
   TELNET_will (io_ptr->NETWORK_FD, TELOPT_SGA);
   TELNET_do   (io_ptr->NETWORK_FD, TELOPT_SGA);

   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  :  _io_telnet_close
* Returned Value :  int_32 Error Code
* Comments       :  Close the telnet device
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_close
   (
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */
   IO_TELNET_PTR  io_ptr;
   _mqx_uint      flags;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   if (io_ptr != NULL) {

      /* Reenable local echo */
      if (io_ptr->CONSOLE_FD) {
         ioctl(io_ptr->CONSOLE_FD, IO_IOCTL_SERIAL_GET_FLAGS, &flags);
         flags |= IO_SERIAL_ECHO;
         ioctl(io_ptr->CONSOLE_FD, IO_IOCTL_SERIAL_SET_FLAGS, &flags);
      } /* Endif */

      _mem_free(io_ptr);
      fd_ptr->DEV_DATA_PTR = NULL;
   } /* Endif */

   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  :  _io_telnet_read
* Returned Value :  Number of characters read
* Comments       :  Read data from the Telnet device
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_read
   (
      MQX_FILE_PTR fd_ptr,
      char _PTR_  data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_TELNET_PTR  io_ptr;
   _mqx_int       count, read_count, write_count, ret;
   char_ptr       read_ptr, write_ptr;
   char           c;

   io_ptr = fd_ptr->DEV_DATA_PTR;

   write_ptr = data_ptr;
   write_count = 0;
   for (;;) {

      read_ptr = write_ptr;
      count = _io_read(io_ptr->NETWORK_FD, read_ptr, num-write_count);
      if (count == IO_EOF) {
         return IO_EOF;
      } /* Endif */

      for (read_count = 0; read_count < count; read_count++) {
         c = *read_ptr++;
         if ((ret = _io_telnet_process_char(&c, fd_ptr)) == IO_ERROR) {
             return ret;
         } else if (ret) {
            *write_ptr++ = c;
            write_count++;
         } /* Endif */
      } /* Endfor */

      if (write_count >= num) {
         break;
      } /* Endif */
      if (write_count && !_io_fstatus(io_ptr->NETWORK_FD)) {
         break;
      } /* Endif */

   } /* Endfor */

   return write_count;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_write
* Returned Value : Number of characters written
* Comments       : Writes the data to the Telnet device
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_write
   (
      MQX_FILE_PTR fd_ptr,
      char _PTR_  data_ptr,
      _mqx_int    num
   )
 { /* Body */
    IO_TELNET_PTR io_ptr;
    _mqx_int count;
    _mqx_int towrite = 0;
    char_ptr scan_ptr;

    io_ptr = fd_ptr->DEV_DATA_PTR;
    for (count = 0, scan_ptr = data_ptr; count < num; count++, scan_ptr++) {
        //transmitting data in real binary mode and avoiding workarounds
        //in handling EOLs (different CR LF combinations) valid only for character based terminals
        if (RAW_MODE(io_ptr)) {
            if (io_ptr->REMOTE_OPTIONS[TELOPT_BINARY]) {
                if (*scan_ptr == TELCMD_IAC) {
                    if (fputc(TELCMD_IAC, io_ptr->NETWORK_FD) == IO_EOF) {
                        return count;
                    }
                    if (fputc(TELCMD_IAC, io_ptr->NETWORK_FD) == IO_EOF) {
                        return count;
                    }
                } else {
                    if (fputc(*scan_ptr, io_ptr->NETWORK_FD) == IO_EOF) {
                        return count;
                    }
                }
            } else { //sending binary data to ASCII remote end should never happen
                return IO_ERROR;
            }
            continue;
        }

        //handle transmissions for character based terminals including CR LF workarounds
        switch (*scan_ptr) {
            case TELCMD_IAC:
            if (io_ptr->REMOTE_OPTIONS[TELOPT_BINARY]) {
                if (fputc(TELCMD_IAC, io_ptr->NETWORK_FD) == IO_EOF) {
                    return count;
                }
                if (fputc(TELCMD_IAC, io_ptr->NETWORK_FD) == IO_EOF) {
                    return count;
                }
            } // else character is not valid USASCII so filter it
            break;

            case TELCC_LF:
                if (fputc(TELCC_CR, io_ptr->NETWORK_FD) == IO_EOF) {
                    return count;
                }
                if (io_ptr->REMOTE_OPTIONS[TELOPT_BINARY]) {
                    if (fputc(TELCC_LF, io_ptr->NETWORK_FD) == IO_EOF) {
                        return count;
                    }
                } else if (CRLF_MODE(io_ptr)) {//in ascii mode CR should be followed by either NULL or LF
                    if (fputc(TELCC_LF, io_ptr->NETWORK_FD) == IO_EOF) {
                        return count;
                    }
                } else {
                    if (fputc(TELCC_NULL, io_ptr->NETWORK_FD) == IO_EOF) {
                        return count;
                    }
                }
                break;

            case TELCC_CR:
                //in character based terminal in CRLF mode is possible to transmit stand-alone CRs
                if (CRLF_MODE(io_ptr) && !RAW_MODE(io_ptr)) {
                    if ((count < num) && (*(scan_ptr + 1) != TELCC_LF)) {
                        if (fputc(TELCC_CR, io_ptr->NETWORK_FD) == IO_EOF) {
                            return count;
                        }
                        if (fputc(TELCC_NULL, io_ptr->NETWORK_FD) == IO_EOF) {
                            return count;
                        }
                    }
                }
                break;

            default:
                if (io_ptr->REMOTE_OPTIONS[TELOPT_BINARY]) {
                    if (fputc(*scan_ptr, io_ptr->NETWORK_FD) == IO_EOF) {
                        return count;
                    }
                } else if (!(*scan_ptr & 0x80)) {
                    if (fputc(*scan_ptr, io_ptr->NETWORK_FD) == IO_EOF) {
                        return count;
                    }
                } //else filter it
                break;
        }
    }

    return count;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_ioctl
* Returned Value : int_32
* Comments       : Returns result of ioctl operation.
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_telnet_ioctl
   (
      MQX_FILE_PTR fd_ptr,
      _mqx_uint   cmd,
      pointer     param_ptr
   )
{ /* Body */
   IO_TELNET_PTR  io_ptr;
   int_32         result;
   _mqx_uint      flags;
   int_32         rawc;
   char           c;

   result = IO_ERROR;
   io_ptr = (IO_TELNET_PTR) fd_ptr->DEV_DATA_PTR;

   if (io_ptr == NULL) {
      return (_mqx_int)result;
   } /* Endif */

   switch (cmd) {

   case IO_IOCTL_CHAR_AVAIL:

      while (1) {
         *(boolean _PTR_)param_ptr = fstatus(io_ptr->NETWORK_FD);
         if (!*(boolean _PTR_)param_ptr) {
            break;
         } /* Endif */
         rawc = (int_32)fgetc(io_ptr->NETWORK_FD);
         if (rawc == IO_EOF) {
            break;
         } /* Endif */
         c = (char)rawc;
         result = _io_telnet_process_char(&c, fd_ptr);
         if (result == IO_ERROR) {
             return result;
         } else if (result) {
            fungetc(c, fd_ptr);
            break;
         } /* Endif */
      } /* Endwhile */
      result = MQX_OK;
      break;

   case IO_IOCTL_SET_STREAM:
      io_ptr->CONSOLE_FD = (MQX_FILE_PTR)param_ptr;

      /* Clear or set echoing by server */
      if (io_ptr->REMOTE_OPTIONS[TELOPT_ECHO])
      {
        ioctl(io_ptr->CONSOLE_FD, IO_IOCTL_SERIAL_GET_FLAGS, &flags);
        flags &= ~IO_SERIAL_ECHO;
        ioctl(io_ptr->CONSOLE_FD, IO_IOCTL_SERIAL_SET_FLAGS, &flags);
      } /* Endif */

      result = MQX_OK;
      break;

   case IO_IOCTL_SERIAL_GET_FLAGS:
      *(_mqx_uint_ptr)param_ptr = io_ptr->FLAGS;
      result = MQX_OK;
      break;

   case IO_IOCTL_SERIAL_SET_FLAGS:
      io_ptr->FLAGS = *(_mqx_uint_ptr)param_ptr;
      /* Handle defaults */

      /* Echo */
      if (!io_ptr->LOCAL_OPTIONS[TELOPT_ECHO] && io_ptr->REMOTE_OPTIONS[TELOPT_ECHO])
      {
        TELNET_dont(io_ptr->NETWORK_FD, TELOPT_ECHO);
      } /* Endif */

      if (io_ptr->FLAGS & IO_SERIAL_ECHO)
      {
        TELNET_will(io_ptr->NETWORK_FD, TELOPT_ECHO);
      }
      else
      {
        TELNET_dont(io_ptr->NETWORK_FD, TELOPT_ECHO);
      }

      /* Binary mode */
      if (!io_ptr->LOCAL_OPTIONS[TELOPT_BINARY]) { //inbinary
          TELNET_will(io_ptr->NETWORK_FD, TELOPT_BINARY);
      }
      if (!io_ptr->REMOTE_OPTIONS[TELOPT_BINARY]) { //outbinary
          TELNET_do(io_ptr->NETWORK_FD, TELOPT_BINARY);
      }
      result = MQX_OK;
      break;

   case IO_IOCTL_SET_ECHO:
       break;

   case IO_IOCTL_GET_ECHO:
       break;

   case IO_IOCTL_SET_INBINARY:
       if (!io_ptr->LOCAL_OPTIONS[TELOPT_BINARY]) { //inbinary
           TELNET_will(io_ptr->NETWORK_FD, TELOPT_BINARY);
           *(boolean _PTR_)param_ptr = TRUE;
       } else {
           *(boolean _PTR_)param_ptr = FALSE;
       }
       result = MQX_OK;
       break;

   case IO_IOCTL_GET_INBINARY:
       *(boolean _PTR_)param_ptr = io_ptr->LOCAL_OPTIONS[TELOPT_BINARY];
       result = MQX_OK;
       break;

   case IO_IOCTL_SET_OUTBINARY:
       if (!io_ptr->REMOTE_OPTIONS[TELOPT_BINARY]) { //outbinary
           TELNET_do(io_ptr->NETWORK_FD, TELOPT_BINARY);
           *(boolean _PTR_)param_ptr = TRUE;
       } else {
           *(boolean _PTR_)param_ptr = FALSE;
       }
       result = MQX_OK;
       break;

   case IO_IOCTL_GET_OUTBINARY:
       *(boolean _PTR_)param_ptr = io_ptr->REMOTE_OPTIONS[TELOPT_BINARY];
       result = MQX_OK;
       break;

   case IO_IOCTL_SET_BINARY:
       *(boolean _PTR_)param_ptr = FALSE;
       if (!io_ptr->LOCAL_OPTIONS[TELOPT_BINARY]) { //inbinary
           TELNET_will(io_ptr->NETWORK_FD, TELOPT_BINARY);
           *(boolean _PTR_)param_ptr = TRUE;
       }
       if (!io_ptr->REMOTE_OPTIONS[TELOPT_BINARY]) { //outbinary
           TELNET_do(io_ptr->NETWORK_FD, TELOPT_BINARY);
           *(boolean _PTR_)param_ptr = TRUE;
       }
       result = MQX_OK;
       break;

   case IO_IOCTL_GET_BINARY:
       *(boolean _PTR_)param_ptr = io_ptr->LOCAL_OPTIONS[TELOPT_BINARY] && io_ptr->REMOTE_OPTIONS[TELOPT_BINARY];
       result = MQX_OK;
       break;

   case IO_IOCTL_SET_CRLF:
       if (*(boolean _PTR_)param_ptr) {
           io_ptr->TERMINAL_OPTIONS |= TEROPT_CRLF_MODE;
       } else {
           io_ptr->TERMINAL_OPTIONS &= ~TEROPT_CRLF_MODE;
       }
       result = MQX_OK;
       break;

   case IO_IOCTL_GET_CRLF:
       *(boolean _PTR_)param_ptr = (io_ptr->TERMINAL_OPTIONS & TEROPT_CRLF_MODE) > 0;
       result = MQX_OK;
       break;

   case IO_IOCTL_SET_RAW:
       if (*(boolean _PTR_)param_ptr) {
           io_ptr->TERMINAL_OPTIONS |= TEROPT_RAW_MODE;
       } else {
           io_ptr->TERMINAL_OPTIONS &= ~TEROPT_RAW_MODE;
       }
       result = MQX_OK;
       break;

   case IO_IOCTL_GET_RAW:
       *(boolean _PTR_)param_ptr = (io_ptr->TERMINAL_OPTIONS & TEROPT_RAW_MODE) > 0;
       result = MQX_OK;
       break;

   case IO_IOCTL_FLUSH_OUTPUT:
      result =ioctl(io_ptr->NETWORK_FD, IO_IOCTL_FLUSH_OUTPUT, param_ptr);
      break;

   } /* Endswitch */

   return (_mqx_int)result;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_echo
* Returned Value : TRUE when character was successfully echoed, FALSE otherwise
* Comments       : Echoes given character if server echo is set
*
*
*END*-----------------------------------------------------------------*/
boolean _io_telnet_echo(IO_TELNET_PTR io_ptr, char c) {
    //echoing received character
    if (io_ptr->LOCAL_OPTIONS[TELOPT_ECHO]) {
        if (c == '\b') {
            fputs("\b ", io_ptr->NETWORK_FD);
        } /* Endif */
        if (fputc(c, io_ptr->NETWORK_FD) != c) {
            return FALSE;
        }
        fflush(io_ptr->NETWORK_FD);
        return TRUE;
    } /* Endif */
    return FALSE;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_telnet_process_char
* Returned Value : IO_ERROR, TRUE or FALSE
* Comments       : process the arrived characters, including negotiated options
*       TRUE is returned when valid ASCII character has been processed.
*       FALSE is returned for either IAC, commands or CR which should be combined
*       with next character.
*       IO_ERROR is returned when received bytes doesn't follow RFC 854, i.e. CR
*       followed neither by LF nor by NULL.
*
*END*-----------------------------------------------------------------*/
_mqx_int _io_telnet_process_char
   (
      char _PTR_  cp,
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */
   char           c = *cp;
   _mqx_uint      flags;
   IO_TELNET_PTR  io_ptr;
   MQX_FILE_PTR   console_stream;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   console_stream = io_ptr->CONSOLE_FD;

   switch (io_ptr->STATE) {

   case 0: /* Start state */
        //receiving data in real binary mode so avoiding workarounds
        //in handling EOLs (different CR LF combinations) valid only for character based terminals
        if (RAW_MODE(io_ptr) && io_ptr->LOCAL_OPTIONS[TELOPT_BINARY]) {
            switch (c) {
            case TELCMD_IAC:
                io_ptr->STATE = 1;
                return FALSE;
            default:
                if (io_ptr->REMOTE_OPTIONS[TELOPT_BINARY] || !(c & 0x80)) {
                    _io_telnet_echo(io_ptr, c);
                    return TRUE;
                } else {
                    return FALSE; //filter character
                }
            }
        }

        if (io_ptr->LOCAL_OPTIONS[TELOPT_BINARY]) { //receiving in binary mode
            switch (c) {
            case TELCMD_IAC:
                io_ptr->STATE = 1;
                return FALSE;
            case TELCC_CR:
                _io_telnet_echo(io_ptr, c);
                _io_telnet_echo(io_ptr, TELCC_LF);
                *cp = TELCC_LF; //application character mapping to LF
                return TRUE;
            default:
                if (io_ptr->REMOTE_OPTIONS[TELOPT_BINARY]) {
                    //strip extra LF because in binary mode each CR is echoed as CR LF
                    if (!CRLF_MODE(io_ptr) || (CRLF_MODE(io_ptr) && (c != TELCC_LF))) {
                        _io_telnet_echo(io_ptr, c);
                    } else {
                        return FALSE;
                    }
                } else if (c & 80) { //remote end is in ASCII mode but we have illegal data to send
                    return FALSE;
                } else {
                    _io_telnet_echo(io_ptr, c);
                }
                return TRUE;
            }
        } else { //ASCII mode
            switch (c) {
            case TELCMD_IAC:
                io_ptr->STATE = 1;
                return FALSE;
            case TELCC_CR:
                _io_telnet_echo(io_ptr, c);
                io_ptr->STATE = 6;
                return FALSE;
            default:
                if (c & 0x80) { //in ASCII only USASCII codes could be send
                    return FALSE;
                } else {
                    _io_telnet_echo(io_ptr, c);
                    return TRUE;
                }
            }
        }

      /* Start SPR P122-0266-33 remove all warnings from RTCS code. */
      /* break; */
      /* End SPR P122-0266-33. */
   case 1: /* IAC */
      switch (c) {

      case TELCMD_AYT:
         /*
         ** Request to send a string to prove we're alive.
         ** Send back comforting message.
         */
         fputs("\nRTCS Telnet server online.\n", fd_ptr);
         io_ptr->STATE = 0;
         return FALSE;

      case TELCMD_EC:
         /*
         ** Request to erase the last character before this command.
         */
         *cp = '\b';
         io_ptr->STATE = 0;
         return TRUE;

      case TELCMD_WILL:
         io_ptr->STATE = 2;
         return FALSE;

      case TELCMD_DO:
         io_ptr->STATE = 3;
         return FALSE;

      case TELCMD_WONT:
         io_ptr->STATE = 4;
         return FALSE;

      case TELCMD_DONT:
         io_ptr->STATE = 5;
         return FALSE;

      case TELCMD_IAC:
        /* Second IAC is not interpreted as command, it's data */
        if (io_ptr->LOCAL_OPTIONS[TELOPT_BINARY] && io_ptr->REMOTE_OPTIONS[TELOPT_BINARY]) { //BINARY mode
            _io_telnet_echo(io_ptr, TELCMD_IAC);
            _io_telnet_echo(io_ptr, TELCMD_IAC);
            io_ptr->STATE = 0;
            return TRUE;
        } //else filter it (continue with default state)

      default:
         /* Ignore all other telnet commands (e.g GA) */
         io_ptr->STATE = 0;
         return FALSE;
      } /* Endswitch */

   case 2: /* IAC WILL */
      switch (c) {

      case TELOPT_ECHO:
        if (!io_ptr->REMOTE_OPTIONS[TELOPT_ECHO])
        {
          if ((io_ptr->LOCAL_OPTIONS[TELOPT_ECHO]) || !(io_ptr->FLAGS & IO_SERIAL_ECHO))
          {
            TELNET_dont(io_ptr->NETWORK_FD, TELOPT_ECHO);
          }
          else
          {
            /* Clear the echo bit */
            ioctl(console_stream, IO_IOCTL_SERIAL_GET_FLAGS, &flags);
            flags &= ~IO_SERIAL_ECHO;
            ioctl(console_stream, IO_IOCTL_SERIAL_SET_FLAGS, &flags);
            io_ptr->REMOTE_OPTIONS[TELOPT_ECHO] = TRUE;
            TELNET_do(io_ptr->NETWORK_FD, TELOPT_ECHO);
          } /* Endif */
        } /* Endif */
        else if (!(io_ptr->FLAGS & IO_SERIAL_ECHO))
        {
          TELNET_dont(io_ptr->NETWORK_FD, TELOPT_ECHO);
        }
        break;

      case TELOPT_SGA:
      case TELOPT_BINARY:
          /* ACK only */
          if (!io_ptr->REMOTE_OPTIONS[c]) {
             io_ptr->REMOTE_OPTIONS[c] = TRUE;
             TELNET_do(io_ptr->NETWORK_FD, c);
          } /* Endif */
          break;

      default:
         TELNET_dont(io_ptr->NETWORK_FD, c);
         break;
      }  /* Endswitch */

      io_ptr->STATE = 0;
      return FALSE;

   case 3: /* IAC DO */
      switch (c) {

      case TELOPT_ECHO:
        /* ACK only */
        if (!io_ptr->LOCAL_OPTIONS[TELOPT_ECHO])
        {
          if (io_ptr->REMOTE_OPTIONS[TELOPT_ECHO])
          {
            if (io_ptr->FLAGS & IO_SERIAL_ECHO)
            {
              TELNET_wont(io_ptr->NETWORK_FD, TELOPT_ECHO);
            }
            else
            {
              TELNET_dont(io_ptr->NETWORK_FD, TELOPT_ECHO);
            }
          }
          else
          {
            if (io_ptr->FLAGS & IO_SERIAL_ECHO)
            {
              io_ptr->LOCAL_OPTIONS[TELOPT_ECHO] = TRUE;
              TELNET_will(io_ptr->NETWORK_FD, TELOPT_ECHO);
            }
            else
            {
              TELNET_dont(io_ptr->NETWORK_FD, TELOPT_ECHO);
            }
          } /* Endif */
        } /* Endif */
        break;

      case TELOPT_SGA:
      case TELOPT_BINARY:
          /* ACK only */
          if (!io_ptr->LOCAL_OPTIONS[c]) {
             io_ptr->LOCAL_OPTIONS[c] = TRUE;
             TELNET_will(io_ptr->NETWORK_FD, c);
          } /* Endif */
          break;

      default:
         TELNET_wont(io_ptr->NETWORK_FD, c);
         break;
      } /* Endswitch */

      io_ptr->STATE = 0;
      return FALSE;

   case 4: /* IAC WONT */
      switch (c) {

      case TELOPT_ECHO:
         if (io_ptr->REMOTE_OPTIONS[TELOPT_ECHO])
            {
            if (io_ptr->FLAGS & IO_SERIAL_ECHO)
               {
               /* Set the echo bit */
               ioctl(console_stream, IO_IOCTL_SERIAL_GET_FLAGS, &flags);
               flags |= IO_SERIAL_ECHO;
               ioctl(console_stream, IO_IOCTL_SERIAL_SET_FLAGS, &flags);
               }

            io_ptr->REMOTE_OPTIONS[TELOPT_ECHO] = FALSE;
            TELNET_dont(io_ptr->NETWORK_FD, TELOPT_ECHO);
            } /* Endif */
         break;

      case TELOPT_BINARY:
      default:
         if (c < TELNET_MAX_OPTS) {
            /* ACK only */
            if (io_ptr->REMOTE_OPTIONS[c]) {
               TELNET_dont(io_ptr->NETWORK_FD, c);
               io_ptr->REMOTE_OPTIONS[c] = FALSE;
            } /* Endif */
         } /* Endif */
         break;
      } /* Endswitch */

      io_ptr->STATE = 0;
      return FALSE;

   case 5: /* IAC DONT */

      if (c < TELNET_MAX_OPTS) {
         /* ACK only */
         if (io_ptr->LOCAL_OPTIONS[c]) {
            io_ptr->LOCAL_OPTIONS[c] = FALSE;
            TELNET_wont(io_ptr->NETWORK_FD, c);
         } /* Endif */
      } /* Endif */

      io_ptr->STATE = 0;
      return FALSE;

   case 6: /* CR received in ASCII mode */
       switch (c) {
       case TELCC_NULL:
           if (CRLF_MODE(io_ptr)) {
               *cp = TELCC_CR; //application character mapping to CR
           } else {
               *cp = TELCC_LF; //application character mapping to LF
           }
           _io_telnet_echo(io_ptr, *cp);
           break;
       case TELCC_LF:
           _io_telnet_echo(io_ptr, c);
           *cp = TELCC_LF; //application character mapping to LF
           break;
       default:
           return IO_ERROR;
       }
       io_ptr->STATE = 0;
       return TRUE;
   } /* Endswitch */

   /* Start SPR P122-0266-33 remove all warnings from RTCS code. */
   /* Will never reach here, this is just for compiler warning. */
   return FALSE;
   /* End SPR P122-0266-33. */
} /* Endbody */


/* EOF */
