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
* $FileName: pcb.h$
* $Version : 3.8.9.0$
* $Date    : Aug-7-2012$
*
* Comments:
*
*   This file contains the definitions of the generic
*   PCB (Packet Control Block) structure.  
*   Since this structure is common to
*   various protocols, this file is distributed with
*   several products.
*
*END************************************************************************/
#ifndef __pcb_h__
#define __pcb_h__

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */


#define __PCB__

#define PCB_free(pcb_ptr)  ((pcb_ptr)->FREE(pcb_ptr))
#define htonl(p,x) (((uint_8_ptr)(p))[0] = ((x) >> 24) & 0xFF, \
                    ((uint_8_ptr)(p))[1] = ((x) >> 16) & 0xFF, \
                    ((uint_8_ptr)(p))[2] = ((x) >>  8) & 0xFF, \
                    ((uint_8_ptr)(p))[3] =  (x)        & 0xFF, \
                    (x))

#define htons(p,x) (((uint_8_ptr)(p))[0] = ((x) >>  8) & 0xFF, \
                    ((uint_8_ptr)(p))[1] =  (x)        & 0xFF, \
                    (x))

#define htonc(p,x) (((uint_8_ptr)(p))[0] = (x) & 0xFF, \
                    (x))

#define ntohl(p)   (\
                    (((uint_32)(((uint_8_ptr)(p))[0])) << 24) | \
                    (((uint_32)(((uint_8_ptr)(p))[1])) << 16) | \
                    (((uint_32)(((uint_8_ptr)(p))[2])) << 8) | \
                    ( (uint_32)(((uint_8_ptr)(p))[3])) \
                   )

#define ntohs(p)   (\
                    (((uint_16)(((uint_8_ptr)(p))[0])) << 8) | \
                    ( (uint_16)(((uint_8_ptr)(p))[1])) \
                   )

#define ntohc(p)   ((uint_8)(((uint_8_ptr)(p))[0]))

/*--------------------------------------------------------------------------*/
/*
 *                          DATA STRUCTURES
 */

/*
 * The PCB (Packet Control Block)
 * 
 * One PCB contains exactly one packet, possibly split over several areas of
 * memory.
 * 
 * The PCB structure consists of two pointers (FREE and PRIVATE) followed by a
 * variable-length array of PCB_FRAGMENT structures.  The array is terminated
 * by an entry with LENGTH=0 and FRAGMENT=NULL.
 *
 * The PCB's owner (i.e. the module that allocated it) must initialize the
 * FREE field to point to a function to free the PCB.  PRIVATE can be used by
 * the PCB's owner to store any information it wishes.
 */

struct pcb;
typedef void (_CODE_PTR_  PCB_FREE_FPTR)(struct pcb _PTR_);

/*!
 * \brief Structure that contains fragment of PCB. 
 */ 
typedef struct pcb_fragment 
{
   /*! \brief Length of PCB fragment. */
   uint_32           LENGTH;
   /*! \brief Pointer to fragment. */
   uchar_ptr         FRAGMENT;
} PCB_FRAGMENT, _PTR_ PCB_FRAGMENT_PTR;

/*!
 * \brief PCB structure. 
 * 
 * The array is terminated by an entry with LENGTH=0 and FRAGMENT=NULL.
 */ 
typedef struct pcb 
{
   /*! \brief Function that frees PCB. */
   PCB_FREE_FPTR     FREE;
   /*! \brief Private PCB information. */
   pointer           PRIVATE;
   /*! \brief Pointer to PCB fragment. */
   PCB_FRAGMENT      FRAG[1];
} PCB, _PTR_ PCB_PTR;

// The minimum size of a PCB must contain two fragments, so the following represents
// the smallest PCB
/*!
 * \brief PCB structure contains two fragments.
 * 
 * The array is terminated by an entry with LENGTH=0 and FRAGMENT=NULL. 
 */
typedef struct pcb2
{
   /*! \brief Function that frees PCB. */
   PCB_FREE_FPTR     FREE;
   /*! \brief Private PCB information. */
   pointer           PRIVATE;
   /*! \brief Pointers to two PCB fragments. */
   PCB_FRAGMENT      FRAG[2];
} PCB2, _PTR_ PCB2_PTR;

#define PCB_MINIMUM_SIZE (sizeof(PCB2))

/*--------------------------------------------------------------------------*/
/*                           EXTERNAL DECLARATIONS                          */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
