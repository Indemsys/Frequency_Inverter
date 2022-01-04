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
* $FileName: ftpclnt.c$
* $Version : 3.8.15.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains an implementation of an
*   FTP client.
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <rtcs.h>
#include <fio.h>
#include <ftpc.h>

int_32 FTP_receive_message      (pointer, MQX_FILE_PTR);
int_32 FTP_receive_message_pasv (uint_32, MQX_FILE_PTR, _ip_address _PTR_, uint_16 _PTR_);
void   FTP_execute_command      (uint_32, boolean, MQX_FILE_PTR);

uint_32 FTPc_window_size = FTPCCFG_WINDOW_SIZE;
uint_32 FTPc_buffer_size = FTPCCFG_BUFFER_SIZE;

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTP_open
* Returned Value   : 3-digit message code, or -1 on error
* Comments  :  Routine to open an FTP session.
*
*END*-----------------------------------------------------------------*/

int_32 FTP_open
   (
      pointer _PTR_  handle_ptr,
      _ip_address    server_ipaddr,
      MQX_FILE_PTR   ctrl_fd
   )
{ /* Body */
   sockaddr_in    server_sockaddr;
   uint_32        ctrl_sock;
   int_32         message;
   uint_32        option;
   int_32         error=RTCS_OK;

   /* Get a control socket */
   ctrl_sock = socket(PF_INET, SOCK_STREAM, 0);
   if (ctrl_sock == RTCS_SOCKET_ERROR) {
      return RTCS_ERROR;
   } /* Endif */


#if FTPCCFG_SMALL_FILE_PERFORMANCE_ENHANCEMENT
   option = TRUE;   
   error = setsockopt(ctrl_sock, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));
#endif

   if (!error) {
      option = FTPc_window_size;   
      error = setsockopt(ctrl_sock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
   }
   
   if (!error) {
      option = FTPc_window_size;   
      error = setsockopt(ctrl_sock, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));
   }      

   if (!error) {
      option = FTPCCFG_TIMEWAIT_TIMEOUT; 
      error = setsockopt(ctrl_sock, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
   } 

   if (error) {
      shutdown(ctrl_sock, FLAG_ABORT_CONNECTION);
      return RTCS_ERROR;
   } /* Endif */

   /* Connect the control socket to port 21 on the server  */
   server_sockaddr.sin_family      = AF_INET;
   server_sockaddr.sin_port        = IPPORT_FTP;  /* port 21 */
   server_sockaddr.sin_addr.s_addr = server_ipaddr;
   message = connect(ctrl_sock,(const sockaddr *)(&server_sockaddr), sizeof(server_sockaddr));
   if (message != RTCS_OK) {
      shutdown(ctrl_sock, FLAG_ABORT_CONNECTION);
      return RTCS_ERROR;
   } /* Endif */

   message = FTP_receive_message((pointer)ctrl_sock, ctrl_fd);

   if (message == RTCS_ERROR) {
      shutdown (ctrl_sock, FLAG_ABORT_CONNECTION);
   }
   
   if (handle_ptr) {
      *handle_ptr = (pointer)ctrl_sock;
   } /* Endif */

   return message;

} /* Endbody */

#if 0
/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTP_receive_message
* Returned Value   : 3-digit message code, or -1 on error
* Comments  :  Receive message on the control connection and write it
*              to fd_ctrl
*
*END*-----------------------------------------------------------------*/

int_32 FTP_receive_message
   (
      uint_32     ctrl_sock,
      MQX_FILE_PTR ctrl_fd
   )
{ /* Body */
   char        *message_buffer;
   uint_32     i, chars_left = 0;
   boolean     message_found = FALSE;
   int_32      message = 0, digit_count = 0, count = 0;

   message_buffer = RTCS_mem_alloc(BUF_SIZE);
   if (!message_buffer) {
      return RTCS_ERROR;
   }
   _mem_set_type(ftpc_context_ptr, MEM_TYPE_FTPc_RX_BUFFER);

   while (1) {
      if (!chars_left) {
         write(ctrl_fd, message_buffer, count);
         count = recv(ctrl_sock, message_buffer, BUF_SIZE, 0);
         if (count == RTCS_ERROR) {
            _mem_free(message_buffer);
            return RTCS_ERROR;
         } /* Endif */
         chars_left = count;
         i = 0;
      } /* Endif */

      if ((digit_count == 0) && (isdigit(message_buffer[i]))) {
         digit_count++;
         message *= 10;
         message += (message_buffer[i] - '0');
      } else if ((digit_count == 1) && (isdigit(message_buffer[i]))) {
         digit_count++;
         message *= 10;
         message += (message_buffer[i] - '0');
      } else if ((digit_count == 2) && (isdigit(message_buffer[i]))) {
         digit_count++;
         message *= 10;
         message += (message_buffer[i] - '0');
      } else if ((digit_count == 3) && (message_buffer[i] == ' ')) {
         message_found = TRUE;

      } else if (message_found) {
         /* EOL and found message code, so break. */
         if (message_buffer[i] == '\n') {
            break;
         } /* Endif */

      /* EOL: maybe message is on next line, so reset digit_count & message */
      } else if (message_buffer[i] == '\n') {
         digit_count = 0;
         message = 0;

      /* Not EOL.  Don't look for message code for the rest of this line. */
      } else {
         digit_count = -1;
      } /* Endif */

      i++;
      chars_left--;
   } /* Endwhile */

   write(ctrl_fd, message_buffer, count);
   _mem_free(message_buffer);
   return message;

} /* Endbody */
#endif


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTP_receive_message
* Returned Value   : 3-digit message code, or -1 on error
* Comments  :  Receive message on the control connection and write it
*              to fd_ctrl
*
*END*-----------------------------------------------------------------*/

int_32 FTP_receive_message
   (
      pointer     handle,
      MQX_FILE_PTR ctrl_fd
   )
{ /* Body */
   uint_32  ctrl_sock = (uint_32)handle;
  
   char        c;
   char        *message_buffer;
   uint_32     i=0;
   boolean     message_found = FALSE;
   int_32      message = 0, digit_count = 0;

   message_buffer = RTCS_mem_alloc(FTPc_window_size);
   if (!message_buffer) {
      return RTCS_ERROR;
   }
   _mem_set_type(message_buffer, MEM_TYPE_FTPc_RX_BUFFER);

   while (1) {
      if (recv(ctrl_sock, &c, 1, 0) == RTCS_ERROR) {
         _mem_free(message_buffer);
         return RTCS_ERROR;
      } /* Endif */
      message_buffer[i++]=c;

      if ((digit_count == 0) && (isdigit(c))) {
         digit_count++;
         message = (c - '0')*100;
      } else if ((digit_count == 1) && (isdigit(c))) {
         digit_count++;
         message += (c - '0')*10;
      } else if ((digit_count == 2) && (isdigit(c))) {
         digit_count++;
         message += (c - '0');
      } else if ((digit_count == 3) && (c == ' ')) {
         message_found = TRUE;

      } else if (c == '\n') {
         if (message_found) {
            /* EOL and found message code, so break. */
            break;
         } /* Endif */

         /* EOL: maybe message is on next line, so reset digit_count & message */
         digit_count = 0;
         message = 0;

      /* Not EOL.  Don't look for message code for the rest of this line. */
      } else {
         digit_count = -1;
      } /* Endif */
      
      if (i==FTPc_window_size)  {
         write(ctrl_fd, message_buffer, i);
         i=0;
      }
   } /* Endwhile */

   if (i)  {
      write(ctrl_fd, message_buffer, i);
   }
   
   _mem_free(message_buffer);
   return message;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTP_receive_message_pasv
* Returned Value   : 3-digit message code, or -1 on error
* Comments  :  Receive the response to a PASV command on the control
*              connection and write it to fd_ctrl
*
*END*-----------------------------------------------------------------*/

int_32 FTP_receive_message_pasv
   (
      uint_32           ctrl_sock,
      MQX_FILE_PTR      ctrl_fd,
      _ip_address _PTR_ data_ipaddr_ptr,
      uint_16 _PTR_     data_port_ptr
   )
{ /* Body */
   char        c, *message_buffer;
   uint_32     i=0, comma_count = 0;
   boolean     message_found = FALSE;
   int_32      message = 0, digit_count = 0, p=0;
   _ip_address ipaddr = 0;
   uint_16     port = 0;

   message_buffer = RTCS_mem_alloc(FTPc_window_size);
   if (!message_buffer) {
      return RTCS_ERROR;
   }
   _mem_set_type(message_buffer, MEM_TYPE_FTPc_RX_BUFFER);

   while (1) {
      if (recv(ctrl_sock, &c, 1, 0) == RTCS_ERROR)  {
         _mem_free(message_buffer);
         return RTCS_ERROR;
      } /* Endif */
      message_buffer[i++]=c;

      if ((digit_count == 0) && (isdigit(c))) {
         digit_count++;
         message = (c - '0')*100;
      } else if ((digit_count == 1) && (isdigit(c))) {
         digit_count++;
         message += (c - '0')*10;
      } else if ((digit_count == 2) && (isdigit(c))) {
         digit_count++;
         message += (c - '0');
      } else if ((digit_count == 3) && (c == ' ')) {
         message_found = TRUE;
         p = 0;

      } else if (message_found) {

         if (isdigit(c)) {
            p *= 10;
            p += (c - '0');
         } else if (c == ',') {
            comma_count++;
            if (comma_count <= 4) {
               ipaddr <<= 8;
               ipaddr |= p & 0xFF;
            } else if (comma_count <= 6) {
               port <<= 8;
               port |= p & 0xFF;
            } /* Endif */
            p = 0;
         } else if (c == '\n') {
            break;
         } /* Endif */

      /* EOL: maybe message is on next line, so reset digit_count & message */
      } else if (c == '\n') {
         digit_count = 0;
         comma_count = 0;
         message = 0;
         ipaddr = 0;
         port = 0;

      /* Not EOL.  Don't look for message code for the rest of this line. */
      } else {
         digit_count = -1;
      } /* Endif */
      
      if (i==FTPc_window_size)  {
         write(ctrl_fd, message_buffer, i);
         i=0;
      }
   } /* Endwhile */

   if (comma_count == 5) {
      port <<= 8;
      port |= p & 0xFF;
   } /* Endif */

   if (data_ipaddr_ptr) {
      *data_ipaddr_ptr = ipaddr;
   } /* Endif */
   if (data_port_ptr) {
      *data_port_ptr = port;
   } /* Endif */

   if (i)  {
      write(ctrl_fd, message_buffer, i);
   }
   
   _mem_free(message_buffer);
   return message;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTP_command
* Returned Value   : 3-digit message code, or -1 on error
* Comments  :  Issue an FTP command that does not require a data
*              connection
*
*END*-----------------------------------------------------------------*/

int_32 FTP_command
   (
      pointer     handle,
      char_ptr    command,
      MQX_FILE_PTR ctrl_fd
   )
{ /* Body */
   uint_32  ctrl_sock = (uint_32)handle;

   send(ctrl_sock, command, strlen(command), 0);
   return FTP_receive_message(handle, ctrl_fd);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTP_close
* Returned Value   : 3-digit message code, or -1 on error
* Comments  :  Close an FTP session
*
*END*-----------------------------------------------------------------*/

int_32 FTP_close
   (
      pointer     handle,
      MQX_FILE_PTR ctrl_fd
   )

{ /* Body */
   uint_32  ctrl_sock = (uint_32)handle;
   int_32   message;

   message = FTP_command(handle, "QUIT\r\n", ctrl_fd);
   shutdown(ctrl_sock, FLAG_CLOSE_TX);
   return message;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTP_command_data
* Returned Value   : int_32 3-digit message code or RTCS_ERROR
* Comments  :  Routine for FTP commands that require a data connection
*
*END*-----------------------------------------------------------------*/

int_32 FTP_command_data
   (
      pointer     handle,
      char_ptr    command,
      MQX_FILE_PTR ctrl_fd,
      MQX_FILE_PTR data_fd,
      uint_32     flags
   )
{ /* Body */
   uint_32     ctrl_sock = (uint_32)handle;
   uint_32     listen_sock, data_sock;
   sockaddr_in listen_addr, data_addr;
   uint_16     listen_addr_len;
   int_32      message;
   int_16      mode, direction;
   char        command_str[31];
   _ip_address data_ipaddr;
   uint_16     data_port;
   uint_32     option;
   int_32      error;


   mode = flags & 3;
   direction = flags & 4;

   switch (mode) {

   /*
   ** default mode: client control = P,  client data = P
   **               server control = 21, server data = 20
   **
   ** PORT mode:    client control = P,  client data = P'
   **               server control = 21, server data = 20
   */
   case 1:
   case 2:

      /* Get a listen socket */
      listen_sock = socket(PF_INET, SOCK_STREAM, 0);
      if (listen_sock == RTCS_SOCKET_ERROR) {
         return RTCS_ERROR;
      } /* Endif */


#ifdef FTPCCFG_SMALL_FILE_PERFORMANCE_ENHANCEMENT
      option = TRUE;   
      error = setsockopt(listen_sock, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));
      if (error) {
         return RTCS_ERROR;
      } /* Endif */  
#endif
      option = FTPc_buffer_size;   
      error = setsockopt(listen_sock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
      if (error) {
         return RTCS_ERROR;
      } /* Endif */      

      option = FTPc_buffer_size;   
      error = setsockopt(listen_sock, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));
      if (error) {
         return RTCS_ERROR;
      } /* Endif */      

      option = FTPCCFG_TIMEWAIT_TIMEOUT; 
      error = setsockopt(listen_sock, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
      if (error) {
         return RTCS_ERROR;
      } /* Endif */      

      /* Determine which local address and port to use */
      listen_addr_len = sizeof(listen_addr);
      message = getsockname(ctrl_sock, (sockaddr *)&listen_addr, &listen_addr_len);
      if (message != RTCS_OK) {
         shutdown(listen_sock, FLAG_CLOSE_TX);
         return RTCS_ERROR;
      } /* Endif */
      if (mode == 2) {
         listen_addr.sin_port = 0;
      } /* Endif */

      /* Put socket in listen mode */
      message = bind(listen_sock,(const sockaddr *) &listen_addr, sizeof(listen_addr));
      if (message != RTCS_OK) {
         shutdown(listen_sock, FLAG_CLOSE_TX);
         return RTCS_ERROR;
      } /* Endif */
      message = listen(listen_sock, 0);
      if (message != RTCS_OK) {
         shutdown(listen_sock, FLAG_CLOSE_TX);
         return RTCS_ERROR;
      } /* Endif */

      /* For PORT mode, send a PORT command with our local port */
      if (mode == 2) {
         listen_addr_len = sizeof(listen_addr);
         message = getsockname(listen_sock, (sockaddr *)&listen_addr, &listen_addr_len);
         if (message != RTCS_OK) {
            shutdown(listen_sock, FLAG_CLOSE_TX);
            return RTCS_ERROR;
         } /* Endif */
         /* Removed use of %hd due to incompatibility with certain compiler runtimes */ 
         sprintf(command_str, "PORT %ld,%ld,%ld,%ld,%ld,%ld\r\n",
                 (listen_addr.sin_addr.s_addr >> 24) & 0xFF,
                 (listen_addr.sin_addr.s_addr >> 16) & 0xFF,
                 (listen_addr.sin_addr.s_addr >>  8) & 0xFF,
                  listen_addr.sin_addr.s_addr        & 0xFF,
                 (uint_32) ((listen_addr.sin_port  >>  8) & 0xFF),
                 (uint_32) (listen_addr.sin_port  & 0xFF));
         message = FTP_command(handle, command_str, ctrl_fd);
         if ((message < 200) || (message >= 300)) {
            shutdown(listen_sock, FLAG_CLOSE_TX);
            return message;
         } /* Endif */
      } /* Endif */

      /* Send command, expecting "1xx Opening data connection" */
      message = FTP_command(handle, command, ctrl_fd);
      if ((message < 100) || (message >= 200)) {
         shutdown(listen_sock, FLAG_CLOSE_TX);
         return message;
      } /* Endif */

      /* Connection requested; accept it */
      data_sock = accept(listen_sock, NULL, NULL);
      shutdown(listen_sock, FLAG_CLOSE_TX);
      if (data_sock == RTCS_SOCKET_ERROR) {
         return RTCS_ERROR;
      } /* Endif */

      break;

   /*
   ** PASV mode:  client control = P,  client data = P'
   **             server control = 21, server data = P''
   */
   case 3:

      /* Send PASV command */
      strcpy(command_str, "PASV\r\n");
      send(ctrl_sock, command_str, strlen(command_str), 0);

      message = FTP_receive_message_pasv(ctrl_sock, ctrl_fd, &data_ipaddr, &data_port);
      if ((message < 200) || (message >= 300)) {
         return message;
      } /* Endif */

      /* Get data socket and connect it */
      data_sock = socket(PF_INET, SOCK_STREAM, 0);
      
#ifdef FTPCCFG_SMALL_FILE_PERFORMANCE_ENHANCEMENT
      option = TRUE;   
      error = setsockopt(data_sock, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));
      if (error) {
         return RTCS_ERROR;
      } /* Endif */ 
#endif
      option = FTPc_buffer_size;   
      error = setsockopt(data_sock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
      if (error) {
         return RTCS_ERROR;
      } /* Endif */      

      option = FTPc_buffer_size;   
      error = setsockopt(data_sock, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));
      if (error) {
         return RTCS_ERROR;
      } /* Endif */      

      option = FTPCCFG_TIMEWAIT_TIMEOUT; 
      error = setsockopt(data_sock, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
      if (error) {
         return RTCS_ERROR;
      } /* Endif */      
      
      data_addr.sin_family      = AF_INET;
      data_addr.sin_port        = data_port;
      data_addr.sin_addr.s_addr = data_ipaddr;
      message = connect(data_sock,(const sockaddr *)(&data_addr), sizeof(data_addr));
      if (message != RTCS_OK) {
         shutdown(data_sock, FLAG_CLOSE_TX);
         return RTCS_ERROR;
      } /* Endif */

      /* Send command, expecting "1xx Opening data connection" */
      message = FTP_command(handle, command, ctrl_fd);
      if ((message < 100) || (message >= 200)) {
         shutdown(data_sock, FLAG_CLOSE_TX);
         return message;
      } /* Endif */

      break;

   default:
      return RTCS_ERROR;
   } /* Endswitch */

   /* Execute the command */
   FTP_execute_command(data_sock, direction, data_fd);
   shutdown(data_sock, FLAG_CLOSE_TX);

   return FTP_receive_message(handle, ctrl_fd);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : FTP_execute_command
* Returned Value   : void
* Comments  :  Transfer data over the data connection
*
*END*-----------------------------------------------------------------*/

void FTP_execute_command
   (
      uint_32     data_sock,
      boolean     sending,
      MQX_FILE_PTR data_fd
   )
{ /* Body */
   char *buff;
   int_32 length;
   int_32 sendLength,index = 0;
   

   buff = RTCS_mem_alloc(FTPc_buffer_size);
   if (!buff) {
      return;
   }
   _mem_set_type(buff, MEM_TYPE_FTPc_COMMAND_BUFFER);

   if (sending) {
      for (;;) {
         length = read(data_fd, buff, FTPc_buffer_size);
        if (length <= 0) break;

         while((sendLength = send(data_sock, &buff[index], length, 0)) != length)
         {
            if(sendLength == RTCS_ERROR) {
               break;
            }
               _time_delay(1);
               index += sendLength;
               length = length - sendLength;
         }
      } /* Endfor */

   } else {
      for (;;) {
         length = recv(data_sock, buff, FTPc_buffer_size, 0);
        if (length <= 0) break;
         write(data_fd, buff, length);
      } /* Endfor */
   } /* Endif */

   _mem_free(buff);
} /* Endbody */


/* EOF */
