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
* $FileName: coff.c$
* $Version : 3.5.9.0$
* $Date    : Jan-22-2010$
*
* Comments:
*
*   This file contains the COFF File decoder for the
*   Exec function of the RTCS Communication Library.
*
*END************************************************************************/

#include <rtcs.h>
#include "dhcpsrv.h"
/* The COFF decoder information to pass to RTCS_load() */
uint_32 COFF_start  (pointer ff_data);
uint_32 COFF_decode (uint_32 size, uchar_ptr data);
uint_32 COFF_eod    (void);
void    COFF_exec   (void);

static FF_CALL_STRUCT ff_coff = {
   COFF_start,
   COFF_decode,
   COFF_eod,
   COFF_exec
};

const FF_CALL_STRUCT_PTR FF_COFF = &ff_coff;

#if PSP_ENDIAN == MQX_BIG_ENDIAN

   #define COFF_GET_UINT_16(i) \
      (((uint_16)COFF_config.TMP[i+0] << 8) | \
        (uint_16)COFF_config.TMP[i+1]);       \
      i+=2;

   #define COFF_GET_UINT_32(i) \
      (((uint_32)COFF_config.TMP[i+0] << 24) | \
       ((uint_32)COFF_config.TMP[i+1] << 16) | \
       ((uint_32)COFF_config.TMP[i+2] <<  8) | \
        (uint_32)COFF_config.TMP[i+3]);        \
      i+=4;

#else

   #define COFF_GET_UINT_16(i) \
      (((uint_16)COFF_config.TMP[i+1] << 8) | \
        (uint_16)COFF_config.TMP[i+0]);       \
      i+=2;

   #define COFF_GET_UINT_32(i) \
      (((uint_32)COFF_config.TMP[i+3] << 24) | \
       ((uint_32)COFF_config.TMP[i+2] << 16) | \
       ((uint_32)COFF_config.TMP[i+1] <<  8) | \
        (uint_32)COFF_config.TMP[i+0]);        \
      i+=4;

#endif

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif

/* Decoder States */
#define COFF_GETTING_HEADER          1
#define COFF_GETTING_OPTIONAL_HEADER 2
#define COFF_GETTING_SECTION_HEADERS 3
#define COFF_LOADING_SECTIONS        4
#define COFF_LOADING_DONE            5


/*
** Sizes of the COFF headers, in bytes
*/
#ifdef PSP_TI32060
#  define COFF_HEADER_SIZE         22
#  define COFF_SECTION_HEADER_SIZE 48
#else
#  define COFF_HEADER_SIZE         20
#  define COFF_SECTION_HEADER_SIZE 40
#endif

/*
** COFF HEADER STRUCT
** this structure defines what the first few bytes of a COFF file look like.
*/
typedef struct coff_header_struct {

   uint_16 C_MAGIC;        /* Magic Number */
   uint_16 C_NSCNS;        /* The Number of sections */
   uint_32 C_TIMDAT;       /* The time and date stamp */
   uint_32 C_SYMPTR;       /* The file pointer to symbol table */
   uint_32 C_NSYMS;        /* The number of symbol table entries */
   uint_16 C_OPTHDR;       /* The optional header size */
   uint_16 C_FLAGS;        /* Flags */

} COFF_HEADER_STRUCT, _PTR_ COFF_HEADER_STRUCT_PTR;

/*
** COFF OPTIONAL HEADER STRUCT
** this structure defines what the COFF optional information contains
** this is usually System V a.out header
*/
typedef struct coff_optional_header_struct {

   uint_16 CO_MAGIC;       /* Magic Number */
   uint_16 CO_VSTAMP;      /* Version Stamp */
   uint_32 CO_TSIZE;       /* Text size in bytes */
   uint_32 CO_DSIZE;       /* Initialized data size in bytes */
   uint_32 CO_BSIZE;       /* Uninitialized data size in bytes */
   uint_32 CO_ENTRY;       /* Entry point */
   uint_32 CO_TEXT_START;  /* Base of text used for this file */
   uint_32 CO_DATA_START;  /* Base of data used for this file */

} COFF_OPTIONAL_HEADER_STRUCT, _PTR_ COFF_OPTIONAL_HEADER_STRUCT_PTR;

/*
** COFF SECTION HEADER STRUCT
** This structure defines what a COFF section header looks like
*/
typedef struct coff_section_header_struct {

   uchar   CS_NAME[8];     /* Section name */
   uint_32 CS_PADDR;       /* Physical address */
   uint_32 CS_VADDR;       /* Virtual address */
   uint_32 CS_SIZE;        /* Section size */
   uint_32 CS_SCNPTR;      /* File pointer to raw data for section */
   uint_32 CS_RELPTR;      /* File opinter to relocation entries */
   uint_32 CS_LNNOPTR;     /* File pointer to line number entries */
   uint_16 CS_NRELOC;      /* Number of relocation entries */
   uint_16 CS_NLNNO;       /* Number of line number entries */
   uint_32 CS_FLAGS;       /* Type and content flags */

} COFF_SECTION_HEADER_STRUCT, _PTR_ COFF_SECTION_HEADER_STRUCT_PTR;


/* COFF decoder state information */
static struct {
   uint_32                         STATE;   /* the current state */
   COFF_HEADER_STRUCT              FILE_HEADER;
   COFF_OPTIONAL_HEADER_STRUCT     OPTIONAL_FILE_HEADER;
   COFF_SECTION_HEADER_STRUCT_PTR  SECTION_HEADERS;
   uint_32                         COUNT;
   uint_32                         TOTAL_COUNT;
   uint_32                         SECTION_COUNT;
   uchar_ptr                       LOAD_ADDR;
   uchar                           TMP[128];
} COFF_config;


#define COFF_NEXT_SECTION(h)  \
   (h) = &COFF_config.SECTION_HEADERS[COFF_config.SECTION_COUNT];          \
   for (;;) {                                                              \
      if (COFF_config.SECTION_COUNT == COFF_config.FILE_HEADER.C_NSCNS) {  \
         COFF_config.STATE = COFF_LOADING_DONE;                            \
         return RTCS_OK;                                                   \
      } /* Endif */                                                        \
      if (((h)->CS_VADDR  == 0)                                            \
       || ((h)->CS_SIZE   == 0)                                            \
       || ((h)->CS_SCNPTR == 0)) {                                         \
         COFF_config.SECTION_COUNT++;                                      \
         (h)++;                                                            \
         continue;                                                         \
      } /* Endif */                                                        \
      break;                                                               \
   } /* Endfor */                                                          \
   COFF_config.LOAD_ADDR = (uchar_ptr)(h)->CS_VADDR;



/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : COFF_start
* Returned Value   : error code
* Comments  :  Routine to begin decoding a HEX file.
*
*END*-----------------------------------------------------------------*/

uint_32 COFF_start
   (
      pointer  data
   )
{ /* Body */

   COFF_config.STATE        = COFF_GETTING_HEADER;
   COFF_config.COUNT        = 0;
   COFF_config.TOTAL_COUNT  = 0;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : COFF_decode
* Returned Value   : error code
* Comments  :  Routine to decode Coff Files
*
*END*-----------------------------------------------------------------*/

uint_32 COFF_decode
   (
      /* [IN] number of bytes to decode */
      uint_32 size,

      /* [IN] bytes to decode */
      uchar_ptr data
   )
{ /* Body */
   COFF_SECTION_HEADER_STRUCT_PTR h_ptr;
   uchar_ptr t_ptr;
   uint_32   i;
   uint_32   amount;
   uchar     c;

   while (size--) {
      c = ntohc(data);
      data++;
      COFF_config.TOTAL_COUNT++;

      switch (COFF_config.STATE) {

         case COFF_GETTING_HEADER:
            COFF_config.TMP[COFF_config.COUNT++] = c;
            if (COFF_config.COUNT == COFF_HEADER_SIZE) {
               i = 0;
               COFF_config.FILE_HEADER.C_MAGIC  = COFF_GET_UINT_16(i);
               COFF_config.FILE_HEADER.C_NSCNS  = COFF_GET_UINT_16(i);
               COFF_config.FILE_HEADER.C_TIMDAT = COFF_GET_UINT_32(i);
               COFF_config.FILE_HEADER.C_SYMPTR = COFF_GET_UINT_32(i);
               COFF_config.FILE_HEADER.C_NSYMS  = COFF_GET_UINT_32(i);
               COFF_config.FILE_HEADER.C_OPTHDR = COFF_GET_UINT_16(i);
               COFF_config.FILE_HEADER.C_FLAGS  = COFF_GET_UINT_16(i);

               COFF_config.COUNT = 0;
               COFF_config.STATE = COFF_GETTING_OPTIONAL_HEADER;

               COFF_config.SECTION_HEADERS = RTCS_mem_alloc(
                  COFF_config.FILE_HEADER.C_NSCNS *
                  sizeof(COFF_SECTION_HEADER_STRUCT));
               if (COFF_config.SECTION_HEADERS == NULL) {
                  COFF_eod();
                  return RTCSERR_COFF_ALLOC_FAILED;
               } /* Endif */

               /* Cannot verify magic, as it is different on very many architectures */
               if (COFF_config.FILE_HEADER.C_MAGIC == 0) {
                  COFF_eod();
                  return RTCSERR_COFF_BAD_MAGIC;
               } /* Endif */
            } /* Endif */
            break;

         case COFF_GETTING_OPTIONAL_HEADER:
            COFF_config.TMP[COFF_config.COUNT++] = c;
            if (COFF_config.COUNT == COFF_config.FILE_HEADER.C_OPTHDR) {
               i = 0;
               COFF_config.OPTIONAL_FILE_HEADER.CO_MAGIC      = COFF_GET_UINT_16(i);
               COFF_config.OPTIONAL_FILE_HEADER.CO_VSTAMP     = COFF_GET_UINT_16(i);
               COFF_config.OPTIONAL_FILE_HEADER.CO_TSIZE      = COFF_GET_UINT_32(i);
               COFF_config.OPTIONAL_FILE_HEADER.CO_DSIZE      = COFF_GET_UINT_32(i);
               COFF_config.OPTIONAL_FILE_HEADER.CO_BSIZE      = COFF_GET_UINT_32(i);
               COFF_config.OPTIONAL_FILE_HEADER.CO_ENTRY      = COFF_GET_UINT_32(i);
               COFF_config.OPTIONAL_FILE_HEADER.CO_TEXT_START = COFF_GET_UINT_32(i);
               COFF_config.OPTIONAL_FILE_HEADER.CO_DATA_START = COFF_GET_UINT_32(i);

               COFF_config.COUNT = 0;
               COFF_config.STATE = COFF_GETTING_SECTION_HEADERS;
               COFF_config.SECTION_COUNT = 0;

               if (COFF_config.OPTIONAL_FILE_HEADER.CO_MAGIC == 0) {
                  COFF_eod();
                  return RTCSERR_COFF_BAD_MAGIC;
               } /* Endif */
            } /* Endif */
            break;

         case COFF_GETTING_SECTION_HEADERS:

            /* Program headers should follow the COFF headers */
            COFF_config.TMP[COFF_config.COUNT++] = c;
            if (COFF_config.COUNT == COFF_SECTION_HEADER_SIZE) {
               h_ptr = &COFF_config.SECTION_HEADERS[COFF_config.SECTION_COUNT++];
               for (i = 0; i < 8; i++) {
                  h_ptr->CS_NAME[i] = COFF_config.TMP[i];
               } /* Endfor */
               h_ptr->CS_PADDR   = COFF_GET_UINT_32(i);
               h_ptr->CS_VADDR   = COFF_GET_UINT_32(i);
               h_ptr->CS_SIZE    = COFF_GET_UINT_32(i);
               h_ptr->CS_SCNPTR  = COFF_GET_UINT_32(i);
               h_ptr->CS_RELPTR  = COFF_GET_UINT_32(i);
               h_ptr->CS_LNNOPTR = COFF_GET_UINT_32(i);
               h_ptr->CS_NRELOC  = COFF_GET_UINT_16(i);
               h_ptr->CS_NLNNO   = COFF_GET_UINT_16(i);
               h_ptr->CS_FLAGS   = COFF_GET_UINT_32(i);
               COFF_config.COUNT = 0;
               if (COFF_config.SECTION_COUNT == COFF_config.FILE_HEADER.C_NSCNS) {
                  COFF_config.STATE = COFF_LOADING_SECTIONS;
                  COFF_config.SECTION_COUNT = 0;
                  COFF_NEXT_SECTION(h_ptr);
               } /* Endif */
            } /* Endif */
         break;

         case COFF_LOADING_SECTIONS:
            /* Loadable sections should follow the Program headers */
            h_ptr = &COFF_config.SECTION_HEADERS[COFF_config.SECTION_COUNT];
            if (COFF_config.TOTAL_COUNT > h_ptr->CS_SCNPTR) {
               /* We have reached the correct location in the file */
               t_ptr = COFF_config.LOAD_ADDR;
               *t_ptr++ = c;
               amount = min(size, (h_ptr->CS_SIZE - COFF_config.COUNT - 1));
               COFF_config.COUNT += amount + 1;
               COFF_config.TOTAL_COUNT += amount;
               size -= amount;
               while (amount--) {
                  *t_ptr++ = ntohc(data);
                  data++;
               } /* Endwhile */
               COFF_config.LOAD_ADDR = t_ptr;
               if (COFF_config.COUNT >= h_ptr->CS_SIZE) {
                  COFF_config.COUNT = 0;
                  COFF_config.SECTION_COUNT++;
                  COFF_NEXT_SECTION(h_ptr);
               } /* Endif */
            } /* Endif */
         break;

         case COFF_LOADING_DONE:
            /* Nothing to do, toss the data */
            return RTCS_OK;

         default:
            /* Should never get here. */
            COFF_eod();
            return RTCSERR_COFF_ERROR;
      } /* Endswitch */

   } /* Endwhile */

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : COFF_eod
* Returned Value   : error code
* Comments  :  Routine to end decoding an COFF file.
*
*END*-----------------------------------------------------------------*/

uint_32 COFF_eod
   (
      void
   )
{ /* Body */

   if (COFF_config.SECTION_HEADERS)  {
      _mem_free(COFF_config.SECTION_HEADERS);
   } /* Endif */
   COFF_config.SECTION_HEADERS = NULL;

   switch (COFF_config.STATE) {
      case COFF_LOADING_DONE:
         return RTCS_OK;
      default:
         return RTCSERR_COFF_TRUNCATED;
   } /* Endswitch */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : COFF_exec
* Returned Value   : none
* Comments  :  Routine to execute a decoded COFF file.
*
*END*-----------------------------------------------------------------*/

void COFF_exec
   (
      void
   )
{ /* Body */

   (*(void(_CODE_PTR_)(void))(COFF_config.OPTIONAL_FILE_HEADER.CO_ENTRY))();

} /* Endbody */


/* EOF */
