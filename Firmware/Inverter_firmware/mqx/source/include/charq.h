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
* $FileName: charq.h$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file includes the definitions for a character queue data
*   structure.
*
*END************************************************************************/
#ifndef __charq_h__
#define __charq_h__

/*--------------------------------------------------------------------------*/
/*
 *                            MACRO DEFINITIONS
 */


/* Initialize the character queue */
#define _CHARQ_INIT(cq,max_size) \
   (cq)->MAX_SIZE = max_size; \
   (cq)->CURRENT_SIZE = 0; \
   (cq)->HEAD         = 0; \
   (cq)->TAIL         = 0;


/* 
 * Remove a character to the TAIL of the queue 
 * (Normal writing location for the queue)
 */
#define _CHARQ_ENQUEUE(cq,c) \
   if ( (cq)->CURRENT_SIZE < (cq)->MAX_SIZE ) { \
      (cq)->QUEUE[(cq)->TAIL++] = (char)(c); \
      if ( (cq)->TAIL == (cq)->MAX_SIZE ) { \
         (cq)->TAIL = 0; \
      } /* Endif */ \
      ++(cq)->CURRENT_SIZE; \
   } /* Endif */ 


/* 
 * Add a character from the HEAD of the queue 
 * (Reading location)
 */
#define _CHARQ_ENQUEUE_HEAD(cq,c) \
   if ( (cq)->CURRENT_SIZE < (cq)->MAX_SIZE ) { \
      if ((cq)->HEAD == 0) { \
        (cq)->HEAD = (cq)->MAX_SIZE; \
      } /* Endif */ \
      --(cq)->HEAD; \
     (cq)->QUEUE[(cq)->HEAD] = c; \
     (cq)->CURRENT_SIZE++; \
   } /* Endif */

   
/* 
 * Remove a character from the HEAD of the queue 
 * (Normal reading location for the queue)
 */
#define _CHARQ_DEQUEUE(cq,c) \
   if ( (cq)->CURRENT_SIZE ) { \
      --(cq)->CURRENT_SIZE; \
      c = (cq)->QUEUE[((cq)->HEAD++)]; \
      if ( (cq)->HEAD == (cq)->MAX_SIZE ) { \
         (cq)->HEAD = 0; \
      } /* Endif */ \
   } /* Endif */


/* 
 * Remove a character from the TAIL of the queue 
 * (Writing location)
 */
#define _CHARQ_DEQUEUE_TAIL(cq,c) \
   if ( (cq)->CURRENT_SIZE ) { \
      --(cq)->CURRENT_SIZE; \
      if ( (cq)->TAIL == 0 ) { \
         (cq)->TAIL = (cq)->MAX_SIZE; \
      } /* Endif */ \
      c = (cq)->QUEUE[--(cq)->TAIL]; \
   } /* Endif */


/* Return the current size of the queue */
#define _CHARQ_SIZE(cq) ((cq)->CURRENT_SIZE)


/* Return whether the queue is empty */
#define _CHARQ_EMPTY(cq) ((cq)->CURRENT_SIZE == 0)


/* Return whether the queue is full */
#define _CHARQ_FULL(q) ((q)->CURRENT_SIZE >= (q)->MAX_SIZE)


/* Return whether the queue is NOT full */
#define _CHARQ_NOT_FULL(q) ((q)->CURRENT_SIZE < (q)->MAX_SIZE)


/*--------------------------------------------------------------------------*/
/*
 *                            DATATYPE DECLARATIONS
 */

/*---------------------------------------------------------------------
 *
 * CHARQ STRUCTURE
 */
 
/*! 
 * \brief This structure used to store a circular character queue.
 *  
 * The structure must be the LAST if it is included into another data structure, 
 * as the character queue falls off of the end of this structure.
 */
typedef struct charq_struct
{
   
   /*! 
    * \brief The maximum number of characters for the queue, as specified in
    * initialization of the queue.
    */
   _mqx_uint  MAX_SIZE;   

   /*! \brief The current number of characters in the queue. */
   _mqx_uint  CURRENT_SIZE;  

   /*! \brief Index of the first character in queue. */
   _mqx_uint  HEAD;          

   /*! \brief Index of the last character in queue. */
   _mqx_uint  TAIL;          

   /*! \brief The character queue itself. */
   char      QUEUE[4];

} CHARQ_STRUCT, _PTR_ CHARQ_STRUCT_PTR;

#endif

/* EOF */
