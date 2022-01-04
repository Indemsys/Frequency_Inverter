/*HEADER*********************************************************************
* 
* Copyright (c) 2013 Freescale Semiconductor;
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
***************************************************************************
* $FileName: mcc_vf600.c$
* $Version : 3.8.1.0$
* $Date    : Jul-3-2012$
*
* Comments:
*
*   This file contains Vybrid-specific functions of the MCC library
*
*END************************************************************************/

#include "mcc_vf600.h"
#include "mcc_mqx.h"

/* This field contains CPU-to-CPU interrupt vector numbers for all device cores */
static const unsigned int mcc_cpu_to_cpu_vectors[] = { INT_CPU_to_CPU_int0, NVIC_CPU_to_CPU_int0 };

/*!
 * \brief This function gets the CPU-to-CPU vector number for the particular core.
 *
 * Platform-specific inter-CPU vector numbers for each core are defined in the mcc_cpu_to_cpu_vectors[] field.
 *
 * \param[in] core Core number.
 *
 * \return vector number for the particular core
 * \return MCC_VECTOR_NUMBER_INVALID (vector number for the particular core number not found)
 */
unsigned int mcc_get_cpu_to_cpu_vector(unsigned int core)
{
    if (core < (sizeof(mcc_cpu_to_cpu_vectors)/sizeof(mcc_cpu_to_cpu_vectors[0]))) {
        return  mcc_cpu_to_cpu_vectors[core];

    }
    return MCC_VECTOR_NUMBER_INVALID;
}

/*!
 * \brief This function clears the CPU-to-CPU interrupt flag for the particular core.
 *
 * Implementation is platform-specific.
 *
 * \param[in] core Core number.
 */
void mcc_clear_cpu_to_cpu_interrupt(unsigned int core)
{
    if(core == 0) {
        /* clear the flag in the MSCM_IRCP0IR register */
        MSCM_IRCP0IR = MSCM_IRCP0IR_INT0_MASK;
    }
    else if(core == 1) {
        /* clear the flag in the MSCM_IRCP1IR register */
        MSCM_IRCP1IR = MSCM_IRCP1IR_INT0_MASK;
   }
}

/*!
 * \brief This function triggers the CPU-to-CPU interrupt.
 *
 * Platform-specific software triggering the inter-CPU interrupts.
 */
void mcc_triger_cpu_to_cpu_interrupt(void)
{
    /* set TLF filed of the MSCM_IRCPGIR to assert directed CPU interrupts for all processors except the requesting core */
    MSCM_IRCPGIR = MSCM_IRCPGIR_TLF(1) | MSCM_IRCPGIR_INTID(0);
}
