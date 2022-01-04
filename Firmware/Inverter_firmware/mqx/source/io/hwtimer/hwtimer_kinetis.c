/*HEADER********************************************************************
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
 **************************************************************************
 *
 * $FileName: hwtimer_kinetis.c$
 * $Version : 4.0.1$
 * $Date : Jan-18-2013$
 *
 * Comments:
 *
 *   This file contains kinetis specific functions of the hwtimer component.
 *
 *END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "hwtimer.h"

/*!
 * \cond DOXYGEN_PRIVATE
 * Macro return number of item in pit_vectors_table array
 */
#define PIT_INTERRUPT_COUNT 4

/* Array of PIT interrupt vectors*/
/*!
 * \cond DOXYGEN_PRIVATE
 * \brief Array of PIT interrupt vectors
 */
const _mqx_uint pit_vectors_table[] =
    {
        INT_PIT0,
        INT_PIT1,
        INT_PIT2,
        INT_PIT3
    };

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This function performs BSP-specific initialization related to pit
 *
 * \param dev_num[in]   Number of PIT module.
 *
 * \return MQX_OK Success.
 *
 * \see pit_get_vectors
 * \see pit_get_hwtimers_array
 */
_mqx_int pit_io_init
(
    uint_32 dev_num
)
{
    SIM_MemMapPtr sim = SIM_BASE_PTR;

    switch (dev_num)
    {
        case 0:
            SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;    /* PIT clock enablement */
            break;
        default:
            /* Do nothing if bad dev_num was selected */
            return -1;
    }

    return MQX_OK;
}

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This function get array of vectors and count of items in this array.
 *
 * \param pit_vectors_table_ptr[out]  Used to get pit_vectors_table.
 *
 * \return PIT_INTERRUPT_COUNT Count of interrupt vectors for PIT module.
 *
 * \see pit_io_init
 * \see pit_get_hwtimers_array
 */
uint_32 pit_get_vectors
(
    uint_32 pit_number,
    const _mqx_uint **pit_vectors_table_ptr
)
{
    switch (pit_number)
    {
        case 0:
            *pit_vectors_table_ptr = pit_vectors_table;
            break;
        default:
            *pit_vectors_table_ptr = NULL;
    }

    return PIT_INTERRUPT_COUNT;
}

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This function is useless for this platform
 *
 * \param hwtimers_array[out] Array of hwtimers. Filled by NULL for this platform
 *
 * \return 0 This platform doesnt need hwtimers_array.
 *
 * \see pit_io_init
 * \see pit_get_vectors
 */
uint_32 pit_get_hwtimers_array
(
    HWTIMER_PTR ** hwtimers_array
)
{
    *hwtimers_array = NULL;
    return 0;
}
