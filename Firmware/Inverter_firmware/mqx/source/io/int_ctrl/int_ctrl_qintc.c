/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2011 Embedded Access Inc.;
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
* $FileName: int_ctrl_qintc.c$
* $Version : 3.8.5.0$
* $Date    : Jul-9-2012$
*
* Comments:
*
*   This file contains the source functions for the QINTC driver.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "bsp.h"
#include "psp.h"


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qintc_init
* Returned Value   : none
* Comments         :
*    Initializes interrupt controller
*
*END*------------------------------------------------------------------------*/

static void _qintc_init
    (
        /* [IN] interrupt controller base address */
        VQINTC_REG_STRUCT_PTR qintc_ptr,

        /* [IN] vector mode */
        int mode
    )
{
    uint_32              i;

    /* Disable interrupts */
    qintc_ptr->CPR = QINTC_PRI_MAX;

    /*
    ** Vector table entry size: 4 bytes
    ** Vector mode: software
    */
    if (mode)
    {
        qintc_ptr->BCR = QINTC_BCR_HVEN;
    }
    else
    {
        qintc_ptr->BCR &= ~(QINTC_BCR_VTES | QINTC_BCR_HVEN);
    }

    qintc_ptr->IACKR = _PSP_GET_VECTOR_TABLE();

    for (i=0; i<8; i++) qintc_ptr->SSCIR[i]=QINTC_SSCIR_CLR;
    for (i=0; i<=_PSP_VECTOR_TO_IRQ(QINTC_LAST_VECTOR); i++) qintc_ptr->PSR[i]=0;

    /* lower PRI in QINTC_CPR to zero */
    qintc_ptr->CPR = QINTC_PRI_MIN;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qintc_set_prio
* Returned Value   : none
* Comments         :
*    Sets priority for given interrupt vector
*
*END*------------------------------------------------------------------------*/

uint_32 _qintc_set_prio
    (
        /* [IN] interrupt vector number */
        uint_32 vector,

        /* [IN] interrupt  priority*/
        uint_32  prio
    )
{
    VQINTC_REG_STRUCT_PTR qintc_ptr = _bsp_get_qintc_base_address();
    uint_32 irq;

    if ((vector < QINTC_FIRST_VECTOR) || (vector > QINTC_LAST_VECTOR))
    {
        return MQX_INVALID_PARAMETER;
    }
    irq = _PSP_VECTOR_TO_IRQ(vector);

    if (prio > QINTC_PRI_MAX) prio = QINTC_PRI_MAX;
    qintc_ptr->PSR[irq]=prio;

    return MQX_OK;
}


/*ISR*---------------------------------------------------------------------
*
* Function Name    : _qintc_external_isr
* Comments         :
*   This ISR is to be installed into the external interrupt vector
* location in order to capture all external interrupts
*
*END*----------------------------------------------------------------------*/

void _qintc_external_isr
    (
        /* [IN] interrupt controller base address */
           pointer base
    )
{
    VQINTC_REG_STRUCT_PTR qintc_ptr = (VQINTC_REG_STRUCT_PTR)base;
    uint_32 vector;

    /* Find the interrupt source */
    vector = _PSP_IRQ_TO_VECTOR((qintc_ptr->IACKR & QINTC_IACKR_INTVEC) >> 2);

     /* invoke the interrupt service routine associated with the interrupt code */
    _psp_generate_interrupt(vector);

    /* End of interrupt */
    qintc_ptr->EOIR = 0;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qintc_install
* Returned Value   : none
* Comments         :
*    Initializes interrupt controller in sw vector mode and intalls isr
*
*END*------------------------------------------------------------------------*/

uint_32 _qintc_install
    (
        /* [IN] interrupt controller base address */
        VQINTC_REG_STRUCT_PTR base,

        /* [IN] external vector/exception number */
        int vector
    )
{
    #if PSP_HAS_DEVICE_PROTECTION
    if (!_bsp_qintc_enable_access()) {
        return MQX_INVALID_DEVICE;
    }
    #endif

    _qintc_init(base, 0);

    /* Install the decrementer interrupt handler */
    if (_int_install_isr(vector, _qintc_external_isr, (pointer)base) == NULL)
    {
        return _task_get_error();
    }

    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qintc_set_sw_interrupt
* Returned Value   : none
* Comments         :
*    Causes the sw interrupt for the specified vector to occur
*
*END*------------------------------------------------------------------------*/

uint_32 _qintc_set_sw_interrupt
    (
        /* [IN] interrupt vector number */
        uint_32 vector
    )
{
    VQINTC_REG_STRUCT_PTR qintc_ptr = _bsp_get_qintc_base_address();
    uint_32 irq;

    irq = _PSP_VECTOR_TO_IRQ(vector);

    if (irq > 7)
    {
        return MQX_INVALID_PARAMETER;
    }

    qintc_ptr->SSCIR[irq]=QINTC_SSCIR_SET;
    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qintc_clear_sw_interrupt
* Returned Value   : none
* Comments         :
*    Clears the sw interrupt for the specified vector
*
*END*------------------------------------------------------------------------*/

uint_32 _qintc_clear_sw_interrupt
    (
        /* [IN] interrupt vector number */
        uint_32 vector
    )
{
    VQINTC_REG_STRUCT_PTR qintc_ptr = _bsp_get_qintc_base_address();
    uint_32 irq;

    irq = _PSP_VECTOR_TO_IRQ(vector);

    if (irq > 7)
    {
        return MQX_INVALID_PARAMETER;
    }

    qintc_ptr->SSCIR[irq]=QINTC_SSCIR_CLR;
    return MQX_OK;
}

#if PSP_NUM_CORES>1
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qintc_set_core_sw_interrupt
* Returned Value   : none
* Comments         :
*    Causes the sw interrupt for the specified vector to occur
*
*END*------------------------------------------------------------------------*/

uint_32 _qintc_set_core_sw_interrupt
    (
        /* [IN] core number */
        uint_32 core_num,
        /* [IN] interrupt vector number */
        uint_32 vector
    )
{
    VQINTC_REG_STRUCT_PTR qintc_ptr = _bsp_get_core_qintc_base_address(core_num);
    uint_32 irq;

    irq = _PSP_VECTOR_TO_IRQ(vector);

    if (irq > 7)
    {
        return MQX_INVALID_PARAMETER;
    }

    qintc_ptr->SSCIR[irq]=QINTC_SSCIR_SET;
    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qintc_set_core0_sw_interrupt
* Returned Value   : none
* Comments         :
*    Causes the sw interrupt for the specified vector to occur
*
*END*------------------------------------------------------------------------*/
uint_32 _qintc_set_core0_sw_interrupt
    (
        /* [IN] interrupt vector number */
        uint_32 vector
    )
{
    return _qintc_set_core_sw_interrupt(0,vector);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qintc_set_core1_sw_interrupt
* Returned Value   : none
* Comments         :
*    Causes the sw interrupt for the specified vector to occur
*
*END*------------------------------------------------------------------------*/
uint_32 _qintc_set_core1_sw_interrupt
    (
        /* [IN] interrupt vector number */
        uint_32 vector
    )
{
    return _qintc_set_core_sw_interrupt(1,vector);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _qintc_clear_core_sw_interrupt
* Returned Value   : none
* Comments         :
*    Clears the sw interrupt for the specified vector
*
*END*------------------------------------------------------------------------*/

uint_32 _qintc_clear_core_sw_interrupt
    (
        /* [IN] core number */
        uint_32 core_num,
        /* [IN] interrupt vector number */
        uint_32 vector
    )
{
    VQINTC_REG_STRUCT_PTR qintc_ptr = _bsp_get_core_qintc_base_address(core_num);
    uint_32 irq;

    irq = _PSP_VECTOR_TO_IRQ(vector);

    if (irq > 7)
    {
        return MQX_INVALID_PARAMETER;
    }

    qintc_ptr->SSCIR[irq]=QINTC_SSCIR_CLR;
    return MQX_OK;
}
#endif
