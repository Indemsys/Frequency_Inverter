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
* $FileName: sai_int_prv.h$
* $Version : 3.8.2.0$
* $Date    : Aug-20-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*END************************************************************************/

#ifndef __SAI_INT_PRV_H__
#define __SAI_INT_PRV_H__

/* 
** TYPE DECLARATION 
*/

typedef struct io_sai_device_struct
{

    /* The I/O init function */
    _mqx_int (_CODE_PTR_ DEV_INIT)(pointer);

    /* The I/O deinit function */
    _mqx_int (_CODE_PTR_ DEV_DEINIT)(pointer, pointer);

    /* [IN] The READ function */
    _mqx_int (_CODE_PTR_ DEV_READ)(pointer, char_ptr, uint_32);

    /* [IN] The WRITE function */
    _mqx_int (_CODE_PTR_ DEV_WRITE)(pointer, char_ptr, uint_32);

    /* The ioctl function, (change bauds etc) */
    _mqx_int (_CODE_PTR_ DEV_IOCTL)(pointer, _mqx_int, pointer);

    /* The I/O channel initialization data */
    pointer DEV_INIT_DATA_PTR;

    /* Device specific information */
    pointer DEV_INFO_PTR;

    /* Open count for number of accessing file descriptor for reading */
    _mqx_int READ_COUNT;
    
    /* Open count for number of accessing file descriptor for writing */
    _mqx_int WRITE_COUNT;
    
    /* Open flags for this channel */
    _mqx_int FLAGS;
    
    LWSEM_STRUCT LWSEM;

} IO_SAI_DEVICE_STRUCT, _PTR_ IO_SAI_DEVICE_STRUCT_PTR;

/*
** FUNCTION PROTOTYPE
*/

#ifdef __cplusplus
extern "C" 
{
#endif

    _mqx_int _io_sai_int_open(MQX_FILE_PTR fd_ptr, char_ptr open_name_ptr, char_ptr flags);
    _mqx_int _io_sai_int_close(MQX_FILE_PTR fd_ptr);
    _mqx_int _io_sai_int_read(MQX_FILE_PTR fd_ptr, char_ptr data_ptr, _mqx_int n);
    _mqx_int _io_sai_int_write(MQX_FILE_PTR fd_ptr, char_ptr data_ptr, _mqx_int n);
    _mqx_int _io_sai_int_ioctl(MQX_FILE_PTR fd_ptr, _mqx_uint cmd, pointer input_param_ptr);
    _mqx_int _io_sai_int_uninstall(IO_DEVICE_STRUCT_PTR io_dev_ptr);

#ifdef __cplusplus
}
#endif

#endif /* __SAI_INT_PRV_H__ */

/* EOF */
