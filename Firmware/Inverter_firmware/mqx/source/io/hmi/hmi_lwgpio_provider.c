/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: hmi_lwgpio_provider.c$
* $Version : 3.8.2.0$
* $Date    : Aug-14-2012$
*
* Comments:
*
*   This contains functions for lwgpio provider.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include "hmi_lwgpio_provider.h"
#include "btnled.h"

/* static functions prototypes */
static void hmi_lwgpio_poll(HMI_PROVIDER_STRUCT_PTR);
static boolean hmi_lwgpio_set_value(HMI_PROVIDER_STRUCT_PTR, uint_32 ,uint_32);
static boolean hmi_lwgpio_get_value(HMI_PROVIDER_STRUCT_PTR, uint_32 ,uint_32_ptr);
static void hmi_lwgpio_install_interrupt(HMI_PROVIDER_STRUCT_PTR);
static void hmi_lwgpio_uninstall_interrupt(HMI_PROVIDER_STRUCT_PTR);
static void hmi_lwgpio_add(const HMI_LWGPIO_INIT_STRUCT _PTR_, HMI_LWGPIO_STRUCT_PTR);
static void hmi_lwgpio_attach_client(HMI_PROVIDER_STRUCT_PTR,HMI_CLIENT_STRUCT_PTR);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_lwgpio_poll
* Returned Value   : void
* Comments         :
*   This function polls all LWGPIO pins and checks if actual state
*   has changed.
*
*END*--------------------------------------------------------------------*/
static void hmi_lwgpio_poll(HMI_PROVIDER_STRUCT_PTR provider_struct)
{
    _mqx_int         i;
    LWGPIO_VALUE     actual_state;
    HMI_LWGPIO_CONTEXT_STRUCT_PTR context_struct = (HMI_LWGPIO_CONTEXT_STRUCT_PTR)provider_struct->CONTEXT_PTR;
    uint_8           flag;

    for(i=0; i<context_struct->BTNLED_COUNT; i++)
    {
        /* Pull only inputs without interrupt flag */
        if(!HMI_LWGPIO_GET_DIRECTION(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG) && \
           !HMI_LWGPIO_GET_INT(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG))
        {
            /* gets actual state of the pin */
            actual_state = lwgpio_get_value(&context_struct->BTNLED_TABLE[i].HANDLE);
            if (actual_state != context_struct->BTNLED_TABLE[i].LAST_STATE)
            {
                 /* saves actual state */
                context_struct->BTNLED_TABLE[i].LAST_STATE = actual_state;
                /* check if actual state is equal to active state */
                if (actual_state == HMI_LWGPIO_GET_ACTIVE_STATE(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG))
                {
                    flag = HMI_VALUE_PUSH;
                }
                else
                {
                    flag = HMI_VALUE_RELEASE;
                }
                context_struct->CLIENT_HANDLE->ON_CHANGE(context_struct->CLIENT_HANDLE, context_struct->BTNLED_TABLE[i].INIT_STRUCT.UID, flag);
            }
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_lwgpio_set_value
* Returned Value   : boolean
* Comments         :
*   This function sets value to LWGPIO pin.
*
*END*--------------------------------------------------------------------*/
static boolean hmi_lwgpio_set_value(HMI_PROVIDER_STRUCT_PTR provider_struct, uint_32 uid, uint_32 value)
{
    _mqx_int                        i;
    boolean                         result = FALSE;
    HMI_LWGPIO_CONTEXT_STRUCT_PTR   context_struct = (HMI_LWGPIO_CONTEXT_STRUCT_PTR) provider_struct->CONTEXT_PTR;

    for(i=0; i<context_struct->BTNLED_COUNT; i++)
    {
        if(context_struct->BTNLED_TABLE[i].INIT_STRUCT.UID == uid)
        {
            result = TRUE;
            if(value == HMI_VALUE_ON)
            {
                lwgpio_set_value(&(context_struct->BTNLED_TABLE[i].HANDLE), \
                    (LWGPIO_VALUE)HMI_LWGPIO_GET_ACTIVE_STATE(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG));
            }
            else if(value == HMI_VALUE_OFF)
            {
                lwgpio_set_value(&context_struct->BTNLED_TABLE[i].HANDLE, \
                    (LWGPIO_VALUE)!HMI_LWGPIO_GET_ACTIVE_STATE(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG));
            }
        context_struct->BTNLED_TABLE[i].LAST_STATE = (LWGPIO_VALUE)value;
        }
    }
    return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_lwgpio_get_value
* Returned Value   : boolean
* Comments         :
*   This function gets value from LWGPIO pin and returns TRUE if the pin
*   exists.
*
*END*--------------------------------------------------------------------*/
static boolean hmi_lwgpio_get_value(HMI_PROVIDER_STRUCT_PTR provider_struct, uint_32 uid, uint_32_ptr value)
{
    _mqx_int    i;
    uint_32     max_value = 0;
    boolean     result = FALSE;
    HMI_LWGPIO_CONTEXT_STRUCT_PTR context_struct = (HMI_LWGPIO_CONTEXT_STRUCT_PTR)provider_struct->CONTEXT_PTR;

    for(i=0; i<context_struct->BTNLED_COUNT; i++)
    {
        if (context_struct->BTNLED_TABLE[i].INIT_STRUCT.UID == uid)
        {
            *value = 0;
        }
    }
    for(i=0; i<context_struct->BTNLED_COUNT; i++)
    {
        if (context_struct->BTNLED_TABLE[i].INIT_STRUCT.UID == uid)
        {
            result = TRUE;
            max_value = lwgpio_get_value(&context_struct->BTNLED_TABLE[i].HANDLE);
            if (max_value == HMI_LWGPIO_GET_ACTIVE_STATE(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG))
            {
                max_value = HMI_VALUE_ON;
            }
            else
            {
                max_value = HMI_VALUE_OFF;
            }
            if(*value < max_value)
            {
                *value = max_value;
            }
        }
    }
    return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_lwgpio_isr
* Returned Value   : void
* Comments         :
*   This function is LWGPIO interrupt service routine.
*
*
*END*--------------------------------------------------------------------*/
static void hmi_lwgpio_isr(pointer provider_struct)
{
    _mqx_int        i=0;
    LWGPIO_VALUE    actual_state;
    uint_8          flag;
    
    HMI_LWGPIO_CONTEXT_STRUCT_PTR context_struct = (HMI_LWGPIO_CONTEXT_STRUCT_PTR)((HMI_PROVIDER_STRUCT_PTR)provider_struct)->CONTEXT_PTR;

    for(i=0; i<context_struct->BTNLED_COUNT; i++)
    {
        if ((((HMI_PROVIDER_STRUCT_PTR)provider_struct)->INSTALL_INTERRUPT_PTR != NULL) && lwgpio_int_get_flag(&context_struct->BTNLED_TABLE[i].HANDLE))
        {
            lwgpio_int_clear_flag(&context_struct->BTNLED_TABLE[i].HANDLE);
            actual_state = lwgpio_get_value(&context_struct->BTNLED_TABLE[i].HANDLE);
            /* check if actual state is equal to active state */
            if (actual_state == HMI_LWGPIO_GET_ACTIVE_STATE(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG))    
            {
                flag = HMI_VALUE_PUSH;
            }
            else
            {
                flag = HMI_VALUE_RELEASE;
            }
            context_struct->CLIENT_HANDLE->ON_CHANGE(context_struct->CLIENT_HANDLE, context_struct->BTNLED_TABLE[i].INIT_STRUCT.UID, flag);
        }
    }
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_lwgpio_attach_client
* Returned Value   : void
* Comments         :
*    This function attaches a client to a provider.
*
*END*--------------------------------------------------------------------*/
static void hmi_lwgpio_attach_client(HMI_PROVIDER_STRUCT_PTR provider_struct, HMI_CLIENT_STRUCT_PTR client_struct)
{
    HMI_LWGPIO_CONTEXT_STRUCT_PTR context = (HMI_LWGPIO_CONTEXT_STRUCT_PTR)provider_struct->CONTEXT_PTR;
    
    /* attach client to provider */
    context->CLIENT_HANDLE = client_struct;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_lwgpio_install_interrupt
* Returned Value   : void
* Comments         :
*   This function install interrupt.
*
*END*--------------------------------------------------------------------*/
static void hmi_lwgpio_install_interrupt(HMI_PROVIDER_STRUCT_PTR provider_struct)
{
    HMI_LWGPIO_CONTEXT_STRUCT_PTR context_struct = (HMI_LWGPIO_CONTEXT_STRUCT_PTR) provider_struct->CONTEXT_PTR;
    _mqx_int    i;

    /* install interrupt if there's any requested */
    for (i=0; i<context_struct->BTNLED_COUNT; i++)
    {
        if (!HMI_LWGPIO_GET_DIRECTION(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG) && \
                HMI_LWGPIO_GET_INT(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG))
        {
            lwgpio_int_init(&context_struct->BTNLED_TABLE[i].HANDLE,LWGPIO_INT_MODE_RISING | LWGPIO_INT_MODE_FALLING);     /* falling,raising mode = 3 */
            context_struct->BTNLED_TABLE[i].INIT_ISR = _int_install_isr(lwgpio_int_get_vector(&context_struct->BTNLED_TABLE[i].HANDLE), hmi_lwgpio_isr, provider_struct);
            _bsp_int_init(lwgpio_int_get_vector(&context_struct->BTNLED_TABLE[i].HANDLE), 3, 0, TRUE);
            lwgpio_int_enable(&context_struct->BTNLED_TABLE[i].HANDLE, TRUE);
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_lwgpio_uninstall_interrupt
* Returned Value   : void
* Comments         :
*   This function uninstall interrupt.
*
*END*--------------------------------------------------------------------*/
static void  hmi_lwgpio_uninstall_interrupt(HMI_PROVIDER_STRUCT_PTR provider_struct)
{
    _mqx_int    i;
    HMI_LWGPIO_CONTEXT_STRUCT_PTR context_struct = (HMI_LWGPIO_CONTEXT_STRUCT_PTR) provider_struct->CONTEXT_PTR;


    /* remove interrupt if there's any */
    for (i=0; i<context_struct->BTNLED_COUNT; i++)
    {
        if (!HMI_LWGPIO_GET_DIRECTION(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG) && \
                HMI_LWGPIO_GET_INT(context_struct->BTNLED_TABLE[i].INIT_STRUCT.FLAG))
        {
            _int_install_isr(lwgpio_int_get_vector(&context_struct->BTNLED_TABLE[i].HANDLE), context_struct->BTNLED_TABLE[i].INIT_ISR, NULL);
            /* disable bsp int */
            _bsp_int_init(lwgpio_int_get_vector(&context_struct->BTNLED_TABLE[i].HANDLE), 3, 0, FALSE);     
            /* disable LWGPIO int */
            lwgpio_int_enable(&context_struct->BTNLED_TABLE[i].HANDLE, FALSE);
        }
    }
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_lwgpio_add
* Returned Value   : void
* Comments         :
*   This function adds BTN / LED.
*
*END*--------------------------------------------------------------------*/
static void hmi_lwgpio_add(const HMI_LWGPIO_INIT_STRUCT _PTR_ init_struct, HMI_LWGPIO_STRUCT_PTR btnled_struct)
{    
    /* save data from init table to context struct */
    btnled_struct->INIT_STRUCT.PID = init_struct->PID;
    btnled_struct->INIT_STRUCT.UID = init_struct->UID;
    btnled_struct->INIT_STRUCT.FLAG = init_struct->FLAG;
    btnled_struct->INIT_STRUCT.FUNC = init_struct->FUNC;
    lwgpio_init(&btnled_struct->HANDLE, \
                btnled_struct->INIT_STRUCT.PID, \
                (LWGPIO_DIR)HMI_LWGPIO_GET_DIRECTION(btnled_struct->INIT_STRUCT.FLAG), \
                (LWGPIO_VALUE)HMI_LWGPIO_GET_ACTIVE_STATE(btnled_struct->INIT_STRUCT.FLAG));

    if(!HMI_LWGPIO_GET_DIRECTION(init_struct->FLAG) && HMI_LWGPIO_GET_PULL_UP(init_struct->FLAG))
    {
        lwgpio_set_attribute(&btnled_struct->HANDLE, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    }
    else if(!HMI_LWGPIO_GET_DIRECTION(init_struct->FLAG) && HMI_LWGPIO_GET_PULL_DOWN(init_struct->FLAG))
    {
        lwgpio_set_attribute(&btnled_struct->HANDLE, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
    }

    lwgpio_set_functionality(&btnled_struct->HANDLE, btnled_struct->INIT_STRUCT.FUNC);
    _time_delay(5);        /* needs time to enable pull up */
    btnled_struct->LAST_STATE = lwgpio_get_value(&btnled_struct->HANDLE);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _hmi_lwgpio_provider_init
* Returned Value   : HMI_PROVIDER_STRUCT_PTR
* Comments         :
*   This function initializes LWGPIO provider.
*
*END*--------------------------------------------------------------------*/
HMI_PROVIDER_STRUCT_PTR hmi_lwgpio_provider_init(const HMI_LWGPIO_INIT_STRUCT _PTR_ init_table)
{
    _mqx_uint                       i = 0;
    HMI_PROVIDER_STRUCT_PTR         provider_struct;
    HMI_LWGPIO_CONTEXT_STRUCT_PTR   context_struct;

    provider_struct = _mem_alloc_system(sizeof(HMI_PROVIDER_STRUCT));
    if(provider_struct == NULL)
    {
        return NULL;
    }

    context_struct = _mem_alloc_system(sizeof(HMI_LWGPIO_CONTEXT_STRUCT));
    if(context_struct == NULL)
    {
        _mem_free(provider_struct);
        return NULL;
    }
    while(init_table[i].UID != 0)
    {
        i++;
    }
    /* number of buttons and leds */
    context_struct->BTNLED_COUNT = i;
    context_struct->CLIENT_HANDLE = NULL;
    context_struct->BTNLED_TABLE = _mem_alloc_system_zero((context_struct->BTNLED_COUNT)*sizeof(HMI_LWGPIO_STRUCT));
    if (context_struct->BTNLED_TABLE == NULL)
    {
        _mem_free(provider_struct);
        _mem_free(context_struct);
        return NULL;
    }
    i=0;

    provider_struct->POLL_PTR = (void (_CODE_PTR_ )(pointer))hmi_lwgpio_poll;
    provider_struct->GET_VALUE_PTR = (boolean(_CODE_PTR_)(pointer, uint_32, uint_32_ptr))hmi_lwgpio_get_value;
    provider_struct->SET_VALUE_PTR = (boolean(_CODE_PTR_)(pointer, uint_32, uint_32))hmi_lwgpio_set_value;
    provider_struct->ATTACH_CLIENT = (void(_CODE_PTR_)(pointer, pointer ))hmi_lwgpio_attach_client;
    provider_struct->INSTALL_INTERRUPT_PTR = (void(_CODE_PTR_)(pointer ))hmi_lwgpio_install_interrupt;
    provider_struct->UNINSTALL_INTERRUPT_PTR = (void(_CODE_PTR_)(pointer))hmi_lwgpio_uninstall_interrupt;
    while(init_table[i].UID != 0)
    {
        hmi_lwgpio_add(&init_table[i], &context_struct->BTNLED_TABLE[i]);
        i++;
    }
    provider_struct->CONTEXT_PTR = context_struct;
    return provider_struct;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _hmi_lwgpio_provider_deinit
* Returned Value   : HMI_PROVIDER_STRUCT_PTR
* Comments         :
*   This function deallocate all resources.
*
*END*--------------------------------------------------------------------*/
uint_32 hmi_lwgpio_provider_deinit(HMI_PROVIDER_STRUCT_PTR provider)
{
    uint_32 result = MQX_OK;
    HMI_LWGPIO_CONTEXT_STRUCT_PTR context_struct = (HMI_LWGPIO_CONTEXT_STRUCT_PTR) (provider)->CONTEXT_PTR;

    result = _mem_free(context_struct->BTNLED_TABLE);
    if(result != MQX_OK)
    {
        return result;
    }
    result = _mem_free((provider)->CONTEXT_PTR);
    if(result != MQX_OK)
    {
        return result;
    }
    (provider) = NULL;
    return result;
}
