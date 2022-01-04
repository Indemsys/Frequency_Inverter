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
* $FileName: kinetis_mpu.c$
* $Version : 3.8.6.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains source for basic mpu settings.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "kinetis_mpu.h"

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _kinetis_mpu_disable
* Returned Value  : uint_32
*       Always return MQX_OK.
* Comments        :
* 	Disable MPU on cortex
*
*END*---------------------------------------------------------------------*/

_mqx_uint _kinetis_mpu_disable(void) {
    MPU_CESR &= ~MPU_CESR_VLD_MASK;

    return MQX_OK;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _kinetis_mpu_enable
* Returned Value  : uint_32
*       Always return MQX_OK.
* Comments        :
* 	Enable MPU on cortex
*
*END*---------------------------------------------------------------------*/

_mqx_uint _kinetis_mpu_enable(void) {
    MPU_CESR |= MPU_CESR_VLD_MASK;

    return MQX_OK;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name   : _kinetis_mpu_init
* Returned Value  : uint_32
*       Always return MQX_OK.
* Comments        :
* 	Initialize MPU and clear all active records in MPU
*
*END*---------------------------------------------------------------------*/

_mqx_uint _kinetis_mpu_init(void) {
    _mqx_uint i;

    MPU_CESR &= ~MPU_CESR_VLD_MASK;

    for (i = 0; i < CORTEX_MPU_REC; i++) {
        MPU_WORD(i, 2) = 0;
    }

    return MQX_OK;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name     : _kinetis_mpu_add_region
* Returned Value    : uint_32
*       Always return MQX_OK.
* Comments          :
* 	Add new region to MPU
*
*END*---------------------------------------------------------------------*/

_mqx_uint _kinetis_mpu_add_region
    (
        // [IN] mpu region start address
        uchar_ptr start,
        // [IN] mpu region end address
        uchar_ptr end,
        // [IN] access rights, direct value - cortex mpu specific
        _mqx_uint flags
    )
{
    _mqx_uint i;
    _mqx_uint res = MQX_OK;

    for (i = 1; (i < CORTEX_MPU_REC) && (MPU_WORD(i, 3) & MPU_WORD_VLD_MASK); i++) {}

    if (i < CORTEX_MPU_REC) {
        MPU_WORD(i, 0) = (_mem_size)start & MPU_WORD_SRTADDR_MASK;
        MPU_WORD(i, 1) = (_mem_size)end & MPU_WORD_ENDADDR_MASK | 0x01f;
        MPU_WORD(i, 2) = flags;
        MPU_WORD(i, 3) = MPU_WORD_VLD_MASK;
    }
    else
        res = MQX_ERROR;

    return res;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name     : _kinetis_mpu_sw_check
* Returned Value    : uint_32
*       return non zero if any region in mpu contain required range with selected access rights
*       return 0 when mpu don't contain any ragion with requested range with selected access rights
* Comments          :
* 	Software check for memory access (check mpu regions) - this is crutch for ...
*
*END*---------------------------------------------------------------------*/

_mqx_uint _kinetis_mpu_sw_check
    (
        // [IN] cheked block start address
        uint_32 addr,
        // [IN] checked block size
        _mem_size size,
        // [IN] required access flags, direct value - cortex mpu specific
        uint_32 flags
    )
{
    _mqx_uint i, res = 0;

    for (i = 0; i < CORTEX_MPU_REC; i++) {
        if ((MPU_WORD(i, 3) & MPU_WORD_VLD_MASK) && ((addr >= MPU_WORD(i, 0) && addr <= MPU_WORD(i, 1)) || ((addr + size - 1) >= MPU_WORD(i, 0) && (addr + size - 1) <= MPU_WORD(i, 1)))) {
            // founded record, check flags

            // only for core access
            if ((MPU_WORD_M0UM(MPU_WORD(i, 2)) & flags) == flags) {
                res = 1;
                break;
            }
        }
    }

    return res;
}

_mqx_uint _kinetis_mpu_sw_check_mask
    (
        // [IN] cheked block start address
        uint_32 addr,
        // [IN] checked block size
        _mem_size size,
        // [IN] required access flags, direct value - cortex mpu specific
        uint_32 flags,
        uint_32 mask
    )
{
    _mqx_uint i, res = 0, state = 0;

    for (i = 0; i < CORTEX_MPU_REC; i++) {
        if ((MPU_WORD(i, 3) & MPU_WORD_VLD_MASK) && ((addr >= MPU_WORD(i, 0) && addr <= MPU_WORD(i, 1)) || ((addr + size - 1) >= MPU_WORD(i, 0) && (addr + size - 1) <= MPU_WORD(i, 1)))) {
            // founded record, check flags

            state |= MPU_WORD_M0UM(MPU_WORD(i, 2));
        }
    }

    // only for core access
    if ((state & mask) == flags) {
        res = 1;
    }

    return res;
}
