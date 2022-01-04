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
* $FileName: hmi_tss_provider.c$
* $Version : 3.8.2.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   This contains functions for TSS provider.
*
*END************************************************************************/
#include "mqx.h"
#include "bsp.h"
#include "hmi_tss_provider.h"
#include "btnled.h"
#include "TSS_API.h"

#define HMI_TSS_OFFSET_ELECTRODES   ((TSS_N_ELECTRODES/8)+1)

/* Functions prototypes */
static void hmi_tss_attach_client(HMI_PROVIDER_STRUCT_PTR , HMI_CLIENT_STRUCT_PTR );
static void hmi_tss_poll(HMI_PROVIDER_STRUCT_PTR );
static void tss_add_element(const HMI_TSS_INIT_STRUCT _PTR_ , HMI_TSS_INIT_STRUCT_PTR );
static HMI_TSS_CONTEXT_STRUCT_PTR hmi_tss_provider_init(const HMI_TSS_INIT_STRUCT _PTR_ , HMI_PROVIDER_STRUCT_PTR _PTR_ );
static boolean hmi_tss_get_value(HMI_PROVIDER_STRUCT_PTR, uint_32, uint_32_ptr);
static void hmi_tss_set_electrodes(uint_8, boolean);
void hmi_tss_keypad_callback(TSS_CONTROL_ID);
void hmi_tss_rotary_callback(TSS_CONTROL_ID);
void hmi_tss_slider_callback(TSS_CONTROL_ID);

/* TSS functions prototypes (TSS library) */
pointer TSS_GetControlPrivateData(uint_8);
uint_8  TSS_GetControlFirstElectrodeIndex(uint_8);
uint_8  TSS_GetControlLastElectrodeIndex(uint_8);
uint_8  TSS_GetControlElectrodeNumber(uint_8);
void    TSS_SetControlPrivateData(uint_8, pointer);
uint_16 TSS_GetSystemConfig(uint_8);
uint_8  TSS_SetSystemConfig(uint_8, uint_16);
uint_8  TSS_SetSliderConfig(TSS_CONTROL_ID, uint_8, uint_8);
uint_8  TSS_SetKeypadConfig(TSS_CONTROL_ID, uint_8, uint_8);
uint_8 _PTR_ TSS_GetControlStruct(uint_8);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_keypad_callback
* Returned Value   : void
* Comments         :
*   This function is callback for TSS buttons.
*
*END*--------------------------------------------------------------------*/
void hmi_tss_keypad_callback(TSS_CONTROL_ID u8ControlId)
{    /* get provider structure from TSS private data */
    HMI_PROVIDER_STRUCT_PTR     provider_struct = (HMI_PROVIDER_STRUCT_PTR)TSS_GetControlPrivateData(u8ControlId.ControlNumber);    
    /* get context data */
    HMI_TSS_CONTEXT_STRUCT_PTR  context_struct = (HMI_TSS_CONTEXT_STRUCT_PTR)provider_struct->CONTEXT_PTR;                            
    /* get keypad structure pointer */
    TSS_CSKeypad _PTR_          keypad_structure = ((TSS_CSKeypad *)TSS_GetControlStruct(context_struct->CONTROL_NUMBER));            
    uint_8                      u8Event;    /* 8 bits local variable used to store the event information */                                                                                            
    uint_8                      flag;       /* push / release flag */

    /* check if client is attached to provider */
    if (context_struct == NULL || context_struct->CLIENT_HANDLE == NULL)        
    {
        return;
    }
    /* While unread events are in the buffer */
    while (!TSS_KEYPAD_BUFFER_EMPTY(_PTR_ keypad_structure))          
    {
        /* Read the buffer and store the event in the u8Event variable */
        TSS_KEYPAD_BUFFER_READ(u8Event, _PTR_ keypad_structure);     
        
        if (u8Event & TSS_KEYPAD_BUFFER_RELEASE_FLAG)
        {
            u8Event = (uint_8)(u8Event & TSS_KEYPAD_BUFFER_KEY_MASK);    /* Get key index */
            flag = HMI_VALUE_RELEASE; 
        }
        else
        {
            flag = HMI_VALUE_PUSH;
        }
        /* check if key index is registered in provider */
        if (u8Event >= context_struct->TSS_COUNT)
        {
            return;
        }
        context_struct->CLIENT_HANDLE->ON_CHANGE(context_struct->CLIENT_HANDLE, context_struct->TSS_TABLE[u8Event].UID, flag);
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_rotary_callback
* Returned Value   : void
* Comments         :
*   This function is rotary callback.
*
*
*END*--------------------------------------------------------------------*/
void hmi_tss_rotary_callback(TSS_CONTROL_ID u8ControlId)
{    /* get provider structure from TSS private data */
    HMI_PROVIDER_STRUCT_PTR     provider_struct = (HMI_PROVIDER_STRUCT_PTR)TSS_GetControlPrivateData(u8ControlId.ControlNumber);    
     /* get context data */
    HMI_TSS_CONTEXT_STRUCT_PTR  context_struct = (HMI_TSS_CONTEXT_STRUCT_PTR) provider_struct->CONTEXT_PTR;

    /* check if client is attached to provider */
    if (context_struct == NULL || context_struct->CLIENT_HANDLE == NULL)        
    {
        return;
    }
    
    if ((*(TSS_CSRotary *)TSS_GetControlStruct(context_struct->CONTROL_NUMBER)).DynamicStatus.Movement)
    {
        context_struct->CLIENT_HANDLE->ON_CHANGE(context_struct->CLIENT_HANDLE, context_struct->TSS_TABLE[0].UID, HMI_VALUE_MOVEMENT);    
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_slider_callback
* Returned Value   : void
* Comments         :
*   This function is slider callback.
*
*END*--------------------------------------------------------------------*/
void hmi_tss_slider_callback(TSS_CONTROL_ID u8ControlId)
{    /* get provider struct from TSS private data */
    HMI_PROVIDER_STRUCT_PTR     provider_struct = (HMI_PROVIDER_STRUCT_PTR)TSS_GetControlPrivateData(u8ControlId.ControlNumber);    
    /* get context data */
    HMI_TSS_CONTEXT_STRUCT_PTR  context_struct = (HMI_TSS_CONTEXT_STRUCT_PTR) provider_struct->CONTEXT_PTR;
    
    /* check if client is attached to provider */
    if (context_struct == NULL || context_struct->CLIENT_HANDLE == NULL)        
    {
        return;
    }
    
    if ((*(TSS_CSSlider *)TSS_GetControlStruct(context_struct->CONTROL_NUMBER)).DynamicStatus.Movement)
    {
        context_struct->CLIENT_HANDLE->ON_CHANGE(context_struct->CLIENT_HANDLE, context_struct->TSS_TABLE[0].UID, HMI_VALUE_MOVEMENT);
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_get_value
* Returned Value   : boolean
* Comments         :
*   This function gets value of requested TSS element.
*
*END*--------------------------------------------------------------------*/
static boolean hmi_tss_get_value(HMI_PROVIDER_STRUCT_PTR provider_struct, uint_32 uid, uint_32_ptr value)
{    /* get context structure */
    HMI_TSS_CONTEXT_STRUCT_PTR  context_struct = (HMI_TSS_CONTEXT_STRUCT_PTR) provider_struct->CONTEXT_PTR;    

    uint_8      i;
    boolean     result = FALSE;
    uint_8      electrodes_status;
    uint_8      electrode_index;
    /* get TSS control structure */
    uint_8_ptr     tss_control_struct_ptr = (uint_8_ptr)TSS_GetControlStruct(context_struct->CONTROL_NUMBER);

    if (tss_control_struct_ptr == NULL || context_struct == NULL)
    {
        return result;
    }
    if ((((TSS_CSRotary *)tss_control_struct_ptr)->ControlId.ControlType) == TSS_CT_ROTARY)        /* rotary */
    {
        if (context_struct->TSS_TABLE[0].UID == uid)
        {
            *value = (*(TSS_CSRotary *)tss_control_struct_ptr).StaticStatus.Position;            /* actual absolute position */
            result = TRUE;
        }
    }
    else if ((((TSS_CSSlider *)tss_control_struct_ptr)->ControlId.ControlType) == TSS_CT_SLIDER)    /* slider */
    {
        if (context_struct->TSS_TABLE[0].UID == uid)
        {
            *value = (*(TSS_CSSlider *)tss_control_struct_ptr).StaticStatus.Position;            /* actual absolute position */
            result = TRUE;
        }
    }
    else if ((((TSS_CSKeypad *)tss_control_struct_ptr)->ControlId.ControlType) == TSS_CT_KEYPAD)    /* keypad */
    {
        for (i=0; i<context_struct->TSS_COUNT; i++)
        {
            if (context_struct->TSS_TABLE[i].UID == uid)
            {
                /* get index */
                electrode_index = (TSS_GetControlFirstElectrodeIndex(context_struct->CONTROL_NUMBER) + i);            
                /* electrodes status */    
                electrodes_status = TSS_GetSystemConfig(System_ElectrodeStatus_Register + (electrode_index/8u));    
                /* status of requested electrode */
                *value = (uint_32)(electrodes_status >> (electrode_index % 8u)) & 0x1;                                        
                result = TRUE;
                break;
            }
        }
    }
    return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : TSS_fOnFault
* Returned Value   : void
* Comments         :
*   This function clears the fault flag of an electrode if fault was detected.
*   User can add his action if fault occurs.
*END*--------------------------------------------------------------------*/
void hmi_tss_on_fault(uint_8 electrode_number)
{

    /* If fault occurs, electrode is re-enabled here */
    if(tss_CSSys.Faults.ChargeTimeout || tss_CSSys.Faults.SmallCapacitor)    /* if large or small capacitor fault  */
    {
        /* clear the fault flag */
        (void) TSS_SetSystemConfig(System_Faults_Register, 0x00); 

    /* User can write code here when faul is detected */        
    }
    
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_attach_client
* Returned Value   : void
* Comments         :
*    This function attaches client to provider.
*
*END*--------------------------------------------------------------------*/
static void hmi_tss_attach_client(HMI_PROVIDER_STRUCT_PTR provider_struct, HMI_CLIENT_STRUCT_PTR client_struct)
{
    HMI_TSS_CONTEXT_STRUCT_PTR context = (HMI_TSS_CONTEXT_STRUCT_PTR)provider_struct->CONTEXT_PTR;
    
    context->CLIENT_HANDLE = client_struct;        /* attach client to provider */
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_poll
* Returned Value   : void
* Comments         :
*   This function calls TSS task.
*
*END*--------------------------------------------------------------------*/
static void hmi_tss_poll(HMI_PROVIDER_STRUCT_PTR provider_struct)
{
    while (TSS_Task() != TSS_STATUS_OK)        /* TSS Task */
    {
        
    }    
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : tss_add_element
* Returned Value   : void
* Comments         :
*   This function adds TSS element.
*
*END*--------------------------------------------------------------------*/
static void tss_add_element(const HMI_TSS_INIT_STRUCT _PTR_ init_table, HMI_TSS_INIT_STRUCT_PTR tss_struct)
{
    /* saves data from init table to init structure */
    tss_struct->UID = init_table->UID;        
    tss_struct->FLAG = init_table->FLAG;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_init
* Returned Value   : void
* Comments         :
*   This function initializes TSS.
*
*END*--------------------------------------------------------------------*/
void hmi_tss_init(const HMI_TSS_SYSTEM_CONTROL_STRUCT system_control)
{
    uint_8  i;
    
    TSS_Init();                         /* TSS initialize */
    
    for (i=0; i<TSS_N_ELECTRODES; i++)    
    {
        (void)TSS_SetSystemConfig(System_Sensitivity_Register + i, system_control.SENSITIVITY_VALUES[i]);    /* set sensitivity values */
    }
    (void)TSS_SetSystemConfig(System_NSamples_Register, system_control.NUMBER_OF_SAMPLES);                   /* set number of samples */
    (void)TSS_SetSystemConfig(System_SystemTrigger_Register, system_control.SYSTEM_TRIGGER);                 /* TSS system trigger */
    (void)TSS_SetSystemConfig(System_AutoTriggerModuloValue_Register, 0xFF);                                 /* set auto trigger modulo value */
    (void)TSS_SetSystemConfig(System_SystemConfig_Register, system_control.SYSTEM_CONFIG);                   /* set system config */
        
    for(i=0; i<HMI_TSS_OFFSET_ELECTRODES; i++)
    {
        TSS_SetSystemConfig(System_ElectrodeEnablers_Register + i, 0x00);                                    /* disable all electrodes */
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_provider_init
* Returned Value   : void
* Comments         :
*   This function allocates provider .
*
*END*--------------------------------------------------------------------*/
static HMI_TSS_CONTEXT_STRUCT_PTR hmi_tss_provider_init(const HMI_TSS_INIT_STRUCT _PTR_ init_table, HMI_PROVIDER_STRUCT_PTR _PTR_ provider_struct)
{
    uint_8                         i=0;
    HMI_TSS_CONTEXT_STRUCT_PTR     context_struct;
    
     /* allocate provider structure */
    _PTR_ provider_struct = _mem_alloc_system(sizeof(HMI_PROVIDER_STRUCT));        
    if(provider_struct == NULL)
    {
        return NULL;
    }
    /* allocate providers context structure */
    context_struct = _mem_alloc_system(sizeof(HMI_TSS_CONTEXT_STRUCT));            
    if(context_struct == NULL)
    {
        _mem_free(provider_struct);
        return NULL;
    }
    
    while(init_table[i].UID != 0)
    {
        i++;
    }
    /* number of  tss provider elements */
    context_struct->TSS_COUNT = i;                    
    /* allocate providers table */
    context_struct->TSS_TABLE = _mem_alloc_system_zero(sizeof(HMI_TSS_INIT_STRUCT)*(i));    
    if (context_struct->TSS_TABLE == NULL)
    {
        _mem_free(provider_struct);
        _mem_free(context_struct);
        return NULL;
    }
    i=0;
    while(init_table[i].UID != 0)
    {
        tss_add_element(&init_table[i], &context_struct->TSS_TABLE[i]);            /* Add TSS element */
        i++;
    }
    
    return context_struct;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_set_electrodes
* Returned Value   : void
* Comments         :
*   This function enables/disables electrodes.
*
*END*--------------------------------------------------------------------*/
static void hmi_tss_set_electrodes(uint_8 control_number, boolean enable_electrodes)
{
    uint_8  i;
    uint_8  offset;
    uint_8  result;
    uint_8  first_electrode;
    uint_8  last_electrode;

    /* first electrode for a control */
    first_electrode = TSS_GetControlFirstElectrodeIndex(control_number);
    /* last electrode for a control */
    last_electrode = TSS_GetControlLastElectrodeIndex(control_number);
    /* get offset in which first electrode is */
    offset = first_electrode/8;                                                
    /* get for this offset electrodes en/dis status */
    result = TSS_GetSystemConfig(System_ElectrodeEnablers_Register + offset);    
    
    for (i=first_electrode; i<=last_electrode; i++)
    {
        if (enable_electrodes)
        {
            result |= 1 << (i%8);        /* enable electrode */
        }
        else
        {
            result &= ~(1 << (i%8));    /* disable electrode */
        }
        if (((i+1)%8) == 0)
        {
            (void)TSS_SetSystemConfig(System_ElectrodeEnablers_Register + offset, result);    /* store enable config */
            offset++;
            result = TSS_GetSystemConfig(System_ElectrodeEnablers_Register + offset);        /* get en/dis status */        
        }
    }
    if ((i%8) != 0)
    {
        (void)TSS_SetSystemConfig(System_ElectrodeEnablers_Register + offset, result);        /* store rest of enable config */
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_keypad_provider_init
* Returned Value   : HMI_PROVIDER_STRUCT_PTR
* Comments         :
*   This function initializes TSS provider.
*
*
*END*--------------------------------------------------------------------*/
HMI_PROVIDER_STRUCT_PTR hmi_tss_keypad_provider_init(const HMI_TSS_INIT_STRUCT _PTR_  init_table, uint_8 control_number)
{
    HMI_PROVIDER_STRUCT_PTR         provider_struct;
    HMI_TSS_CONTEXT_STRUCT_PTR      context_struct;
    uint_8_ptr                      tss_control_struct_ptr = (uint_8_ptr)TSS_GetControlStruct(control_number);

    /* allocate memory and add tss elements */
    context_struct = hmi_tss_provider_init(init_table, &provider_struct);        
    /* enable electrodes */
    hmi_tss_set_electrodes(control_number, TRUE);                                
    /* store TSS control number in the context structure */
    context_struct->CONTROL_NUMBER = control_number;                            
    context_struct->CLIENT_HANDLE = NULL;    
    provider_struct->GET_VALUE_PTR = (boolean(_CODE_PTR_)(pointer, uint_32, uint_32_ptr))hmi_tss_get_value;        /* provider structure function pointers */
    provider_struct->INSTALL_INTERRUPT_PTR = NULL;
    provider_struct->ATTACH_CLIENT =(void(_CODE_PTR_)(pointer, pointer ))hmi_tss_attach_client;
    provider_struct->POLL_PTR =(void (_CODE_PTR_ )(pointer))hmi_tss_poll;
    provider_struct->SET_VALUE_PTR = NULL;
    provider_struct->UNINSTALL_INTERRUPT_PTR = NULL;
    provider_struct->CONTEXT_PTR = context_struct;
    
    (void)TSS_SetKeypadConfig(((TSS_CSKeypad *)tss_control_struct_ptr)->ControlId, Keypad_Events_Register, (TSS_KEYPAD_TOUCH_EVENT_EN_MASK | TSS_KEYPAD_RELEASE_EVENT_EN_MASK));
    (void)TSS_SetKeypadConfig(((TSS_CSKeypad *)tss_control_struct_ptr)->ControlId, Keypad_ControlConfig_Register, (TSS_KEYPAD_CALLBACK_EN_MASK | TSS_KEYPAD_CONTROL_EN_MASK));
    TSS_SetControlPrivateData(control_number, provider_struct);        /* set private data in TSS */
    
    return provider_struct;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_rotary_provider_init
* Returned Value   : HMI_PROVIDER_STRUCT_PTR
* Comments         :
*   This function initializes TSS provider.
*
*
*END*--------------------------------------------------------------------*/
HMI_PROVIDER_STRUCT_PTR hmi_tss_rotary_provider_init(const HMI_TSS_INIT_STRUCT _PTR_  init_table, uint_8 control_number)
{
    HMI_PROVIDER_STRUCT_PTR         provider_struct;
    HMI_TSS_CONTEXT_STRUCT_PTR      context_struct;
    uint_8_ptr                      tss_control_struct_ptr = (uint_8_ptr)TSS_GetControlStruct(control_number);

    /* allocate memory and add tss elements */
    context_struct = hmi_tss_provider_init(init_table, &provider_struct);            
    /* enable electrodes */
    hmi_tss_set_electrodes(control_number, TRUE);                                
    /* store TSS control number in the context structure */
    context_struct->CONTROL_NUMBER = control_number;                            
    context_struct->CLIENT_HANDLE = NULL;
    /* provider structure function pointers */
    provider_struct->GET_VALUE_PTR =  (boolean(_CODE_PTR_)(pointer, uint_32, uint_32_ptr))hmi_tss_get_value;        
    provider_struct->INSTALL_INTERRUPT_PTR = NULL;
    provider_struct->ATTACH_CLIENT = (void(_CODE_PTR_)(pointer, pointer ))hmi_tss_attach_client;
    provider_struct->POLL_PTR = (void (_CODE_PTR_ )(pointer))hmi_tss_poll;
    provider_struct->SET_VALUE_PTR = NULL;
    provider_struct->UNINSTALL_INTERRUPT_PTR = NULL;
    provider_struct->CONTEXT_PTR = context_struct;
    
    (void)TSS_SetRotaryConfig(((TSS_CSRotary *)tss_control_struct_ptr)->ControlId, Rotary_Events_Register, (TSS_ROTARY_MOVEMENT_EVENT_EN_MASK | TSS_ROTARY_INITIAL_TOUCH_EVENT_EN_MASK));
    (void)TSS_SetRotaryConfig(((TSS_CSRotary *)tss_control_struct_ptr)->ControlId, Rotary_ControlConfig_Register, (TSS_ROTARY_CONTROL_EN_MASK | TSS_ROTARY_CALLBACK_EN_MASK));
    TSS_SetControlPrivateData(control_number, provider_struct);        /* set private data in TSS */
    
    return provider_struct;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_slider_provider_init
* Returned Value   : HMI_PROVIDER_STRUCT_PTR
* Comments         :
*   This function initializes TSS provider.
*
*
*END*--------------------------------------------------------------------*/
HMI_PROVIDER_STRUCT_PTR hmi_tss_slider_provider_init(const HMI_TSS_INIT_STRUCT _PTR_ init_table, uint_8 control_number)
{
    HMI_PROVIDER_STRUCT_PTR         provider_struct;
    HMI_TSS_CONTEXT_STRUCT_PTR      context_struct;
    uint_8_ptr                      tss_control_struct_ptr = (uint_8_ptr)TSS_GetControlStruct(control_number);

    /* allocate memory and add tss elements */
    context_struct = hmi_tss_provider_init(init_table,&provider_struct);    
    /* enable electrodes */
    hmi_tss_set_electrodes(control_number, TRUE);                            
    /* store TSS control number in the context structure */
    context_struct->CONTROL_NUMBER = control_number;                            
    context_struct->CLIENT_HANDLE = NULL;
    /* provider structure function pointers */
    provider_struct->GET_VALUE_PTR =  (boolean(_CODE_PTR_)(pointer, uint_32, uint_32_ptr))hmi_tss_get_value;
    provider_struct->INSTALL_INTERRUPT_PTR = NULL;
    provider_struct->ATTACH_CLIENT = (void(_CODE_PTR_)(pointer, pointer ))hmi_tss_attach_client;
    provider_struct->POLL_PTR = (void (_CODE_PTR_ )(pointer))hmi_tss_poll;
    provider_struct->SET_VALUE_PTR = NULL;
    provider_struct->UNINSTALL_INTERRUPT_PTR = NULL;
    provider_struct->CONTEXT_PTR = context_struct;
    
    (void)TSS_SetSliderConfig(((TSS_CSSlider *)tss_control_struct_ptr)->ControlId, Slider_Events_Register, (TSS_SLIDER_MOVEMENT_EVENT_EN_MASK | TSS_SLIDER_INITIAL_TOUCH_EVENT_EN_MASK));
    (void)TSS_SetSliderConfig(((TSS_CSSlider *)tss_control_struct_ptr)->ControlId, Slider_ControlConfig_Register, (TSS_SLIDER_CONTROL_EN_MASK | TSS_SLIDER_CALLBACK_EN_MASK));
    TSS_SetControlPrivateData(control_number, provider_struct);        /* set private data in TSS */
    
    return provider_struct;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _hmi_tss_provider_deinit
* Returned Value   : uint_32
* Comments         :
*   This function deallocates all resources and disables provider electrodes.
*
*END*--------------------------------------------------------------------*/
uint_32 hmi_tss_provider_deinit(HMI_PROVIDER_STRUCT_PTR provider)
{
    uint_32 result = MQX_OK;
    HMI_TSS_CONTEXT_STRUCT_PTR context_struct = (HMI_TSS_CONTEXT_STRUCT_PTR) (provider)->CONTEXT_PTR;
    
    /* disable providers electrodes */
    hmi_tss_set_electrodes(context_struct->CONTROL_NUMBER, FALSE);            
    result = _mem_free(context_struct->TSS_TABLE);
    if(result != MQX_OK)
    {
        return result;
    }
    result = _mem_free(provider->CONTEXT_PTR);
    if(result != MQX_OK)
    {
        return result;
    }
    provider = NULL;

    return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : hmi_tss_empty_function
* Returned Value   :
* Comments         :
*   Function definition for TSS_SystemSetup.h .
*
*END*--------------------------------------------------------------------*/
void hmi_tss_empty_function(void)
{
    /* TSS On init ,replaced with _bsp_hmi_init() */
}
