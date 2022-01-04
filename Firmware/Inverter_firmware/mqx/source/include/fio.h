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
* $FileName: fio.h$
* $Version : 3.8.20.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file is the header file for the standard formatted I/O library 
*   provided with mqx.
*
*END************************************************************************/
#ifndef __fio_h__
#define __fio_h__

/* Include for variable length argument functions */
#include <stdarg.h>

/*--------------------------------------------------------------------------*/
/*
 *                            CONSTANT DEFINITIONS
 */

/* Maximum line size for scanf */
#define IO_MAXLINE  (256)

/* Definitions for filesystem table */
#define FS_MAX_DEVLEN         (8)
#define MAX_FS_INSTANCES      (4)
/* map function names to mqx function names */
#if !defined(MQX_SUPPRESS_STDIO_MACROS) || MQX_SUPPRESS_STDIO_MACROS == 0 
   #define  clearerr   _io_clearerr
   #define  fclose     _io_fclose
   #define  feof       _io_feof
   #define  ferror     _io_ferror
   #define  fflush     _io_fflush
   #define  fgetc      _io_fgetc
   #define  fgetline   _io_fgetline
   #define  fgets      _io_fgets
   #define  fopen      _io_fopen
   #define  fprintf    _io_fprintf
   #define  fputc      _io_fputc
   #define  fputs      _io_fputs
   #define  fscanf     _io_fscanf
   #define  fseek      _io_fseek
   #define  fstatus    _io_fstatus
   #define  ftell      _io_ftell
   #define  fungetc    _io_fungetc
   #define  ioctl      _io_ioctl 
   #define  printf     _io_printf
   #define  putc       _io_fputc
   #define  read       _io_read
   #define  scanf      _io_scanf
   #define  sprintf    _io_sprintf
   #define  snprintf   _io_snprintf
   #define  sscanf     _io_sscanf
   #define  vprintf    _io_vprintf
   #define  vfprintf   _io_vfprintf
   #define  vsprintf   _io_vsprintf
   #define  vsnprintf  _io_vsnprintf
   #define  write      _io_write
   /* fread and fwrite do not read/write chars but objects */
   #define  fread(ptr,so,no,f)  (_io_read(f,ptr,(so)*(no))/(so))
   #define  fwrite(ptr,so,no,f) (_io_write(f,ptr,(so)*(no))/(so))
#endif

#if !defined(MQX_SUPPRESS_STRINGH_MACROS) || MQX_SUPPRESS_STRINGH_MACROS == 0 
   #define  strcasecmp  _io_strcasecmp
   #define  strncasecmp _io_strncasecmp
#endif

/*--------------------------------------------------------------------------*/
/*
 *                        MACRO DECLARATIONS
 */
#if !defined(MQX_SUPPRESS_STDIO_MACROS) || MQX_SUPPRESS_STDIO_MACROS == 0 

#define stdin     (MQX_FILE_PTR)_io_get_handle(IO_STDIN)
#define stdout    (MQX_FILE_PTR)_io_get_handle(IO_STDOUT)
#define stderr    (MQX_FILE_PTR)_io_get_handle(IO_STDERR)

#define getchar()    _io_fgetc(stdin)
#define getline(x,y) _io_fgetline(stdin, (x), (y))
#define gets(x)      _io_fgets((x), 0, stdin)
#define putchar(c)   _io_fputc((c), stdout)
#define puts(s)      _io_fputs((s), stdout)
#define status()     _io_fstatus(stdin)
#define ungetc(c)    _io_fungetc(c, stdin)
#endif /* MQX_SUPPRESS_STDIO_MACROS */

 
/*--------------------------------------------------------------------------*/
/*
 *                            DATATYPE DECLARATIONS
 */


/* FILE STRUCTURE */
 
/*!
 * \brief This structure defines the information kept in order to implement ANSI 
 * 'C' standard I/O stream.
 */
typedef struct mqx_file
{
    
    /*! \brief The address of the Device for this stream. */
    struct io_device_struct _PTR_ DEV_PTR;

    /*! \brief Device Driver specific information. */
    pointer       DEV_DATA_PTR;

    /*! \brief General control flags for this stream. */
    _mqx_uint     FLAGS;
    
    /*! \brief The current error for this stream. */
    _mqx_uint     ERROR;

    /*! \brief The current position in the stream. */
    _file_size    LOCATION;

    /*! \brief The current size of the file. */
    _file_size    SIZE;

    /*! \brief Undelete implementation. */
    boolean       HAVE_UNGOT_CHARACTER;
    /*! \brief Undelete implementation. */
    _mqx_int      UNGOT_CHARACTER;

} MQX_FILE, _PTR_ MQX_FILE_PTR;

/* FILE and MQX_FILE_PTR types are deprecated, but still available for backward compatibility */
#if !defined(MQX_SUPPRESS_FILE_DEF) || MQX_SUPPRESS_FILE_DEF == 0
typedef struct mqx_file file_struct;
typedef MQX_FILE FILE;
typedef MQX_FILE_PTR FILE_PTR;
#endif
 
typedef _mqx_int (_CODE_PTR_ IO_PUTCHAR_FPTR)( _mqx_int, MQX_FILE_PTR);

/*!
 * \brief File system table entry structure.
 */ 
typedef struct 
{
   /*! \brief File system name. */
   char         FS_NAME[FS_MAX_DEVLEN];
   /*! \brief Pointer MQX File. */
   MQX_FILE_PTR FS_PTR;
} FS_TABLE_ENTRY, _PTR_ FS_TABLE_ENTRY_PTR; 

/*--------------------------------------------------------------------------*/
/*
 *                      FUNCTION PROTOTYPES
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
/* ANSI 'C' library function prototypes */
extern void        _io_clearerr(MQX_FILE_PTR);
extern _mqx_int    _io_fclose(MQX_FILE_PTR);
extern _mqx_int    _io_feof(MQX_FILE_PTR);
extern _mqx_int    _io_ferror(MQX_FILE_PTR);
extern _mqx_int    _io_fflush(MQX_FILE_PTR);
extern _mqx_int    _io_fgetc(MQX_FILE_PTR);
extern _mqx_int    _io_fgetline(MQX_FILE_PTR, char _PTR_, _mqx_int);
extern char _PTR_  _io_fgets(char _PTR_, _mqx_int, MQX_FILE_PTR);
extern MQX_FILE_PTR _io_fopen(const char _PTR_, const char _PTR_);
extern _mqx_int    _io_fprintf(MQX_FILE_PTR, const char _PTR_, ... );
extern _mqx_int    _io_fputc(_mqx_int, MQX_FILE_PTR);
extern _mqx_int    _io_fputs(const char _PTR_, MQX_FILE_PTR);
extern _mqx_int    _io_fscanf(MQX_FILE_PTR, const char _PTR_, ... );
extern _mqx_int    _io_fseek(MQX_FILE_PTR, _file_offset, _mqx_uint);
extern boolean     _io_fstatus(MQX_FILE_PTR);
extern _mqx_int    _io_ftell(MQX_FILE_PTR);
extern _mqx_int    _io_fungetc(_mqx_int, MQX_FILE_PTR);
extern _mqx_int    _io_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);
extern double      _io_modf(double, double _PTR_);
extern _mqx_int    _io_printf(const char _PTR_ , ... );
extern _mqx_int    _io_read(MQX_FILE_PTR, pointer, _mqx_int);
extern _mqx_int    _io_scanf(const char _PTR_ , ... );
extern _mqx_int    _io_sprintf(char _PTR_ , const char _PTR_ , ... );
extern _mqx_int    _io_snprintf(char _PTR_, _mqx_int, const char _PTR_, ...);
extern _mqx_int    _io_sscanf(char _PTR_ , const char _PTR_ , ... );
extern _mqx_int    _io_strcasecmp(const char *s1, const char *s2);
extern _mqx_int    _io_strncasecmp(const char *s1, const char *s2, uint_32 n);
extern double      _io_strtod(char _PTR_, char _PTR_ _PTR_);
extern _mqx_int    _io_vprintf(const char _PTR_, va_list);
extern _mqx_int    _io_vfprintf(MQX_FILE_PTR, const char _PTR_, va_list);
extern _mqx_int    _io_vsprintf(char _PTR_, const char _PTR_, va_list);
extern _mqx_int    _io_vsnprintf(char _PTR_, _mqx_int, const char _PTR_, va_list);
extern _mqx_int    _io_write(MQX_FILE_PTR, pointer, _mqx_int);
extern _mqx_int    _io_atoi(const char *str);


/* 
 * functions mapped out as macros in 'C' but provided for assembler functions
 */
extern _mqx_int    _io_getchar(void);
extern _mqx_int    _io_getline(char _PTR_, _mqx_int);
extern char _PTR_  _io_gets(char _PTR_);
extern _mqx_int    _io_putchar(_mqx_int);
extern _mqx_int    _io_puts(char _PTR_);
extern boolean     _io_status(void);
extern _mqx_int    _io_ungetc(_mqx_int);


extern boolean     _io_strtolower( char_ptr arg);
extern char_ptr    _io_find_filename(char_ptr arg);  
extern boolean     _io_validate_device(char_ptr arg);  
extern void        _io_create_prefixed_filename(char_ptr new_ptr, char_ptr in_ptr,  char_ptr dev_ptr);
extern int_32      _io_rel2abs(char_ptr result, char_ptr curdir, char_ptr inputpath, int_32 len, char_ptr cur_dev);
extern int_32      _io_get_dev_for_path(char_ptr out_dev, boolean * is_dev_in_path, int_32 dev_len, char_ptr input_path, char_ptr cur_dev);

extern MQX_FILE_PTR _io_get_first_valid_fs(void);
extern MQX_FILE_PTR _io_get_fs_by_name(char_ptr fs_name);
extern int_32      _io_get_fs_name(MQX_FILE_PTR fs_ptr, char_ptr fs_name, int_32 fs_name_len);
extern boolean     _io_is_fs_valid(MQX_FILE_PTR fs_ptr);

extern uint_32     _io_unregister_file_system(MQX_FILE_PTR fd_ptr);
extern uint_32     _io_register_file_system(MQX_FILE_PTR fd_ptr,char_ptr name_ptr);

#endif

/*==========================================================================*/

#ifdef __cplusplus
}
#endif

/* Include for I/O sub-system */
#include <io.h>

#endif
/* EOF */
