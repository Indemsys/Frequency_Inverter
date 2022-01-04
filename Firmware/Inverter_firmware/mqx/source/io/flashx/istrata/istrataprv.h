#ifndef _istrataprv_h_
#define _istrataprv_h_
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
* $FileName: istrataprv.h$
* $Version : 3.8.8.0$
* $Date    : Oct-4-2011$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for the Intel strataflash devices
*
*END************************************************************************/


/*----------------------------------------------------------------------*/
/*
**                   HARDWARE DEFINITIONS
*/
#define ISTRATA_WRITE_CMD_8X1   (0x40)
#define ISTRATA_WRITE_CMD_8X2   (0x4040)
#define ISTRATA_WRITE_CMD_8X4   (0x40404040)
#define ISTRATA_WRITE_CMD_16X1  (0x0040)
#define ISTRATA_WRITE_CMD_16X2  (0x00400040)
#define ISTRATA_WRITE_CMD_32X1  (0x00000040)

#define ISTRATA_READ_MODE_CMD_8X1   (0xFF)
#define ISTRATA_READ_MODE_CMD_8X2   (0xFFFF)
#define ISTRATA_READ_MODE_CMD_8X4   (0xFFFFFFFF)
#define ISTRATA_READ_MODE_CMD_16X1  (0x00FF)
#define ISTRATA_READ_MODE_CMD_16X2  (0x00FF00FF)
#define ISTRATA_READ_MODE_CMD_32X1  (0x000000FF)

#define ISTRATA_ERASE_CMD1_8X1      (0x20)
#define ISTRATA_ERASE_CMD2_8X1      (0xD0)
#define ISTRATA_ERASE_CMD1_8X2      (0x2020)
#define ISTRATA_ERASE_CMD2_8X2      (0xD0D0)
#define ISTRATA_ERASE_CMD1_8X4      (0x20202020)
#define ISTRATA_ERASE_CMD2_8X4      (0xD0D0D0D0)
#define ISTRATA_ERASE_CMD1_16X1     (0x0020)
#define ISTRATA_ERASE_CMD2_16X1     (0x00D0)
#define ISTRATA_ERASE_CMD1_16X2     (0x00200020)
#define ISTRATA_ERASE_CMD2_16X2     (0x00D000D0)
#define ISTRATA_ERASE_CMD1_32X1     (0x00000020)
#define ISTRATA_ERASE_CMD2_32X1     (0x000000D0)

/* Start CR 871 */
#define ISTRATA_CLEAR_CMD1_8X1      (0x60)
#define ISTRATA_CLEAR_CMD2_8X1      (0xD0)
#define ISTRATA_CLEAR_CMD1_8X2      (0x6060)
#define ISTRATA_CLEAR_CMD2_8X2      (0xD0D0)
#define ISTRATA_CLEAR_CMD1_8X4      (0x60606060)
#define ISTRATA_CLEAR_CMD2_8X4      (0xD0D0D0D0)
#define ISTRATA_CLEAR_CMD1_16X1     (0x0060)
#define ISTRATA_CLEAR_CMD2_16X1     (0x00D0)
#define ISTRATA_CLEAR_CMD1_16X2     (0x00600060)
#define ISTRATA_CLEAR_CMD2_16X2     (0x00D000D0)
#define ISTRATA_CLEAR_CMD1_32X1     (0x00000060)
#define ISTRATA_CLEAR_CMD2_32X1     (0x000000D0)
/* End CR 871 */

#define ISTRATA_SET_CMD1_8X1        (0x60)
#define ISTRATA_SET_CMD2_8X1        (0x01)
#define ISTRATA_SET_CMD1_8X2        (0x6060)
#define ISTRATA_SET_CMD2_8X2        (0x0101)
#define ISTRATA_SET_CMD1_8X4        (0x60606060)
#define ISTRATA_SET_CMD2_8X4        (0x01010101)
#define ISTRATA_SET_CMD1_16X1       (0x0060)
#define ISTRATA_SET_CMD2_16X1       (0x0001)
#define ISTRATA_SET_CMD1_16X2       (0x00600060)
#define ISTRATA_SET_CMD2_16X2       (0x00010001)
#define ISTRATA_SET_CMD1_32X1       (0x00000060)
#define ISTRATA_SET_CMD2_32X1       (0x00000001)

#define ISTRATA_STATUS_BUSY_8X1     (0x80)
#define ISTRATA_STATUS_BUSY_8X2     (0x8080)
#define ISTRATA_STATUS_BUSY_8X4     (0x80808080)
#define ISTRATA_STATUS_BUSY_16X1    (0x0080)
#define ISTRATA_STATUS_BUSY_16X2    (0x00800080)
#define ISTRATA_STATUS_BUSY_32X1    (0x00000080)

/* Start CR 2077 */
#define ISTRATA_READ_ID_MODE_CMD_16X1   (0x0090)
/* End CR 2077 */

#define ISTRATA_WRITE_BUFFER_SIZE   (32)

/* Programming timing definitions in microseconds */
/* Start CR 2077 */
#ifdef I28FXXXC3
/*
** Define the macro in Flash device specific driver file. 
** e.g. mqx2.50\source\bsp\mcfzsdk5329\i28f160c3.c
*/
#define ISTRATA_PROGRAM_WORD_TIME_MAX    200
#else
/*     I28FXXXJ3 */
#define ISTRATA_PROGRAM_WORD_TIME_MAX    630
#endif
/* End CR 2077 */

/* Erase timing definitions in seconds */
#define ISTRATA_PROGRAM_ERASE_TIME_MAX   5

/* Timing definitions in ticks */
#define ISTRATA_PROGRAM_WORD_TICKS_MAX  \
(((ISTRATA_PROGRAM_WORD_TIME_MAX * BSP_ALARM_FREQUENCY) / 1000000) + 3)
   
#define ISTRATA_PROGRAM_ERASE_TICKS_MAX  \
   (ISTRATA_PROGRAM_ERASE_TIME_MAX * BSP_ALARM_FREQUENCY)

/* Start CR 871 */
#define ISTRATA_PROGRAM_CLEAR_TICKS_MAX  ISTRATA_PROGRAM_ERASE_TICKS_MAX
/* End CR 871 */


/*----------------------------------------------------------------------*/
/*
**                   MACROS
*/

/* Macro to do the programming algorithm */
#define ISTRATA_PROGRAM(read_cmd, write_cmd, busy_cmd)                     \
   /* We only have to start where the source and destination diverged */   \
   for ( j = offset; ((j < size) && success); j++ ) {                      \
      tmp = *src_ptr++;     /* data to write */                            \
      *dest_ptr = read_cmd; /* put into read mode to compare */            \
      if (tmp != *dest_ptr) {                                              \
         *dest_ptr = write_cmd;   /* set to write mode */                  \
         *dest_ptr = tmp;         /* write the data    */                  \
         _time_get_elapsed_ticks(&start_ticks);                            \
         timeout = FALSE;                                                  \
         while (!timeout) {                                                \
            /* Read the status register */                                 \
            status = *dest_ptr;                                            \
            if ((status & busy_cmd) == busy_cmd) {                         \
               /* Program complete */                                      \
               break;                                                      \
            } /* Endif */                                                  \
            timeout = _intel_strata_check_timeout(&start_ticks,            \
               ISTRATA_PROGRAM_WORD_TICKS_MAX);                            \
         } /* Endwhile */                                                  \
         /* Start CR 874 */                                                \
         /* success = !timeout || status; */                               \
         success = !timeout;                                               \
         /* End CR 874 */                                                  \
      } /* Endif */                                                        \
      dest_ptr++;                                                          \
   } /* Endfor */                                                          \
   /* Place the device back in read mode */                                \
   dest_ptr--;                                                             \
   *dest_ptr = read_cmd

/* Macro to do the sector erase algorithm */
#define ISTRATA_ERASE(read_cmd, erase_cmd1, erase_cmd2, busy_cmd, erase_pattern) \
      /* erase sequence */                                                 \
      *dest_ptr = erase_cmd1;                                              \
      *dest_ptr = erase_cmd2;                                              \
      _time_get_elapsed_ticks(start_ticks_ptr);                            \
      timeout = FALSE;                                                     \
      while (!timeout) {                                                   \
         status = *dest_ptr;                                               \
         if ((status & busy_cmd) == busy_cmd) {                            \
            /* erase complete */                                           \
            break;                                                         \
         } /* Endif */                                                     \
         timeout = _intel_strata_check_timeout(start_ticks_ptr,            \
            ISTRATA_PROGRAM_ERASE_TICKS_MAX);                              \
      } /* Endwhile */                                                     \
      *dest_ptr = read_cmd;                                                \
      /* success = !timeout || status; */                                  \
      success = !timeout;                                                  \
      _time_get_elapsed_ticks(start_ticks_ptr);                            \
      timeout = FALSE;                                                     \
      while (!timeout) {                                                   \
        if(*dest_ptr == erase_pattern){                                    \
            break;                                                         \
        }                                                                  \
        timeout = _intel_strata_check_timeout(start_ticks_ptr,             \
            ISTRATA_PROGRAM_ERASE_TICKS_MAX);                              \
      }                                                                    \
      success = !timeout;

/* 
** We don't have to erase if bits don't change or they transition from a 1
** to a 0
*/
#define ISTRATA_ERASE_UNIT(unit_in_flash, unit_in_ram) \
   ((((unit_in_flash) ^ (unit_in_ram)) & (unit_in_ram)) != 0)

/* Start CR 871 */
/* Macro to clear the lock bits */
#define ISTRATA_CLEAR_LOCKBITS(read_cmd, clear_cmd1, clear_cmd2, busy_cmd) \
      /* clear lock bits sequence */                                       \
      *dest_ptr = clear_cmd1;                                              \
      *dest_ptr = clear_cmd2;                                              \
      _time_get_elapsed_ticks(start_ticks_ptr);                            \
      timeout = FALSE;                                                     \
      while (!timeout) {                                                   \
         status = *dest_ptr;                                               \
         if ((status & busy_cmd) == busy_cmd) {                            \
            /* clear complete */                                           \
            break;                                                         \
         } /* Endif */                                                     \
         timeout = _intel_strata_check_timeout(start_ticks_ptr,            \
            ISTRATA_PROGRAM_CLEAR_TICKS_MAX);                              \
      } /* Endwhile */                                                     \
      success = !timeout;                                                  \
      *dest_ptr = read_cmd;
/* End CR 871 */

/* Macro to set the lock bits */
#define ISTRATA_SET_LOCKBITS(read_cmd, clear_cmd1, clear_cmd2, busy_cmd) \
      /* set lock bits sequence */                                       \
      *dest_ptr = clear_cmd1;                                              \
      *dest_ptr = clear_cmd2;                                              \
      _time_get_elapsed_ticks(start_ticks_ptr);                            \
      timeout = FALSE;                                                     \
      while (!timeout) {                                                   \
         status = *dest_ptr;                                               \
         if ((status & busy_cmd) == busy_cmd) {                            \
            /* set complete */                                           \
            break;                                                         \
         } /* Endif */                                                     \
         timeout = _intel_strata_check_timeout(start_ticks_ptr,            \
            ISTRATA_PROGRAM_CLEAR_TICKS_MAX);                              \
      } /* Endwhile */                                                     \
      success = !timeout;                                                  \
      *dest_ptr = read_cmd;

/* Start CR 2077 */
/* Macro to read the lock status of the block */
#define ISTRATA_READ_LOCK_STATUS(read_cmd, readid_cmd, busy_cmd) \
      /* set lock bits sequence */                                         \
      *dest_ptr = readid_cmd;                                              \
      status = *((uint_16_ptr)dest_ptr+0x00002);                                           \
      *dest_ptr = read_cmd;
/* End CR 2077 */

/*----------------------------------------------------------------------*/
/*
**                     FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

/* Top level functions visible to the generic flashx driver */
boolean _intel_strata_program(IO_FLASHX_STRUCT_PTR, char_ptr, char_ptr, _mem_size);
boolean _intel_strata_erase(IO_FLASHX_STRUCT_PTR, char_ptr, _mem_size);
boolean _intel_strata_write_protect(IO_FLASHX_STRUCT_PTR , _mqx_uint);
_mqx_int _io_intel_strata_ioctl(IO_FLASHX_STRUCT_PTR, _mqx_uint, pointer);

/* lower level functions private to the strata flash driver */
boolean _intel_strata_clear_lock_bits(IO_FLASHX_STRUCT_PTR);
boolean _intel_strata_set_lock_bits(IO_FLASHX_STRUCT_PTR);

boolean _intel_strata_check_timeout(MQX_TICK_STRUCT_PTR, _mqx_uint);
boolean _intel_strata_program_1byte(IO_FLASHX_STRUCT_PTR, char_ptr, char_ptr, _mem_size, _mqx_uint);
boolean _intel_strata_erase_1byte(IO_FLASHX_STRUCT_PTR, char_ptr, _mem_size, MQX_TICK_STRUCT_PTR);
boolean _intel_strata_program_2byte(IO_FLASHX_STRUCT_PTR, uint_16_ptr, uint_16_ptr, _mem_size, _mqx_uint);
boolean _intel_strata_erase_2byte(IO_FLASHX_STRUCT_PTR, uint_16_ptr, _mem_size, MQX_TICK_STRUCT_PTR);
boolean _intel_strata_program_4byte(IO_FLASHX_STRUCT_PTR, uint_32_ptr, uint_32_ptr, _mem_size, _mqx_uint);
boolean _intel_strata_erase_4byte(IO_FLASHX_STRUCT_PTR, uint_32_ptr, _mem_size, MQX_TICK_STRUCT_PTR);
boolean _intel_strata_init(IO_FLASHX_STRUCT_PTR);

boolean _intel_strata_clearlockbits_1byte(IO_FLASHX_STRUCT_PTR, MQX_TICK_STRUCT_PTR);
boolean _intel_strata_clearlockbits_2byte(IO_FLASHX_STRUCT_PTR, MQX_TICK_STRUCT_PTR);
boolean _intel_strata_clearlockbits_4byte(IO_FLASHX_STRUCT_PTR, MQX_TICK_STRUCT_PTR);

boolean _intel_strata_setlockbits_1byte(IO_FLASHX_STRUCT_PTR, MQX_TICK_STRUCT_PTR);
boolean _intel_strata_setlockbits_2byte(IO_FLASHX_STRUCT_PTR, MQX_TICK_STRUCT_PTR);
boolean _intel_strata_setlockbits_4byte(IO_FLASHX_STRUCT_PTR, MQX_TICK_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
