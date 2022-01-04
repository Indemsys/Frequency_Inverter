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
* $FileName: btnled.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the implementation of HMI client for buttons and LEDs.
*
*END************************************************************************/
#include "btnled.h"
#include "hmi_client.h"

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : btnled_poll
* Returned Value   : void
* Comments         :
*   This function calls poll function of all added providers.
*
*END*--------------------------------------------------------------------*/
void btnled_poll(HMI_CLIENT_STRUCT_PTR handle)
{
    uint_32 i;

    if(handle == NULL)
    {
        return;
    }
    for(i=0; i<handle->MAX_PROVIDER_COUNT; i++)
    {
        if((handle->PROVIDER_TABLE[i] != NULL) && (handle->PROVIDER_TABLE[i]->POLL_PTR != NULL))
        {
            handle->PROVIDER_TABLE[i]->POLL_PTR(handle->PROVIDER_TABLE[i]);
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : btnled_get_value
* Returned Value   : TRUE if UID exist, else FALSE
* Comments         :
*   This function calls get function of all added provider.
*
*END*--------------------------------------------------------------------*/
boolean btnled_get_value(HMI_CLIENT_STRUCT_PTR handle, uint_32 uid, uint_32_ptr value)
{
    uint_32 i;
    boolean result = FALSE;

    if(handle == NULL)
    {
        return FALSE;
    }
    for(i=0; i<handle->MAX_PROVIDER_COUNT; i++)
    {
        if((handle->PROVIDER_TABLE[i] != NULL) && (handle->PROVIDER_TABLE[i]->GET_VALUE_PTR != NULL))
        {
            result |= handle->PROVIDER_TABLE[i]->GET_VALUE_PTR(handle->PROVIDER_TABLE[i], uid, value);
        }
    }
    return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : btnled_set_value
* Returned Value   : TRUE if UID exist, else FALSE
* Comments         :
*   This function calls set function of all added provider.
*
*END*--------------------------------------------------------------------*/
boolean btnled_set_value(HMI_CLIENT_STRUCT_PTR handle, uint_32 uid, uint_32 value)
{
    uint_32 i;
    boolean result = FALSE;

    if(handle == NULL)
    {
        return FALSE;
    }
    for(i=0; i<handle->MAX_PROVIDER_COUNT; i++)
    {
        if((handle->PROVIDER_TABLE[i] != NULL) && (handle->PROVIDER_TABLE[i]->SET_VALUE_PTR != NULL))
        {
            result |= handle->PROVIDER_TABLE[i]->SET_VALUE_PTR(handle->PROVIDER_TABLE[i], uid, value);
        }
    }
    return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : btnled_toogle
* Returned Value   : TRUE if UID exist, else FALSE
* Comments         :
*   This function is for toogling a value of UID.
*
*END*--------------------------------------------------------------------*/
boolean btnled_toogle(HMI_CLIENT_STRUCT_PTR handle, uint_32 uid)
{
    uint_32 value;
    boolean result = FALSE;

    result = btnled_get_value(handle, uid, &value);
    result = btnled_set_value(handle, uid, !value);
    return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : btnled_add_clb
* Returned Value   : BTNLED_CLBREG_STRUCT_PTR
* Comments         :
*   This function is for callback registration.
*
*END*--------------------------------------------------------------------*/
BTNLED_CLBREG_STRUCT_PTR btnled_add_clb(HMI_CLIENT_STRUCT_PTR handle, uint_32 uid, uint_32 state, void (_CODE_PTR_ function)(pointer), pointer callback_parameter)
{
    BTNLED_CONTEXT_STRUCT_PTR   context;
    BTNLED_CLBREG_STRUCT_PTR    clbreg_struct;
    BTNLED_CLBREG_STRUCT_PTR    new_clbreg_struct;

    if(handle == NULL)
    {
        return NULL;
    }
    /* allocate btnled_clb_structure */
    new_clbreg_struct = _mem_alloc_system_zero(sizeof(BTNLED_CLBREG_STRUCT));
    if(new_clbreg_struct == NULL)
    {
        return NULL;
    }
    /* fill new new_clbreg_structure */
    new_clbreg_struct->STATE = state;
    new_clbreg_struct->UID = uid;
    new_clbreg_struct->CALLBACK_PARAMETER = callback_parameter;
    new_clbreg_struct->CLBFUNCTION = function;
    new_clbreg_struct->NEXT_CALLBACK = NULL;

    /* find last callback */
    context = (BTNLED_CONTEXT_STRUCT_PTR) handle->CONTEXT_PTR;
    /* wait semaphore */
    _lwsem_wait(&context->LOCK_BTNLED_SEM);
    clbreg_struct = context->CLBREG;
    if(clbreg_struct == NULL)
    {
        context->CLBREG = new_clbreg_struct;
    }
    else
    {
        while(clbreg_struct->NEXT_CALLBACK!=NULL)
        {
            clbreg_struct = clbreg_struct->NEXT_CALLBACK;
        }
        clbreg_struct->NEXT_CALLBACK = new_clbreg_struct;
    }
    /* post semaphore */
    _lwsem_post(&context->LOCK_BTNLED_SEM);
    return new_clbreg_struct;
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : btnled_remove_clb
* Returned Value   : TRUE if removed, FALSE otherwise.
* Comments         :
*   This function is for callback unregistration.
*
*END*--------------------------------------------------------------------*/
boolean btnled_remove_clb(HMI_CLIENT_STRUCT_PTR handle, BTNLED_CLBREG_STRUCT_PTR comp_clbreg)
{
    BTNLED_CONTEXT_STRUCT_PTR   context;
    BTNLED_CLBREG_STRUCT_PTR    step_clbreg;
    BTNLED_CLBREG_STRUCT_PTR    tmp_clbreg;

    if((handle == NULL) || (comp_clbreg == NULL))
    {
        return FALSE;
    }
    context = (BTNLED_CONTEXT_STRUCT_PTR)handle->CONTEXT_PTR;
    /* wait semaphore */
    _lwsem_wait(&context->LOCK_BTNLED_SEM);
    step_clbreg = context->CLBREG;
    if(step_clbreg == NULL)
    {
        /* post semaphore */
        _lwsem_post(&context->LOCK_BTNLED_SEM);
        return FALSE;
    }
    /* only one clbreg_struct is in a queue */
    if(step_clbreg == comp_clbreg)    
    {
        context->CLBREG = step_clbreg->NEXT_CALLBACK;
        if(_mem_free(step_clbreg) != MQX_OK)
        {
            _lwsem_post(&context->LOCK_BTNLED_SEM);
            return FALSE;
        }
        _lwsem_post(&context->LOCK_BTNLED_SEM);
        return TRUE;
    }
    /* more than one clbreg_struct is in a queue */
    while(step_clbreg->NEXT_CALLBACK!=NULL)    
    {
        if(step_clbreg->NEXT_CALLBACK == comp_clbreg)
        {
            tmp_clbreg = step_clbreg->NEXT_CALLBACK;
            step_clbreg->NEXT_CALLBACK = step_clbreg->NEXT_CALLBACK->NEXT_CALLBACK;
            if(_mem_free(tmp_clbreg)!=MQX_OK)
            {
                _lwsem_post(&context->LOCK_BTNLED_SEM);
                return FALSE;
            }
            _lwsem_post(&context->LOCK_BTNLED_SEM);
            return TRUE;
        }
        step_clbreg = step_clbreg->NEXT_CALLBACK;
    }
    _lwsem_post(&context->LOCK_BTNLED_SEM);
    return FALSE;
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : btnled_on_change
* Returned Value   : void
* Comments         :
*   This function is for callback processing.
*
*END*--------------------------------------------------------------------*/
static void btnled_on_change(HMI_CLIENT_STRUCT_PTR client, uint_32 uid, uint_32 state)
{
    BTNLED_CONTEXT_STRUCT_PTR   context;
    BTNLED_CLBREG_STRUCT_PTR    clbreg;


    if(client == NULL)
    {
        return;
    }
    /* gets a context from a client */
    context = (BTNLED_CONTEXT_STRUCT_PTR) client->CONTEXT_PTR;

    clbreg = context->CLBREG;
    if(clbreg == NULL)
    {
        return;
    }

    do
    {
        /* UID and STATE must match */
        if((clbreg->UID == uid) && (clbreg->STATE == state) && (clbreg->CLBFUNCTION != NULL))
        {
            clbreg->CLBFUNCTION(clbreg->CALLBACK_PARAMETER);  /* calls callback */
        }
        clbreg = clbreg->NEXT_CALLBACK;
    }while(clbreg != NULL);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : btnled_init
* Returned Value   : client struct HMI_CLIENT_STRUCT_PTR
* Comments         :
*   This function is for HMI BTNLED client initialization.
*
*END*--------------------------------------------------------------------*/
HMI_CLIENT_STRUCT_PTR btnled_init(void)
{
    HMI_CLIENT_STRUCT_PTR       btnled_struct;
    HMI_PROVIDER_STRUCT_PTR     provider_table;
    BTNLED_CONTEXT_STRUCT_PTR   context;

    /* allocate btnled structure */
    btnled_struct = hmi_client_init();
    provider_table = (pointer)btnled_struct->PROVIDER_TABLE;
    /* allocate context data */
    context = _mem_alloc_system_zero(sizeof(BTNLED_CONTEXT_STRUCT));
    if(context == NULL)
    {
        _mem_free(provider_table);
        _mem_free(btnled_struct);
        return NULL;
    }
    /* create a semaphore for callbacks */
    _lwsem_create(&context->LOCK_BTNLED_SEM, 1);
    btnled_struct->CONTEXT_PTR = context;
    btnled_struct->ON_CHANGE = (void ( _CODE_PTR_)(pointer, uint_32, uint_32))btnled_on_change;
    return btnled_struct;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : btnled_deinit
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function deallocates used memory.
*
*END*--------------------------------------------------------------------*/
uint_32 btnled_deinit(HMI_CLIENT_STRUCT_PTR handle)
{
    BTNLED_CONTEXT_STRUCT_PTR   context;
    uint_32                     result = MQX_OK;

    /* destroy callbacks and a semaphore */
    context = (BTNLED_CONTEXT_STRUCT_PTR) (handle)->CONTEXT_PTR;

    if(context->CLBREG != NULL)
    {
        do
        {
            btnled_remove_clb(handle, context->CLBREG);
        }
        while(context->CLBREG->NEXT_CALLBACK != NULL);
    }

    _lwsem_destroy(&context->LOCK_BTNLED_SEM);

    result =_mem_free(handle->PROVIDER_TABLE);
    if(result != MQX_OK)
    {
        return result;
    }
    result =_mem_free((handle)->CONTEXT_PTR);
    if(result != MQX_OK)
    {
        return result;
    }
    handle = NULL;
    return result;
}
