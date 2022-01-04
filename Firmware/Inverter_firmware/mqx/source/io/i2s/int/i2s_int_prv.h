#ifndef _i2s_int_prv_h_
#define _i2s_int_prv_h_ 1

/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: i2s_int_prv.h$
* $Version : 3.8.1.0$
* $Date    : Sep-16-2011$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the I2S I/O functions.
*
*END************************************************************************/

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/


/*---------------------------------------------------------------------
**
** IO I2S DEVICE STRUCT
**
** This structure used to store information about an I2S interrupt io device
** for the IO device table
*/
typedef struct io_i2s_device_struct
{

    /* The I/O init function */
    _mqx_uint (_CODE_PTR_ DEV_INIT)(pointer, char_ptr);

    /* The I/O deinit function */
    _mqx_uint (_CODE_PTR_ DEV_DEINIT)(pointer, pointer);

    /* [IN] The READ function */
    _mqx_int  (_CODE_PTR_ DEV_READ)(pointer, char_ptr, _mqx_int);

    /* [IN] The WRITE function */
    _mqx_int  (_CODE_PTR_ DEV_WRITE)(pointer, char_ptr, _mqx_int);

    /* The ioctl function, (change bauds etc) */
    _mqx_int  (_CODE_PTR_ DEV_IOCTL)(pointer, _mqx_uint, _mqx_uint_ptr);

    /* The I/O channel initialization data */
    pointer    DEV_INIT_DATA_PTR;

    /* Device specific information */
    pointer    DEV_INFO_PTR;

    /* Open count for number of accessing file descriptors */
    _mqx_uint  COUNT;

    /* Open flags for this channel */
    _mqx_uint  FLAGS;

} IO_I2S_DEVICE_STRUCT, _PTR_ IO_I2S_DEVICE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

    /* I/O prototypes */
    extern _mqx_int _io_i2s_open(MQX_FILE_PTR fd_ptr, char_ptr open_name_ptr, char_ptr flags);
    extern _mqx_int _io_i2s_close(MQX_FILE_PTR fd_ptr);
    extern _mqx_int _io_i2s_read(MQX_FILE_PTR fd_ptr, char_ptr data_ptr, _mqx_int n);
    extern _mqx_int _io_i2s_write(MQX_FILE_PTR fd_ptr, char_ptr data_ptr, _mqx_int n);
    extern _mqx_int _io_i2s_ioctl(MQX_FILE_PTR fd_ptr, _mqx_uint cmd, pointer input_param_ptr);
    extern _mqx_int _io_i2s_uninstall(IO_DEVICE_STRUCT_PTR io_dev_ptr);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
