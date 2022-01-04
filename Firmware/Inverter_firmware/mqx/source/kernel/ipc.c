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
 * $FileName: ipc.c$
 * $Version : 3.8.1.0$
 * $Date    : Apr-13-2012$
 *
 * Comments:
 *
 *   This file contains functions of the IPC 
 *   (Inter Processor Communication) component.
 *
 *END************************************************************************/

#include <stdarg.h>
#include <string.h>
#include "mqx_inc.h"
#if MQX_USE_IPC
#include "message.h"
#include "msg_prv.h"
#include "fio.h"
#include "io.h"
#include "io_pcb.h"
#include "name.h"
#include "name_prv.h"
#include "event.h"
#include "event_prv.h"
#include "ipc.h"
#include "ipc_prv.h"
#include "ipc_pcb.h"
#include "ipc_pcbv.h"
#include "ipcrtprv.h"

/* A list of current IPC PCB drivers that have been installed */
QUEUE_STRUCT _ipc_pcb_drivers =
{   NULL, NULL, 0, 0};

/* Dummy router table for the IPC. */
const IPC_ROUTING_STRUCT _ipc_routing_table[] =
{
    {   0, 0, 0}
};

/*
 * Definition of the field sizes in the IPC message data.
 * So that the ENDIAN-ness can be swapped by _msg_swap_endian_data
 */
static const uchar _ipc_msg_type_def[] =
{
    sizeof(_mqx_uint),
    0};

static const uchar _ipc_msg_def[] =
{
    sizeof(_mqx_uint), sizeof(_task_id),
    /* Parameters... number of these must equal IPC_MAX_PARAMETERS */
    sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(_mqx_uint),
    sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(_mqx_uint),
    sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(_mqx_uint),
    sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(_mqx_uint),
    0};

static const uchar _ipc_mqx_msg_def[] =
{
    sizeof(_mqx_uint), sizeof(_task_id), sizeof(_task_id), sizeof(_mqx_uint),
    sizeof(_mqx_uint), sizeof(void (_CODE_PTR_)(_mqx_uint)),
    sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(char _PTR_),
    sizeof(_mqx_uint), sizeof(_mqx_uint), sizeof(_mqx_uint),
    0};

const IPC_PROTOCOL_INIT_STRUCT _ipc_init_table[] =
{
    {   NULL, NULL, NULL, 0}
};

static const IPC_INIT_STRUCT _default_ipc_init =
{
    _ipc_routing_table,
    _ipc_init_table
};

/*!
 * \brief Adds an IPC handler for the MQX component.
 * 
 * The IPC task calls the function when an IPC message for the specified MQX 
 * component is received. The IPC task calls the function once for each component.
 * \n The parameter component can be one of:
 * \li KERNEL_EDS_SERIAL
 * \li KERNEL_EVENTS
 * \li KERNEL_IPC
 * \li KERNEL_IPC_MSG_ROUTING
 * \li KERNEL_LOG
 * \li KERNEL_LWLOG
 * \li KERNEL_MESSAGES
 * \li KERNEL_MUTEXES
 * \li KERNEL_NAME_MANAGEMENT
 * \li KERNEL_PARTITIONS
 * \li KERNEL_SEMAPHORES
 * \li KERNEL_TIMER
 * \li KERNEL_WATCHDOG
 * 
 * \param[in] handler   Pointer to the function that MQX calls when it receives 
 * an IPC request for the component.
 * \param[in] component MQX component that the handler is for (see Description).
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (IPC component was not created.)
 * \return MQX_IPC_SERVICE_NOT_AVAILABLE (IPC server has not been started.)
 * 
 * \see _ipc_add_io_ipc_handler   
 */ 
_mqx_uint _ipc_add_ipc_handler
(
    IPC_HANDLER_FPTR handler,
    _mqx_uint        component
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    IPC_COMPONENT_STRUCT_PTR ipc_component_ptr;

    _GET_KERNEL_DATA(kernel_data);

    ipc_component_ptr = kernel_data->IPC_COMPONENT_PTR;

#if MQX_CHECK_ERRORS
    if (!ipc_component_ptr)
    {
        return MQX_IPC_SERVICE_NOT_AVAILABLE;
    } /* Endif */

    if (component > MAX_KERNEL_COMPONENTS)
    {
        return MQX_COMPONENT_DOES_NOT_EXIST;
    } /* Endif */
#endif

    ipc_component_ptr->IPC_COMPONENT_HANDLER[component] = handler;

    return MQX_OK;

} /* Endif */

/*!
 * \brief Add an IPC handler for the I/O component.
 * 
 * The IPC task calls the function when an IPC message for the specified I/O 
 * component is received. The IPC task calls the function once for each component.
 * \n The parameter component can be one of:
 * \li IO_CAN_COMPONENT
 * \li IO_EDS_COMPONENT
 * \li IO_HDLC_COMPONENT
 * \li IO_LAPB_COMPONENT
 * \li IO_LAPD_COMPONENT
 * \li IO_MFS_COMPONENT
 * \li IO_PPP_COMPONENT
 * \li IO_RTCS_COMPONENT
 * \li IO_SDLC_COMPONENT
 * \li IO_SNMP_COMPONENT
 * \li IO_SUBSYSTEM_COMPONENT
 * 
 * \param[in] handler   Pointer to the function that MQX calls when it receives 
 * an IPC request for the component.
 * \param[in] component I/O component that the handler is for (see Description).
 * 
 * \return MQX_OK
 * \return MQX_IPC_SERVICE_NOT_AVAILABLE (IPC server has not been started.)
 * 
 * \see _ipc_add_ipc_handler 
 */ 
_mqx_uint _ipc_add_io_ipc_handler
(
    IPC_HANDLER_FPTR handler,
    _mqx_uint        component
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    IPC_COMPONENT_STRUCT_PTR ipc_component_ptr;

    _GET_KERNEL_DATA(kernel_data);

    ipc_component_ptr = kernel_data->IPC_COMPONENT_PTR;

#if MQX_CHECK_ERRORS
    if (!ipc_component_ptr)
    {
        return MQX_IPC_SERVICE_NOT_AVAILABLE;
    } /* Endif */
#endif

    ipc_component_ptr->IPC_IO_COMPONENT_HANDLER[component] = handler;

    return MQX_OK;

} /* Endbody */

/*!
 * \brief Handles multi-processor event requests.
 * 
 * \param[in] imsg_ptr The incoming message.
 * 
 * \return MQX_OK  
 */ 
_mqx_uint _event_ipc_handler
(
    pointer imsg_ptr
)
{ /* Body */
    IPC_MESSAGE_STRUCT_PTR ipc_msg_ptr = imsg_ptr;
    pointer                event_ptr;
    _mqx_uint              error_code;
    _mqx_uint              result = 0;

    switch (IPC_GET_TYPE(ipc_msg_ptr->MESSAGE_TYPE))
    {
        case IPC_EVENT_OPEN:
        error_code = _event_open((char _PTR_)&ipc_msg_ptr->PARAMETERS[0],
                        &event_ptr);
        result = (_mqx_uint)event_ptr;
        break;
        case IPC_EVENT_SET:
        event_ptr = (pointer)ipc_msg_ptr->PARAMETERS[0];
        error_code = _event_set(event_ptr, ipc_msg_ptr->PARAMETERS[1]);
        break;
        default:
        error_code = MQX_IPC_INVALID_MESSAGE;
        break;
    } /* Endswitch */

    _ipc_send_internal(FALSE,
                    PROC_NUMBER_FROM_QID(ipc_msg_ptr->HEADER.SOURCE_QID),
                    KERNEL_MESSAGES, IPC_ACTIVATE,
                    3, result, ipc_msg_ptr->REQUESTOR_ID,
                    error_code);
    _msg_free(ipc_msg_ptr);

    return MQX_OK;

} /* Endbody */

/*!
 * \brief Installs the handler for IPC events.
 */ 
void _event_install_ipc_handler(void)
{ /* Body */

    _ipc_add_ipc_handler( _event_ipc_handler, KERNEL_EVENTS);

} /* Endbody */

/*!
 * \brief Initializes an IPC for a PCB driver.
 * 
 * The function is used in structure of type IPC_PROTOCOL_STRUCT to initialize the 
 * Inter-Processor Communications over a serial port using the standard 
 * ASYNC_SERIAL driver. 
 * \n The IPC_PROTOCOL_INIT_DATA field in IPC_PROTOCOL_INIT_STRUCT must point to 
 * a structure of type IPC_PCB_INIT_STRUCT.
 * 
 * \param[in] init_ptr Pointer to an IPC protocol initialization structure
 * (IPC_PROTOCOL_INIT_STRUCT).
 * \param[in] data_ptr Pointer to an IPC protocol information structure 
 * (IPC_PROTOCOL_INFO_STRUCT).
 * 
 * \return MQX_OK
 * \return IPC_PCB_OUTPUT_PCB_POOL_CREATE_FAILED
 * \return IPC_PCB_INVALID_QUEUE
 * \return IPC_PCB_PACKET_POOL_CREATE_FAILED
 * \return IPC_PCB_DEVICE_OPEN_FAILED
 * \return IPC_PCB_INPUT_PCB_POOL_CREATE_FAILED
 * \return        
 */ 
_mqx_uint _ipc_pcb_init(
    const IPC_PROTOCOL_INIT_STRUCT * init_ptr,
    pointer                          data_ptr)
{
    IPC_PROTOCOL_INFO_STRUCT_PTR info_ptr;
    IPC_PCB_INFO_STRUCT_PTR      pcb_info_ptr;
    IPC_PCB_INIT_STRUCT_PTR      pcb_init_ptr;
    _io_pcb_pool_id              pcb_pool;
    _queue_id                    qid;
    _pool_id                     msg_pool;
    _mqx_uint                    result = MQX_OK;
    _psp_code_addr               _ipc_pcb_function_location = (_psp_code_addr) _ipc_pcb_input_notification;

    info_ptr = (IPC_PROTOCOL_INFO_STRUCT_PTR) data_ptr;
    pcb_info_ptr = (IPC_PCB_INFO_STRUCT_PTR) _mem_alloc_system_zero((_mem_size) sizeof(IPC_PCB_INFO_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (pcb_info_ptr == NULL)
    {
        return (_task_get_error());
    }
#endif
    info_ptr->IPC_PROTOCOL_INFO_PTR = pcb_info_ptr;

    pcb_init_ptr = (IPC_PCB_INIT_STRUCT_PTR) init_ptr->IPC_PROTOCOL_INIT_DATA;

    info_ptr->IPC_TYPE = IPC_PCB_PROTOCOL;
    info_ptr->IPC_INIT_PTR = init_ptr;

    /* CREATE OUTPUT PCB POOL */
    pcb_pool = _io_pcb_create_pool(1, 0, pcb_init_ptr->OUT_PCBS_INITIAL, pcb_init_ptr->OUT_PCBS_TO_GROW,
                    pcb_init_ptr->OUT_PCBS_MAX, 0, NULL, 0, NULL);
#if MQX_CHECK_ERRORS
    if (pcb_pool == IO_PCB_NULL_POOL_ID)
    {
        result = IPC_PCB_OUTPUT_PCB_POOL_CREATE_FAILED;
    }
    else
    {
#endif
        pcb_info_ptr->PCB_OUTPUT_POOL = pcb_pool;
#if MQX_CHECK_ERRORS
    }
#endif

    /* Create output message queue */
    if (result == MQX_OK)
    {
        qid = _msgq_open_system((_queue_number) info_ptr->IPC_INIT_PTR->IPC_OUT_QUEUE, 0, _ipc_pcb_output_notification,
                        info_ptr);
#if MQX_CHECK_ERRORS
        if (qid == MSGQ_NULL_QUEUE_ID)
        {
            result = IPC_PCB_INVALID_QUEUE;
        }
        else
        {
#endif
            pcb_info_ptr->OUT_MSG_QID = qid;
#if MQX_CHECK_ERRORS
        }
#endif
    }

    if (result == MQX_OK)
    {
        /* Create input message pool */
        msg_pool = _msgpool_create(pcb_init_ptr->IN_MESSAGES_MAX_SIZE, pcb_init_ptr->IN_MESSAGES_TO_ALLOCATE,
                        pcb_init_ptr->IN_MESSAGES_TO_GROW, pcb_init_ptr->IN_MESSAGES_MAX_ALLOCATE);
#if MQX_CHECK_ERRORS
        if (msg_pool == MSGPOOL_NULL_POOL_ID)
        {
            result = (IPC_PCB_PACKET_POOL_CREATE_FAILED);
        }
        else
        {
#endif
            pcb_info_ptr->MSG_INPUT_POOL = msg_pool;
#if MQX_CHECK_ERRORS
        }
#endif
    }

    if (result == MQX_OK)
    {
        if (pcb_init_ptr->DEVICE_INSTALL != NULL)
        {
            result = (*pcb_init_ptr->DEVICE_INSTALL)(pcb_init_ptr->IO_PCB_DEVICE_NAME,
                            pcb_init_ptr->DEVICE_INSTALL_PARAMETER);
        }
    }

    if (result == MQX_OK)
    {
        /* Open the IO PCB device */
        pcb_info_ptr->FD = fopen(pcb_init_ptr->IO_PCB_DEVICE_NAME, (char_ptr) 0);
#if MQX_CHECK_ERRORS
        if (pcb_info_ptr->FD == NULL)
        {
            result = IPC_PCB_DEVICE_OPEN_FAILED;
        }
#endif
    }

    if (result == MQX_OK)
    {
        pcb_pool = _io_pcb_create_pool(1, 0, pcb_init_ptr->IN_MESSAGES_TO_ALLOCATE, pcb_init_ptr->IN_MESSAGES_TO_GROW,
                        pcb_init_ptr->IN_MESSAGES_MAX_ALLOCATE, _ipc_pcb_alloc, info_ptr, 0, NULL);
#if MQX_CHECK_ERRORS
        if (pcb_pool == IO_PCB_NULL_POOL_ID)
        {
            result = IPC_PCB_INPUT_PCB_POOL_CREATE_FAILED;
        }
        else
        {
#endif
            pcb_info_ptr->PCB_INPUT_POOL = pcb_pool;
#if MQX_CHECK_ERRORS
        }
#endif
    }

    if (result == MQX_OK)
    {
        result = ioctl(pcb_info_ptr->FD, IO_PCB_IOCTL_READ_CALLBACK_SET, &_ipc_pcb_function_location);
    }

    if (result == MQX_OK)
    {
        result = ioctl(pcb_info_ptr->FD, IO_PCB_IOCTL_SET_INPUT_POOL, (pointer) &pcb_info_ptr->PCB_INPUT_POOL);
    }

    if (result == MQX_OK)
    {
        result = ioctl(pcb_info_ptr->FD, IO_PCB_IOCTL_START, NULL);
    }

#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        if (pcb_info_ptr->FD)
        {
            fclose(pcb_info_ptr->FD);
        }
        if (pcb_info_ptr->PCB_INPUT_POOL)
        {
            _io_pcb_destroy_pool(pcb_info_ptr->PCB_INPUT_POOL);
        }
        if (pcb_info_ptr->MSG_INPUT_POOL)
        {
            _msgpool_destroy(pcb_info_ptr->MSG_INPUT_POOL);
        }
        if (pcb_info_ptr->OUT_MSG_QID)
        {
            _msgq_close(pcb_info_ptr->OUT_MSG_QID);
        }
        if (pcb_info_ptr->PCB_OUTPUT_POOL)
        {
            _io_pcb_destroy_pool(pcb_info_ptr->PCB_OUTPUT_POOL);
        }
        _mem_free(pcb_info_ptr);
    }
    else
    {
#endif
        if (_ipc_pcb_drivers.NEXT == NULL)
        {
            _queue_init(&_ipc_pcb_drivers, 0);
        }
        _queue_enqueue(&_ipc_pcb_drivers, &pcb_info_ptr->QUEUE);
#if MQX_CHECK_ERRORS
    }
#endif

    return (result);
}

/*!
 * \brief This function is called whenever a message is to be sent offboard.
 * 
 * It packs the message into a PCB and sends it off to an IO PCB device driver.   
 * The function is called by _msg_send (it is an activation function).
 * Note that the PCB free function is set to _ipc_pcb_free.
 * 
 * \param[in] data_ptr Pointer to an IPC protocol information structure 
 * (IPC_PROTOCOL_INFO_STRUCT). 
 */                     
void _ipc_pcb_output_notification
(
    pointer data_ptr
)
{ /* Body */
    IPC_PROTOCOL_INFO_STRUCT_PTR info_ptr;
    IPC_PCB_INFO_STRUCT_PTR      pcb_info_ptr;
    IO_PCB_STRUCT_PTR            pcb_ptr;
    IPC_MESSAGE_STRUCT_PTR       msg_ptr;
#if PSP_MEMORY_ADDRESSING_CAPABILITY > 8
    boolean                      must_unpack;
    boolean                      success;
    uint_32                      frag_size;
    uint_32                      result;
#endif

    info_ptr = (IPC_PROTOCOL_INFO_STRUCT_PTR) data_ptr;
    pcb_info_ptr = (IPC_PCB_INFO_STRUCT_PTR) info_ptr->IPC_PROTOCOL_INFO_PTR;

#if PSP_MEMORY_ADDRESSING_CAPABILITY > 8
    /* Determine if we must unpack the messages */
    result = ioctl(pcb_info_ptr->FD, IO_PCB_IOCTL_UNPACKED_ONLY, &must_unpack);
    if (result != MQX_OK)
    {
        must_unpack = FALSE;
    }
#endif

    while (_msgq_get_count(info_ptr->IPC_OUT_QID))
    {
        pcb_ptr = _io_pcb_alloc(pcb_info_ptr->PCB_OUTPUT_POOL, FALSE);
        if (pcb_ptr == NULL)
        {
            return;
        }
        msg_ptr = (IPC_MESSAGE_STRUCT_PTR) _msgq_poll(info_ptr->IPC_OUT_QID);

#if PSP_MEMORY_ADDRESSING_CAPABILITY > 8
        if (must_unpack)
        {
            success = _ipc_pcb_unpack_message(msg_ptr, pcb_ptr);
            _msg_free(msg_ptr);
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
            if (!success)
            {
                /* Toss the packet */
                _io_pcb_free_internal(pcb_ptr);
                return;
            }
#endif
        }
        else
        {
            pcb_ptr->FRAGMENTS[0].LENGTH = (uint_32)msg_ptr->HEADER.SIZE;
            pcb_ptr->FRAGMENTS[0].FRAGMENT = (uchar_ptr)msg_ptr;
            pcb_ptr->OWNER_PRIVATE_FLAGS = IPC_PCB_FRAG_IS_MSG;
        }
#else
        pcb_ptr->FRAGMENTS[0].LENGTH = (uint_32) msg_ptr->HEADER.SIZE;
        pcb_ptr->FRAGMENTS[0].FRAGMENT = (uchar_ptr) msg_ptr;
#endif
        pcb_ptr->FREE_PCB_FUNCTION_PTR = _ipc_pcb_free;

        _io_pcb_write(pcb_info_ptr->FD, pcb_ptr);

        pcb_info_ptr->OUTPUT_MESSAGE_COUNT++;

    }
}

/*!
 * \brief Frees a pcb sent via the ipc_pcb output task.
 *   
 * The message that is in the PCB must be freed.
 * 
 * \param[in] pcb_ptr The pcb that has finished. 
 */ 
_mqx_uint _ipc_pcb_free
(
    IO_PCB_STRUCT_PTR pcb_ptr
)
{
    _msg_free(pcb_ptr->FRAGMENTS[0].FRAGMENT);
    return (_io_pcb_free_internal(pcb_ptr));
}

/*!
 * \brief Obtains PCB from input PCB pool.
 * 
 * This function is called whenever a PCB is being allocated from the input PCB 
 * pool, by the input task. For example, when the input task is preparing to set 
 * up receive data, it would obtain a PCB from the input pool. At that point, 
 * this function will be called by the pcb alloc function. This function obtains 
 * a message and sets up the PCB to point to it.
 *  
 * \param[out] pcb_ptr  Pointer to the new PCB.
 * \param[in]  data_ptr Pointer to an IPC protocol information structure. 
 * (IPC_PROTOCOL_INFO_STRUCT).
 * 
 * \return Pointer to the PCB (or new PCB).
 * \return NULL (failure) 
 */ 

IO_PCB_STRUCT_PTR _ipc_pcb_alloc
(
    IO_PCB_STRUCT_PTR pcb_ptr,
    pointer           data_ptr
)
{
    IPC_PROTOCOL_INFO_STRUCT_PTR info_ptr;
    IPC_PCB_INFO_STRUCT_PTR      pcb_info_ptr;
    MESSAGE_HEADER_STRUCT_PTR    msg_ptr;

    info_ptr = (IPC_PROTOCOL_INFO_STRUCT_PTR) data_ptr;
    pcb_info_ptr = (IPC_PCB_INFO_STRUCT_PTR) info_ptr->IPC_PROTOCOL_INFO_PTR;

    msg_ptr = _msg_alloc(pcb_info_ptr->MSG_INPUT_POOL);
#if MQX_CHECK_ERRORS
    if (msg_ptr == NULL)
    {
        return (NULL);
    }
#endif
    pcb_ptr->FRAGMENTS[0].LENGTH = 0;
    pcb_ptr->FRAGMENTS[0].FRAGMENT = (uchar_ptr) msg_ptr;
    pcb_ptr->INSTANTIATOR_PRIVATE = info_ptr;

    return (pcb_ptr);
}

/*!
 * \brief This function is called by the underlying IO PCB device driver whenever 
 * an input message has been received.
 * 
 * \param[in] fd_ptr  The file descriptor used to set the notification function.
 * \param[in] pcb_ptr The pcb that has arrived. 
 */ 
void _ipc_pcb_input_notification
(
    MQX_FILE_PTR      fd_ptr,
    IO_PCB_STRUCT_PTR pcb_ptr
)
{
    KERNEL_DATA_STRUCT_PTR       kernel_data;
    IPC_PROTOCOL_INFO_STRUCT_PTR info_ptr;
    IPC_PCB_INFO_STRUCT_PTR      pcb_info_ptr;
    MESSAGE_HEADER_STRUCT_PTR    msg_ptr;

    _GET_KERNEL_DATA(kernel_data);

    info_ptr = (IPC_PROTOCOL_INFO_STRUCT_PTR) pcb_ptr->INSTANTIATOR_PRIVATE;
    pcb_info_ptr = (IPC_PCB_INFO_STRUCT_PTR) info_ptr->IPC_PROTOCOL_INFO_PTR;

    msg_ptr = (MESSAGE_HEADER_STRUCT_PTR) pcb_ptr->FRAGMENTS[0].FRAGMENT;
    if (MSG_MUST_CONVERT_HDR_ENDIAN(msg_ptr->CONTROL))
    {
        _msg_swap_endian_header(msg_ptr);
    }
    if (PROC_NUMBER_FROM_QID(msg_ptr->TARGET_QID) == 0)
    {
        /* This is a special message for this CPU */
        msg_ptr->TARGET_QID = BUILD_QID(kernel_data->INIT.PROCESSOR_NUMBER, QUEUE_FROM_QID(msg_ptr->TARGET_QID));
    }
    _msgq_send(msg_ptr);

    _io_pcb_free(pcb_ptr);

    pcb_info_ptr->INPUT_MESSAGE_COUNT++;
}

/*!
 * \brief Adds a route to the message routing table.
 * 
 * The IPC component must first be created.
 * 
 * \param[in] min_proc_number Minimum processor number in the range.
 * \param[in] max_proc_number Maximum processor number in the range.
 * \param[in] queue           Queue number of the IPC to use for processor numbers 
 * in the range.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (IPC component was not created.)
 * \return MQX_INVALID_PROCESSOR_NUMBER
 * \return MSGQ_INVALID_QUEUE_ID
 * \return IPC_ROUTE_EXISTS
 * \return MQX_OUT_OF_MEMORY
 * 
 * \see _ipc_msg_route_remove
 * \see _ipc_msg_processor_route_exists
 * \see IPC_ROUTING_STRUCT      
 */ 
_mqx_uint _ipc_msg_route_add
(
    _processor_number min_proc_number,
    _processor_number max_proc_number,
    _queue_number     queue
)
{
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR component_ptr;
    IPC_MSG_ROUTING_STRUCT_PTR           routing_ptr;
#if MQX_CHECK_ERRORS
    _processor_number                    i;
#endif

    _GET_KERNEL_DATA(kernel_data);
    component_ptr = (IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_IPC_MSG_ROUTING];
#if MQX_CHECK_ERRORS
    if (component_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    }
    if ((min_proc_number < 1) || (max_proc_number > MQX_MAX_PROCESSOR_NUMBER) || (min_proc_number > max_proc_number))
    {
        return (MQX_INVALID_PROCESSOR_NUMBER);
    }
    if ((queue == 0) || (queue >= MAX_QNUMBERS))
    {
        return (MSGQ_INVALID_QUEUE_ID);
    }
    for (i = min_proc_number; i <= max_proc_number; ++i)
    {
        if (_ipc_msg_processor_route_exists(i))
        {
            return (IPC_ROUTE_EXISTS);
        }
    }
#endif
    routing_ptr = (IPC_MSG_ROUTING_STRUCT_PTR) _mem_alloc_system(sizeof(IPC_MSG_ROUTING_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (routing_ptr == NULL)
    {
        return (MQX_OUT_OF_MEMORY);
    }
#endif
    routing_ptr->MIN_PROC_NUMBER = min_proc_number;
    routing_ptr->MAX_PROC_NUMBER = max_proc_number;
    routing_ptr->QUEUE = queue;

    _QUEUE_ENQUEUE(&component_ptr->ROUTING_LIST, &routing_ptr->LINK);

    return (MQX_OK);
}

/*!
 * \brief Removes a route from the message routing table.
 * 
 * The IPC component must first be installed.
 * 
 * \param[in] min_proc_number Minimum processor number in the range.
 * \param[in] max_proc_number Maximum processor number in the range.
 * \param[in] queue           Queue number of the IPC to remove.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (IPC component was not created.)
 * \return MQX_INVALID_PROCESSOR_NUMBER  
 * 
 * \see _ipc_msg_route_add     
 * \see _ipc_msg_processor_route_exists  
 */ 
_mqx_uint _ipc_msg_route_remove
(
    _processor_number min_proc_number,
    _processor_number max_proc_number,
    _queue_number     queue
)
{
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR component_ptr;
    IPC_MSG_ROUTING_STRUCT_PTR           routing_ptr;
    IPC_MSG_ROUTING_STRUCT_PTR           next_ptr;
    uint_16                              i;

    _GET_KERNEL_DATA(kernel_data);
    component_ptr = (IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR)(
                    (pointer) kernel_data->KERNEL_COMPONENTS[KERNEL_IPC_MSG_ROUTING]);
#if MQX_CHECK_ERRORS
    if (component_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    }
    if ((min_proc_number < 1) || (max_proc_number > MQX_MAX_PROCESSOR_NUMBER) || (min_proc_number > max_proc_number))
    {
        return (MQX_INVALID_PROCESSOR_NUMBER);
    }
#endif

    routing_ptr = (IPC_MSG_ROUTING_STRUCT_PTR)((pointer) component_ptr->ROUTING_LIST.NEXT);

    /* Scan list for specified processors */
    for (i = 0; i < component_ptr->ROUTING_LIST.SIZE; i++)
    {
        next_ptr = (IPC_MSG_ROUTING_STRUCT_PTR)((pointer) routing_ptr->LINK.NEXT);
        if ((routing_ptr->MIN_PROC_NUMBER == min_proc_number) && (routing_ptr->MAX_PROC_NUMBER == max_proc_number))
        {
#if MQX_CHECK_ERRORS
            if (routing_ptr->QUEUE == queue)
            {
#endif
                _QUEUE_REMOVE(&component_ptr->ROUTING_LIST, &routing_ptr->LINK);
                _mem_free(routing_ptr);
#if MQX_CHECK_ERRORS
            }
#endif
        }
        routing_ptr = next_ptr;
    }

    return (MQX_OK);
}

/*!
 * \brief Checks whether a route to the specified processor exists.
 * 
 * \param[in] proc_number Processor number to check for a route.
 * 
 * \return Pointer to the route (a route exists).
 * \return NULL (Route does not exist.)
 * 
 * \see _ipc_msg_route_add
 * \see _ipc_msg_route_remove  
 */ 
pointer _ipc_msg_processor_route_exists
(
    _processor_number proc_number
)
{
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR component_ptr;
    IPC_MSG_ROUTING_STRUCT_PTR           routing_ptr;
    uint_16                              i;

    _GET_KERNEL_DATA(kernel_data);
    component_ptr = (IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_IPC_MSG_ROUTING];
#if MQX_CHECK_ERRORS
    if (component_ptr == NULL)
    {
        return (NULL);
    }
#endif

    routing_ptr = (IPC_MSG_ROUTING_STRUCT_PTR)((pointer) component_ptr->ROUTING_LIST.NEXT);
    for (i = 0; i < component_ptr->ROUTING_LIST.SIZE; i++)
    {
        if ((proc_number >= routing_ptr->MIN_PROC_NUMBER) && (proc_number <= routing_ptr->MAX_PROC_NUMBER))
        {
            return ((pointer) routing_ptr);
        }
        routing_ptr = (IPC_MSG_ROUTING_STRUCT_PTR)((pointer) routing_ptr->LINK.NEXT);
    }

    /* Route not found */
    return (NULL);
}

/*!
 * \private
 * 
 * \brief This function is called from the _ipc_init to initialize the message
 * router.
 * 
 * \param[in] route_ptr Pointer to IPC routing structure (IPC_MSG_ROUTING_STRUCT_PTR).
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (IPC component was not created.)
 * \return MQX_INVALID_PROCESSOR_NUMBER
 * \return MSGQ_INVALID_QUEUE_ID
 * \return IPC_ROUTE_EXISTS
 * \return MQX_OUT_OF_MEMORY 
 */ 
_mqx_uint _ipc_msg_route_init_internal
(
    const IPC_ROUTING_STRUCT * route_ptr
)
{
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR component_ptr;

    _mqx_uint result;

    _GET_KERNEL_DATA(kernel_data);

    component_ptr = (IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR) _mem_alloc_system_zero(
                    (_mem_size) sizeof(IPC_MSG_ROUTING_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (component_ptr == NULL)
    {
        return (MQX_OUT_OF_MEMORY);
    }
#endif

    _QUEUE_INIT(&component_ptr->ROUTING_LIST, 0);

    /* Install the message routing handlers */
    kernel_data->KERNEL_COMPONENTS[KERNEL_IPC_MSG_ROUTING] = (pointer) component_ptr;

    while (route_ptr->QUEUE != 0)
    {
        result = _ipc_msg_route_add(route_ptr->MIN_PROC_NUMBER, route_ptr->MAX_PROC_NUMBER, route_ptr->QUEUE);
#if MQX_CHECK_ERRORS
        if (result != MQX_OK)
        {
            return (result);
        }
#endif
        route_ptr++;
    }

    component_ptr->MSG_ROUTER = _ipc_msg_route_internal;

    return (MQX_OK);
}

/*!
 * \private
 * 
 * \brief This function is called from _msg_send to route an inter-processor message.
 * 
 * \param[in] pnum     The processor to send the message to.
 * \param[in] message  The message to be sent.
 * \param[in] blocking Is this a blocking message.
 * 
 * \return TRUE or FALSE
 * 
 * \warning On failure, calls _task_set_error() to set the following task error code:
 * \li MSGQ_INVALID_QUEUE_ID 
 */ 
_mqx_uint _ipc_msg_route_internal
(
    _processor_number pnum,
    pointer           message,
    boolean           blocking
)
{
    KERNEL_DATA_STRUCT_PTR               kernel_data;
    IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR component_ptr;
    IPC_MSG_ROUTING_STRUCT_PTR           route_ptr;
    boolean                              result;
    _queue_number                        queue;

    _GET_KERNEL_DATA(kernel_data);
    component_ptr = (IPC_MSG_ROUTING_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_IPC_MSG_ROUTING];

    route_ptr = (IPC_MSG_ROUTING_STRUCT_PTR) _ipc_msg_processor_route_exists(pnum);
    if (!route_ptr)
    {
        _task_set_error(MSGQ_INVALID_QUEUE_ID);
        return (FALSE);
    }
    queue = route_ptr->QUEUE;

    result = _msgq_send_queue(message, BUILD_QID(kernel_data->INIT.PROCESSOR_NUMBER, queue));
    if (result && blocking && (!kernel_data->IN_ISR))
    {
        _int_disable();
        kernel_data->ACTIVE_PTR->STATE = SEND_BLOCKED;
        _task_block();
        _int_enable();
    }

    return (result);
}

/*!
 * \brief Task that initializes IPCs and processes remote service requests.
 * 
 * For applications to use the IPC component, the task must be either specified 
 * in the task template list as an autostart task or explicitly created.
 * \n The task installs the IPCs that are listed in the IPC initialization 
 * structure. Pointer to this initialization structure (IPC_INIT_STRUCT_PTR) is 
 * provided as the creation parameter, otherwise default IPC_INIT_STRUCT is used 
 * (_default_ipc_init). When the initialization is finished it waits for service
 * requests from remote processors.
 *  
 * \param[in] parameter Pointer to the IPC_INIT_STRUCT (task creation parameter).
 * 
 * \see IPC_INIT_STRUCT
 */ 
void _ipc_task
(
    uint_32 parameter
)
{
    KERNEL_DATA_STRUCT_PTR         kernel_data;
    IPC_COMPONENT_STRUCT_PTR       ipc_component_ptr;
    const IPC_INIT_STRUCT          * ipc_init_ptr = (IPC_INIT_STRUCT_PTR) parameter;
    IPC_MESSAGE_STRUCT_PTR         msg_ptr;
    IPC_MQX_MESSAGE_STRUCT_PTR     ipc_msg_ptr;
    const IPC_PROTOCOL_INIT_STRUCT * protocol_init_ptr;
    IPC_PROTOCOL_INFO_STRUCT_PTR   info_ptr;
    TD_STRUCT_PTR                  td_ptr;
    IPC_HANDLER_FPTR               handler;
    _queue_id                      queue;
    _mqx_uint                      result;
    _mqx_uint                      component;

    _GET_KERNEL_DATA(kernel_data);

    if (ipc_init_ptr == NULL)
    {
        ipc_init_ptr = &_default_ipc_init;
    }

    /* Install the message routing handlers */
    result = _ipc_msg_route_init_internal(ipc_init_ptr->ROUTING_LIST_PTR);
#if MQX_CHECK_ERRORS
    if (result != MQX_OK)
    {
        _task_set_error(result);
        _task_block();
    }
#endif

    kernel_data->MY_IPC_TD_PTR = kernel_data->ACTIVE_PTR;
    kernel_data->MY_IPC_ID = kernel_data->ACTIVE_PTR->TASK_ID;

    /* Create the IPC component structure */
    ipc_component_ptr = (IPC_COMPONENT_STRUCT_PTR) _mem_alloc_zero((_mem_size) sizeof(IPC_COMPONENT_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (ipc_component_ptr == NULL)
    {
        _task_block();
    }
#endif
    kernel_data->IPC_COMPONENT_PTR = ipc_component_ptr;

    /* Create a pool of messages for use by the ipc task. */
    kernel_data->IPC_NAMED_POOL = _msgpool_create(sizeof(IPC_MESSAGE_STRUCT), IPC_NUM_MESSAGES, IPC_GROW_MESSAGES,
                    IPC_LIMIT_MESSAGES);
#if MQX_CHECK_ERRORS || MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (kernel_data->IPC_NAMED_POOL == MSGPOOL_NULL_POOL_ID)
    {
        _task_block();
    }
#endif

    /* Open the ipc message queue */
    queue = _msgq_open(IPC_MESSAGE_QUEUE_NUMBER, 0);
#if MQX_CHECK_ERRORS
    if (queue == MSGQ_NULL_QUEUE_ID)
    {
        _task_block();
    }
#endif

    kernel_data->IPC = _ipc_send_internal;

    /* Initialize inter-processor communications protocols */
    protocol_init_ptr = ipc_init_ptr->PROTOCOL_LIST_PTR;
    while (protocol_init_ptr->IPC_PROTOCOL_INIT != NULL)
    {
#if MQX_CHECK_ERRORS
        if (((_queue_number) protocol_init_ptr->IPC_OUT_QUEUE) == 0
                        || ((_queue_number) protocol_init_ptr->IPC_OUT_QUEUE > MQX_MAX_PROCESSOR_NUMBER))
        {
            _task_block();
        }
#endif
        info_ptr = (IPC_PROTOCOL_INFO_STRUCT_PTR) _mem_alloc_zero((_mem_size) sizeof(IPC_PROTOCOL_INFO_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (info_ptr == NULL)
        {
            _task_block();
        }
#endif
        info_ptr->IPC_OUT_QUEUE = protocol_init_ptr->IPC_OUT_QUEUE;
        info_ptr->IPC_NAME = protocol_init_ptr->IPC_NAME;
        info_ptr->IPC_OUT_QID = _msgq_get_id(0, (_queue_number) protocol_init_ptr->IPC_OUT_QUEUE);
        info_ptr->IPC_INIT_PTR = protocol_init_ptr;

        (*protocol_init_ptr->IPC_PROTOCOL_INIT)(protocol_init_ptr, info_ptr);

        protocol_init_ptr++;
    }

    /*
     * The IPC Initialization is complete. Wait for messages to come in
     * and service them.
     */
    while (TRUE)
    {

        _task_set_error(MQX_OK);
        msg_ptr = (IPC_MESSAGE_STRUCT_PTR) _msgq_receive(MSGQ_ANY_QUEUE, 0L);
        if (MSG_MUST_CONVERT_DATA_ENDIAN(msg_ptr->HEADER.CONTROL))
        {
            _msg_swap_endian_data((uchar _PTR_) ((pointer) _ipc_msg_type_def), (MESSAGE_HEADER_STRUCT_PTR)(
                            (pointer) msg_ptr));
            if (msg_ptr->MESSAGE_TYPE & IPC_MQX_TYPE_MASK)
            {
                _mem_swap_endian((uchar _PTR_) _ipc_mqx_msg_def, &msg_ptr->NUMBER_OF_PARAMETERS);
            }
            else
            {
                _mem_swap_endian_len((uchar _PTR_) _ipc_msg_def, &msg_ptr->NUMBER_OF_PARAMETERS,
                                msg_ptr->NUMBER_OF_PARAMETERS + 2);
            }
        }

        if (msg_ptr->MESSAGE_TYPE & IPC_MQX_TYPE_MASK)
        {

            ipc_msg_ptr = (IPC_MQX_MESSAGE_STRUCT_PTR)((pointer) msg_ptr);
            switch (ipc_msg_ptr->MESSAGE_TYPE & IPC_MQX_TYPE_MASK)
            {

                case IPC_MQX_CREATE:
                    _KLOGE4(KLOG_task_create, 0, ipc_msg_ptr->TEMPLATE_INDEX,ipc_msg_ptr->CREATE_PARAMETER)
                    ;
                    if (ipc_msg_ptr->TEMPLATE_INDEX == 0)
                    {
                        td_ptr = _task_build_internal(0, (uint_32) &ipc_msg_ptr->TEMPLATE, NULL, 0, FALSE);
                    }
                    else
                    {
                        td_ptr = _task_build_internal(ipc_msg_ptr->TEMPLATE_INDEX,
                                        (uint_32) ipc_msg_ptr->CREATE_PARAMETER, NULL, 0, FALSE);
                    }

                    _KLOGX3(KLOG_task_create, MQX_NULL_TASK_ID, MQX_INVALID_PROCESSOR_NUMBER)
                    ;
                    if (td_ptr != NULL)
                    {
                        td_ptr->PARENT = ipc_msg_ptr->REQUESTOR_ID;
                        _task_ready(td_ptr);
                        _KLOGX3(KLOG_task_create, td_ptr->TASK_ID, MQX_OK);
                    }
                    else
                    {
                        _KLOGX3(KLOG_task_create, MQX_NULL_TASK_ID, _task_get_error());
                    }
                    result = (_mqx_uint) td_ptr->TASK_ID;
                    _ipc_send_internal(FALSE, PROC_NUMBER_FROM_TASKID(msg_ptr->REQUESTOR_ID), KERNEL_MESSAGES,
                                    IPC_ACTIVATE, (_mqx_uint) 3, result, msg_ptr->REQUESTOR_ID,
                                    kernel_data->ACTIVE_PTR->TASK_ERROR_CODE);
                    break;

                case IPC_MQX_ABORT:
                    result = (_mqx_uint) _task_abort(ipc_msg_ptr->VICTIM_ID);
                    break;
                case IPC_MQX_RESTART:
                    result = (_mqx_uint) _task_restart(ipc_msg_ptr->VICTIM_ID, NULL, TRUE);
                    break;
                case IPC_MQX_DESTROY:
                    result = (_mqx_uint) _task_destroy(ipc_msg_ptr->VICTIM_ID);
                    break;

                case IPC_MQX_ACTIVATE:
                    /* Start the specified task, setting the INFO
                     * to the returned result.
                     */
                    td_ptr = (TD_STRUCT_PTR) _task_get_td(ipc_msg_ptr->REQUESTOR_ID);
                    if (td_ptr != NULL)
                    {
                        if (td_ptr->STATE == SEND_BLOCKED)
                        {
                            td_ptr->INFO = ipc_msg_ptr->VICTIM_ID;
                            _task_set_error_td_internal(td_ptr, ipc_msg_ptr->CREATE_PARAMETER);
                            _task_ready(td_ptr);
                        }
                    }
                    result = MQX_OK;
                    break;

            }

            _msg_free((pointer) msg_ptr);

        }
        else
        {

            component = IPC_GET_COMPONENT(msg_ptr->MESSAGE_TYPE);

            if (component < MAX_KERNEL_COMPONENTS)
            {
                handler = ipc_component_ptr->IPC_COMPONENT_HANDLER[component];
#if MQX_CHECK_ERRORS
                if (handler)
                {
#endif
                    _task_set_error(MQX_OK);
                    result = (*handler)((pointer) msg_ptr);
#if MQX_CHECK_ERRORS
                }
                else
                {
                    result = MQX_IPC_SERVICE_NOT_AVAILABLE;
                }
#endif
            }
            else if (component < (MAX_KERNEL_COMPONENTS + MAX_IO_COMPONENTS))
            {
                handler = ipc_component_ptr->IPC_IO_COMPONENT_HANDLER[component - MAX_KERNEL_COMPONENTS];
#if MQX_CHECK_ERRORS
                if (handler)
                {
#endif
                    _task_set_error(MQX_OK);
                    result = (*handler)((pointer) msg_ptr);
#if MQX_CHECK_ERRORS
                }
                else
                {
                    result = MQX_IPC_SERVICE_NOT_AVAILABLE;
                    _msg_free((pointer) msg_ptr);
                }
#endif
            }
            else
            {
                result = MQX_IPC_INVALID_MESSAGE;
                _msg_free((pointer) msg_ptr);
            }
        }
    }
}

/*!
 * \private
 * 
 * \brief Creates an IPC message and sends it.
 * 
 * \param[in] blocking         Specifies whether or not to block current task.
 * \param[in] processor_number The processor to send to.
 * \param[in] component        The component to receive the message.
 * \param[in] type             The type of component message.
 * \param[in] num_parameters   The number of parameters in the message.
 * 
 * \return MQX_OK
 * \return Task error code from ....  
 */ 
_mqx_uint _ipc_send_internal
(
    boolean           blocking,
    _processor_number processor_number,
    _mqx_uint         component,
    _mqx_uint         type,
    _mqx_uint         num_parameters,
    ...)
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    IPC_MESSAGE_STRUCT_PTR     msg_ptr;
    IPC_MQX_MESSAGE_STRUCT_PTR ipc_msg_ptr;
    va_list                    ap;
    _mqx_uint                  i;

    _GET_KERNEL_DATA(kernel_data);

    kernel_data->ACTIVE_PTR->TASK_ERROR_CODE = MQX_OK;

    msg_ptr = (IPC_MESSAGE_STRUCT_PTR) _msg_alloc((_pool_id) kernel_data->IPC_NAMED_POOL);
    while (msg_ptr == NULL)
    {
        _time_delay(kernel_data->KERNEL_ALARM_RESOLUTION * 2);
        msg_ptr = (IPC_MESSAGE_STRUCT_PTR) _msg_alloc((_pool_id) kernel_data->IPC_NAMED_POOL);
    }

    msg_ptr->HEADER.TARGET_QID = BUILD_QID(processor_number, IPC_MESSAGE_QUEUE_NUMBER);
    msg_ptr->HEADER.SOURCE_QID = BUILD_QID((_processor_number) kernel_data->INIT.PROCESSOR_NUMBER,
                    IPC_MESSAGE_QUEUE_NUMBER);

    va_start(ap, num_parameters);

    if (component == KERNEL_MESSAGES)
    {
        ipc_msg_ptr = (IPC_MQX_MESSAGE_STRUCT_PTR)((pointer) msg_ptr);
        ipc_msg_ptr->REQUESTOR_ID = kernel_data->ACTIVE_PTR->TASK_ID;
        switch (type)
        {
            case IPC_TASK_CREATE:
            case IPC_TASK_CREATE_WITH_TEMPLATE:
                ipc_msg_ptr->MESSAGE_TYPE = IPC_MQX_CREATE;
                if (type == IPC_TASK_CREATE_WITH_TEMPLATE)
                {
                    ipc_msg_ptr->TEMPLATE_INDEX = 0;
                    ipc_msg_ptr->TEMPLATE.TASK_TEMPLATE_INDEX = va_arg(ap, uint_32);
                    ipc_msg_ptr->TEMPLATE.TASK_ADDRESS = (TASK_FPTR)va_arg(ap, _psp_code_addr);
                    ipc_msg_ptr->TEMPLATE.TASK_STACKSIZE = va_arg(ap, _mem_size);
                    ipc_msg_ptr->TEMPLATE.TASK_PRIORITY = va_arg(ap, _mqx_uint);
                    ipc_msg_ptr->TEMPLATE.TASK_NAME = va_arg(ap, char_ptr);
                    ipc_msg_ptr->TEMPLATE.TASK_ATTRIBUTES = va_arg(ap, _mqx_uint);
                    ipc_msg_ptr->TEMPLATE.CREATION_PARAMETER = va_arg(ap, uint_32);
                    ipc_msg_ptr->CREATE_PARAMETER = ipc_msg_ptr->TEMPLATE.CREATION_PARAMETER;
                    ipc_msg_ptr->TEMPLATE.DEFAULT_TIME_SLICE = va_arg(ap, uint_32);
                    ipc_msg_ptr->HEADER.SIZE = sizeof(IPC_MQX_MESSAGE_STRUCT);
                }
                else
                {
                    i = va_arg(ap, _mqx_uint); /* Toss processor number */
                    ipc_msg_ptr->TEMPLATE_INDEX = va_arg(ap, _mqx_uint);
                    ipc_msg_ptr->CREATE_PARAMETER = (uint_32) va_arg(ap, uint_32);
                    ipc_msg_ptr->HEADER.SIZE = sizeof(IPC_MQX_MESSAGE_STRUCT) - sizeof(TASK_TEMPLATE_STRUCT);
                }
                break;
            case IPC_TASK_DESTROY:
                ipc_msg_ptr->MESSAGE_TYPE = IPC_MQX_DESTROY;
                ipc_msg_ptr->VICTIM_ID = va_arg(ap, _mqx_uint);
                ipc_msg_ptr->HEADER.SIZE = sizeof(IPC_MQX_MESSAGE_STRUCT) - sizeof(TASK_TEMPLATE_STRUCT);
                break;
            case IPC_TASK_ABORT:
                ipc_msg_ptr->MESSAGE_TYPE = IPC_MQX_ABORT;
                ipc_msg_ptr->VICTIM_ID = va_arg(ap, _mqx_uint);
                ipc_msg_ptr->HEADER.SIZE = sizeof(IPC_MQX_MESSAGE_STRUCT) - sizeof(TASK_TEMPLATE_STRUCT);
                break;
            case IPC_TASK_RESTART:
                ipc_msg_ptr->MESSAGE_TYPE = IPC_MQX_RESTART;
                ipc_msg_ptr->VICTIM_ID = va_arg(ap, _mqx_uint);
                ipc_msg_ptr->HEADER.SIZE = sizeof(IPC_MQX_MESSAGE_STRUCT) - sizeof(TASK_TEMPLATE_STRUCT);
                break;
            case IPC_ACTIVATE:
                ipc_msg_ptr->MESSAGE_TYPE = IPC_MQX_ACTIVATE;
                ipc_msg_ptr->VICTIM_ID = va_arg(ap, _mqx_uint);
                ipc_msg_ptr->REQUESTOR_ID = va_arg(ap, _mqx_uint);
                ipc_msg_ptr->CREATE_PARAMETER = (uint_32) va_arg(ap, uint_32);
                ipc_msg_ptr->HEADER.SIZE = sizeof(IPC_MQX_MESSAGE_STRUCT) - sizeof(TASK_TEMPLATE_STRUCT);
                break;
        }
    }
    else
    {
        msg_ptr->MESSAGE_TYPE = IPC_SET_COMPONENT(0, component);
        msg_ptr->MESSAGE_TYPE = IPC_SET_TYPE(msg_ptr->MESSAGE_TYPE, type);
        if (!blocking)
        {
            msg_ptr->MESSAGE_TYPE = IPC_SET_NON_BLOCKING(msg_ptr->MESSAGE_TYPE, 1);
        }

        msg_ptr->REQUESTOR_ID = kernel_data->ACTIVE_PTR->TASK_ID;

        if (num_parameters > IPC_MAX_PARAMETERS)
        {
            num_parameters = IPC_MAX_PARAMETERS;
        }

        if (type & IPC_STRING_PARAMETER)
        {
            --num_parameters;
            msg_ptr->HEADER.SIZE = (_msg_size) sizeof(IPC_MESSAGE_STRUCT);
        }
        else
        {
            msg_ptr->HEADER.SIZE = (_msg_size)(sizeof(IPC_MESSAGE_STRUCT) - (IPC_MAX_PARAMETERS * sizeof(uint_32))
                            + (num_parameters * sizeof(uint_32)));
        }

        msg_ptr->NUMBER_OF_PARAMETERS = (uint_32) num_parameters;

        for (i = 0; (i < num_parameters) && (i < IPC_MAX_PARAMETERS); ++i)
        {
            msg_ptr->PARAMETERS[i] = (uint_32) (va_arg(ap, uint_32));
        }

        if (type & IPC_STRING_PARAMETER)
        {
            char_ptr to_ptr = (char_ptr) &msg_ptr->PARAMETERS[i];
            char_ptr from_ptr = (char_ptr) va_arg(ap, pointer);
            uint_32 size = (IPC_MAX_PARAMETERS - i) * sizeof(uint_32);
            strncpy((pointer) to_ptr, (pointer) from_ptr, size);
            to_ptr[size - 1] = '\0';
        }
    }

    va_end(ap);

    if (blocking)
    {
        if (_msgq_send_blocked_internal(msg_ptr))
        {
            return MQX_OK;
        }
    }
    else
    {
        if (_msgq_send(msg_ptr))
        {
            return MQX_OK;
        }
    }

    return kernel_data->ACTIVE_PTR->TASK_ERROR_CODE;
}
#endif /* MQX_USE_IPC */

/* EOF */
