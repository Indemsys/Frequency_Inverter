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
* $FileName: eds.c$
* $Version : 3.8.10.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   Source for the Embedded Debug Server.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

#define EDS_PORT          5002
#define EDS_TASK          10
#define EDS_BIG_ENDIAN    0L
#define EDS_LITTLE_ENDIAN 0xFFFFFFFF

/* server operations */
#define EDS_IDENTIFY 1L
#define EDS_READ     2L
#define EDS_WRITE    3L
#define EDS_LITTLE_ENDIAN_IDENTIFY 0x01000000

/* error codes */
#define EDS_INVALID_OP   1L
#define EDS_INVALID_SIZE 2L
#define EDS_OK           0L

/* structure declarations */
typedef struct eds_op_struct {
   uchar OPERATION[4];   /* server operation          */
   uchar ADDRESS[4];     /* read/write memory address */
   uchar ADDRESS2[4];    /* extra address field       */
   uchar SIZE[4];        /* size of buffer            */
   uchar PROCESSOR[4];   /* processor type            */
   uchar ENDIAN[4];      /* endian of processor       */
   uchar EDS_ERROR[4];   /* error code                */
} EDS_OP_STRUCT, _PTR_ EDS_OP_STRUCT_PTR;

/* macros */
#define EDS_OP_STRUCT_SIZE (sizeof(EDS_OP_STRUCT))
#define EDS_BUFFER_SIZE    (1472 - EDS_OP_STRUCT_SIZE)

#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 8)

#define READ_BYTES(dest,src,size)   _mem_copy(dest,src,size)
#define WRITE_BYTES(dest,src,size)  _mem_copy(dest,src,size)

#elif (PSP_ENDIAN == MQX_BIG_ENDIAN)

#define READ_BYTES(src,dest,size) \
                              {\
                                 uint_32 i, j;\
                                 uint_32 _PTR_ src_lp;\
                                 uchar   _PTR_ dest_lp;\
                                 src_lp = (src);\
                                 dest_lp = (dest);\
                                 for(i=0;i<(size);i+=4) {\
                                    j = *src_lp++;\
                                    *dest_lp++ = (j >> 24) & 0xFF;\
                                    *dest_lp++ = (j >> 16) & 0xFF;\
                                    *dest_lp++ = (j >>  8) & 0xFF;\
                                    *dest_lp++ = (j)       & 0xFF;\
                                 }\
                              }

#define WRITE_BYTES(src,dest,size) \
                              {\
                                 uint_32 i;\
                                 uchar   _PTR_ src_lp;\
                                 uint_32 _PTR_ dest_lp;\
                                 src_lp = (src);\
                                 dest_lp = (dest);\
                                 for(i=0;i<(size);i+=4) {\
                                    *dest_lp++ = (((src_lp[0] & 0xFF) << 24) |\
                                                  ((src_lp[1] & 0xFF) << 16) |\
                                                  ((src_lp[2] & 0xFF) <<  8) |\
                                                  ((src_lp[3] & 0xFF)));\
                                    src_lp+=4;\
                                 }\
                              }

#else /* PSP_ENDIAN == MQX_LITTLE_ENDIAN */

#define READ_BYTES(src,dest,size) \
                              {\
                                 uint_32 i, j;\
                                 uint_32 _PTR_ src_lp;\
                                 uchar   _PTR_ dest_lp;\
                                 src_lp = (src);\
                                 dest_lp = (dest);\
                                 for(i=0;i<(size);i+=4) {\
                                    j = *src_lp++;\
                                    *dest_lp++ = (j)       & 0xFF;\
                                    *dest_lp++ = (j >>  8) & 0xFF;\
                                    *dest_lp++ = (j >> 16) & 0xFF;\
                                    *dest_lp++ = (j >> 24) & 0xFF;\
                                 }\
                              }
#define WRITE_BYTES(src,dest,size) \
                              {\
                                 uint_32 i;\
                                 uchar   _PTR_ src_lp;\
                                 uint_32 _PTR_ dest_lp;\
                                 src_lp = (src);\
                                 dest_lp = (dest);\
                                 for(i=0;i<(size);i+=4) {\
                                    *dest_lp++ = ((src_lp[0] & 0xFF)        |\
                                                 ((src_lp[1] & 0xFF) <<  8) |\
                                                 ((src_lp[2] & 0xFF) << 16) |\
                                                 ((src_lp[3] & 0xFF) << 24));\
                                    src_lp+=4;\
                                 }\
                              }

#endif


/* #define DEBUG  */   /* Uncomment to enable debug printfs */
#ifdef DEBUG
#define DEBUGM(x) x
#else
#define DEBUGM(x)
#endif


/* Start CR 1658 */
typedef struct eds_context  {
   uint_32     UDPSOCK;
   uint_32     LISTENSOCK;
   char _PTR_  BUFFER_PTR;
} EDS_CONTEXT, _PTR_ EDS_CONTEXT_PTR;

_rtcs_taskid EDS_task_id = 0;
//char _PTR_  buffer;

#ifdef __MQX__ 
void EDS_Exit_handler(void);
#endif
void EDS_Process_TCP_packet(uint_32, char _PTR_ );
void EDS_Process_UDP_packet(uint_32, char _PTR_);
/* End CR 1658 */
void EDS_Check_errors(EDS_OP_STRUCT_PTR);
void EDS_identify(EDS_OP_STRUCT_PTR);
void EDS_task_internal (pointer, pointer);

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : EDS_init
* Returned Value   : error code
* Comments  :  Start the EDS task.
*
*END*-----------------------------------------------------------------*/

uint_32 EDS_init
   (
      char_ptr name,
      uint_32  priority,
      uint_32  stacksize
   )
{ /* Body */
   /* Start CR 1658 */
   if (EDS_task_id == 0)  {
      return RTCS_task_create(name, priority, stacksize, EDS_task_internal, NULL);
   } else  {
      return RTCSERR_SERVER_ALREADY_RUNNING;
   }
   /* End CR 1658 */
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : EDS_task
* Returned Value   : none
* Comments  :  The Embedded Debug Server.
*
*END*-----------------------------------------------------------------*/

void EDS_task
   (
      uint_32 dummy
   )
{ /* Body */
   EDS_task_internal(NULL, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : EDS_task_internal
* Returned Value   : none
* Comments  :  The Embedded Debug Server.
*
*END*-----------------------------------------------------------------*/

void EDS_task_internal
   (
      pointer dummy,
      pointer creator
   )
{ /* Body */
   /* Start CR 1658 */
   sockaddr_in    laddr;
   EDS_CONTEXT    eds_context =  { 0 };
   uint_32        sock;
   uint_32        optval, optlen;
   uint_32        error;

   DEBUGM(printf("\nRTCS Embedded Debug Server");)

   EDS_task_id = RTCS_task_getid();
#ifdef __MQX__ 
   /* Set up exit handler and context so that we can clean up if EDS is terminated */
   _task_set_environment( _task_get_id(), (pointer) &eds_context );
   _task_set_exit_handler( _task_get_id(), EDS_Exit_handler );
#endif 
   
   eds_context.BUFFER_PTR = RTCS_mem_alloc_zero(EDS_BUFFER_SIZE);
   if (!eds_context.BUFFER_PTR) {
      DEBUGM(printf("\n_mem_alloc failed ");)
      RTCS_task_exit(creator, RTCSERR_OUT_OF_MEMORY);
   } /* Endif */
   _mem_set_type(eds_context.BUFFER_PTR, MEM_TYPE_EDS_BUFFER);

   laddr.sin_family      = AF_INET;
   laddr.sin_port        = EDS_PORT;
   laddr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   eds_context.UDPSOCK = socket(PF_INET, SOCK_DGRAM, 0);
   if (eds_context.UDPSOCK == RTCS_SOCKET_ERROR) {
      DEBUGM(printf("\nCreate datagram socket failed");)
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(eds_context.UDPSOCK,(const sockaddr *) &laddr, sizeof(laddr));
   if (error != RTCS_OK) {
      DEBUGM(printf("\nDatagram bind failed - 0x%lx", error);)
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Listen on TCP port */
   eds_context.LISTENSOCK = socket(PF_INET, SOCK_STREAM, 0);
   if (eds_context.LISTENSOCK == RTCS_SOCKET_ERROR) {
      DEBUGM(printf("\nCreate stream socket failed");)
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(eds_context.LISTENSOCK,(const sockaddr *) &laddr, sizeof(laddr));
   if (error != RTCS_OK) {
      DEBUGM(printf("\nStream bind failed - 0x%lx", error);)
      RTCS_task_exit(creator, error);
   } /* Endif */
   error = listen(eds_context.LISTENSOCK, 0);
   if (error != RTCS_OK) {
      DEBUGM(printf("\nListen failed - 0x%lx", error);)
      RTCS_task_exit(creator, error);
   } /* Endif */

   RTCS_task_resume_creator(creator, RTCS_OK);

   DEBUGM(printf("\n\nEmbedded Debug Server active (port %d)\n", EDS_PORT);)

   while (TRUE) {

      sock = RTCS_selectall(0);

      if (sock == eds_context.LISTENSOCK) {
         /* Connect received; issue ACCEPT on listening stream socket */
         sock = accept(eds_context.LISTENSOCK, NULL, NULL);
         if (sock == RTCS_SOCKET_ERROR) {
            DEBUGM(printf("\nAccept failed, error 0x%lx",
               RTCS_geterror(listensock));)
         } /* Endif */
         DEBUGM(printf("\nAccepted connection");)
      } else {
         /* Get the socket type */
         optlen = sizeof(optval);
         getsockopt(sock, SOL_SOCKET, OPT_SOCKET_TYPE, &optval, &optlen);

         if (optval == SOCK_STREAM) {
            EDS_Process_TCP_packet(sock, eds_context.BUFFER_PTR);
         } else {   /* optval == SOCK_DGRAM */
            EDS_Process_UDP_packet(sock, eds_context.BUFFER_PTR);
         } /* Endif */

      } /* Endif */
   } /* Endwhile */
   /* End CR 1658 */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : EDS_Process_TCP_packet
* Returned Value : void
* Comments       : Receives a stream packet and processes it according to the
*                  client's request.
*
*END------------------------------------------------------------------*/

void EDS_Process_TCP_packet
(
   uint_32     sock,
   /* Start CR 1658 */
   char _PTR_  buffer
   /* End CR 1658 */
)
{ /* Body */
   EDS_OP_STRUCT_PTR      op_ptr;
   sockaddr_in            raddr;
   int_32                 length;
   uint_16                rlen;

   _mem_zero((char_ptr)&raddr, sizeof(raddr));
   rlen = sizeof(raddr);

   getpeername(sock,(sockaddr *)&raddr, &rlen);
   length = recv(sock, buffer, EDS_OP_STRUCT_SIZE, 0);
   if (length == RTCS_ERROR) {
     shutdown(sock, FLAG_CLOSE_TX);
     DEBUGM(printf("\nClosed connection to %d.%d.%d.%d, port %d",
        (raddr.sin_addr.s_addr >> 24) & 0xFF,
        (raddr.sin_addr.s_addr >> 16) & 0xFF,
        (raddr.sin_addr.s_addr >>  8) & 0xFF,
         raddr.sin_addr.s_addr        & 0xFF,
         raddr.sin_port);)
      return;
   } else {
      DEBUGM(printf("\n%d bytes received from %d.%d.%d.%d, port %d",
         length,
         (raddr.sin_addr.s_addr >> 24) & 0xFF,
         (raddr.sin_addr.s_addr >> 16) & 0xFF,
         (raddr.sin_addr.s_addr >>  8) & 0xFF,
          raddr.sin_addr.s_addr        & 0xFF,
          raddr.sin_port);)
   } /* Endif */

   op_ptr = (EDS_OP_STRUCT_PTR)buffer;

   EDS_Check_errors(op_ptr);
   if ((ntohl(op_ptr->EDS_ERROR)) != MQX_OK) {
      DEBUGM(printf("\nOperation error %d", op_ptr->EDS_ERROR);)
      send(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE, 0);
   } else {
      /* Determine and handle client request */
      switch(ntohl(op_ptr->OPERATION)) {

         case EDS_IDENTIFY:
         case EDS_LITTLE_ENDIAN_IDENTIFY:
            EDS_identify(op_ptr);

            /* Send the identity */
            length = send(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE, 0);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         case EDS_READ:
            READ_BYTES((pointer)(ntohl(op_ptr->ADDRESS)),
            (pointer)&buffer[EDS_OP_STRUCT_SIZE], ntohl(op_ptr->SIZE));
            /* Send the result status and the data to read */
            length = send(sock, buffer, EDS_OP_STRUCT_SIZE + ntohl(op_ptr->SIZE), 0);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         case EDS_WRITE:
            /* Write data directly to memory */
            length = recv(sock, (char_ptr)(&buffer[EDS_OP_STRUCT_SIZE]), ntohl(op_ptr->SIZE), 0);
            WRITE_BYTES((pointer)&buffer[EDS_OP_STRUCT_SIZE],
               (pointer)(ntohl(op_ptr->ADDRESS)), ntohl(op_ptr->SIZE));
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n%d bytes received from %d.%d.%d.%d, port %d",
                  length,
                  (raddr.sin_addr.s_addr >> 24) & 0xFF,
                  (raddr.sin_addr.s_addr >> 16) & 0xFF,
                  (raddr.sin_addr.s_addr >>  8) & 0xFF,
                   raddr.sin_addr.s_addr        & 0xFF,
                   raddr.sin_port);)
            } /* Endif */

            /* Send the result status */
            length = send(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE,
               0);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         default:
            break;
      } /* Endswitch */
   }/* Endif */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : EDS_Process_UDP_packet
* Returned Value : void
* Comments       : Receives a datagram packet and processes it according to the
*                  client's request.
*
*END------------------------------------------------------------------*/

void EDS_Process_UDP_packet
(
   uint_32                   sock,
   /* Start CR 1658 */
   char _PTR_  buffer
   /* End CR 1658 */
)
{ /* Body */
   EDS_OP_STRUCT_PTR      op_ptr;
   sockaddr_in            raddr;
   uint_16                rlen;
   int_32                 length;

   _mem_zero((char_ptr)&raddr, sizeof(raddr));
   rlen = sizeof(raddr);

   length = recvfrom(sock, buffer, EDS_BUFFER_SIZE, 0, (sockaddr *)&raddr, &rlen);
   if (length == RTCS_ERROR) {
      DEBUGM(printf("\nError %d receiving from %d.%d.%d.%d, port %d",
         RTCS_geterror(sock),
         (raddr.sin_addr.s_addr >> 24) & 0xFF,
         (raddr.sin_addr.s_addr >> 16) & 0xFF,
         (raddr.sin_addr.s_addr >>  8) & 0xFF,
          raddr.sin_addr.s_addr        & 0xFF,
          raddr.sin_port);)
   } else {
      /* Success on receive */
      DEBUGM(printf("\nDatagram (%d bytes) received from %d.%d.%d.%d, port %d",
         length,
         (raddr.sin_addr.s_addr >> 24) & 0xFF,
         (raddr.sin_addr.s_addr >> 16) & 0xFF,
         (raddr.sin_addr.s_addr >>  8) & 0xFF,
          raddr.sin_addr.s_addr        & 0xFF,
          raddr.sin_port);)
   } /* Endif */

   op_ptr = (EDS_OP_STRUCT_PTR)buffer;

   EDS_Check_errors(op_ptr);
   if ((ntohl(op_ptr->EDS_ERROR)) != MQX_OK) {
      DEBUGM(printf("\nOperation error %d", op_ptr->EDS_ERROR);)
      sendto(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE, 0, (sockaddr *)&raddr, rlen);
   } else {
      /* Determine and handle client request */
      switch(ntohl(op_ptr->OPERATION)) {

         case EDS_IDENTIFY:
         case EDS_LITTLE_ENDIAN_IDENTIFY:
            EDS_identify(op_ptr);

            length = sendto(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE,
                            0, (sockaddr *)&raddr, rlen);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         case EDS_READ:
            /* Copy data read into buffer */
            READ_BYTES((pointer)(ntohl(op_ptr->ADDRESS)),
            (pointer)&buffer[EDS_OP_STRUCT_SIZE], ntohl(op_ptr->SIZE));
            /* Send buffer */
            length = sendto(sock, buffer, EDS_OP_STRUCT_SIZE + ntohl(op_ptr->SIZE), 0,
                            (sockaddr *)&raddr, rlen);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         case EDS_WRITE:
            /* Write received data into memory */
            WRITE_BYTES((pointer)&buffer[EDS_OP_STRUCT_SIZE],
               (pointer)(ntohl(op_ptr->ADDRESS)), ntohl(op_ptr->SIZE));

            /* Send the result status */
            length = sendto(sock, (char_ptr)op_ptr, (uint_32)EDS_OP_STRUCT_SIZE,
                            0, (sockaddr *)&raddr, rlen);
            if (length == RTCS_ERROR) {
               DEBUGM(printf("\n(error 0x%lx)", RTCS_geterror(sock));)
            } else {
               DEBUGM(printf("\n(%d bytes sent)", length);)
            } /* Endif */

            break;

         default:
            break;
      } /* Endswitch */
   } /* Endif */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : EDS_Check_errors
* Returned Value : void
* Comments       : Determines if the client request is valid.
*
*END------------------------------------------------------------------*/

void EDS_Check_errors
(
   EDS_OP_STRUCT_PTR op_ptr
)
{ /* Body */
   /* Valid operation? */
   switch(ntohl(op_ptr->OPERATION)) {
      case EDS_IDENTIFY:
      case EDS_LITTLE_ENDIAN_IDENTIFY:
      case EDS_READ:
      case EDS_WRITE:
         htonl(op_ptr->EDS_ERROR,EDS_OK);
         break;
      default:
         htonl(op_ptr->EDS_ERROR,EDS_INVALID_OP);
         return;
   } /* Endswitch */

   /* Valid size? */
   if ((ntohl(op_ptr->OPERATION) == EDS_READ) || (ntohl(op_ptr->OPERATION) == EDS_WRITE)) {
      if (ntohl(op_ptr->SIZE) > (uint_32)EDS_BUFFER_SIZE) {
         htonl(op_ptr->EDS_ERROR,EDS_INVALID_SIZE);
      } else {
         htonl(op_ptr->EDS_ERROR,EDS_OK);
      } /* Endif */
   } /* Endif */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : EDS_identify
* Returned Value : void
* Comments       : Fills in an OP_STRUCT containing the processor type, endian,
*                  start and end of kernel memory of this server, and the result
*                  status.
*
*END------------------------------------------------------------------*/

void EDS_identify
(
   EDS_OP_STRUCT_PTR op_ptr
)
{ /* Body */
#ifdef __MQX__
   /* Only useful if MQX is present */

   htonl(op_ptr->PROCESSOR,_mqx_get_cpu_type());
   htonl(op_ptr->ENDIAN, MSG_HDR_ENDIAN);
   htonl(op_ptr->ADDRESS,(uint_32)_mqx_get_initialization()->START_OF_KERNEL_MEMORY);
   htonl(op_ptr->ADDRESS2,(uint_32)_mqx_get_initialization()->END_OF_KERNEL_MEMORY);
#endif
} /* Endbody */

/* Start CR 1658 */
#ifdef __MQX__ 
/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : EDS_stop
* Returned Value   : error code
* Comments  :  Start the EDS task.
*
*END*-----------------------------------------------------------------*/

uint_32 EDS_stop( void )
{ /* Body */
   if (EDS_task_id == 0)  {
      return RTCSERR_SERVER_NOT_RUNNING;
   }
   RTCS_task_abort(EDS_task_id);
   EDS_task_id = 0;
   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : EDS_Exit_handler
* Returned Value   : error code
* Comments  :  Start the EDS task.
*
*END*-----------------------------------------------------------------*/

void EDS_Exit_handler( void )
{ /* Body */
   EDS_CONTEXT_PTR    eds_context_ptr;
   
   eds_context_ptr = (EDS_CONTEXT_PTR) _task_get_environment( _task_get_id() );
   if (eds_context_ptr != NULL)  {
      if (eds_context_ptr->UDPSOCK)  {
         shutdown(eds_context_ptr->UDPSOCK, 0);
         eds_context_ptr->UDPSOCK = 0;   
      }   
      if (eds_context_ptr->LISTENSOCK)  {
         shutdown(eds_context_ptr->LISTENSOCK, FLAG_ABORT_CONNECTION);   
         eds_context_ptr->LISTENSOCK = 0;   
      }   
      if (eds_context_ptr->BUFFER_PTR)  {
         _mem_free(eds_context_ptr->BUFFER_PTR);   
         eds_context_ptr->BUFFER_PTR = 0;   
      }   
   }
   EDS_task_id = 0;
} /* Endbody */
#endif
/* End CR 1658 */

#endif
/* EOF */
