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
* $FileName: flash_mpxs20.c$
* $Version : 3.8.3.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the default initialization record for the
*   flash device. The 8313e-rdb uses a Spansion 29lv640 The
*   8323e-rdb uses a Intel Strata 28f128j3.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "flashx.h"
#include "flash_c90.h"
#include "flash_c90_prv.h"


const FLASHX_BLOCK_INFO_STRUCT _mpxs20_block_map[] =
{
    /* count, address, size */

    /* LAS */
    { 1,        0,  0x4000, C90_ASPACE_LAS|C90_WIDTH_128|C90_ALIGN_64 }, /* L0       16 KB */
    { 2,  0x04000,  0xC000, C90_ASPACE_LAS|C90_WIDTH_128|C90_ALIGN_64 }, /* L1..L2 2*48 KB */
    { 1,  0x1C000,  0x4000, C90_ASPACE_LAS|C90_WIDTH_128|C90_ALIGN_64 }, /* L3       16 KB */
    { 2,  0x20000, 0x10000, C90_ASPACE_LAS|C90_WIDTH_128|C90_ALIGN_64 }, /* L4..L5 2*64 KB */

    /* MAS */
    { 2,  0x40000, 0x20000, C90_ASPACE_MAS|C90_WIDTH_128|C90_ALIGN_64 }, /* M0..M1 2*128 KB */

    /* HAS */
    { 2,  0x80000, 0x40000, C90_ASPACE_HAS|C90_WIDTH_128|C90_ALIGN_64 }, /* H0..H1 2*256 KB */

    /* Shadow row */
    { 1, 0xF00000,  0x4000, C90_ASPACE_SHADOW|C90_WIDTH_128|C90_ALIGN_64 }, /* shadow row, 16 KB */

    { 0, 0, 0 }
};


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_c90_address
* Returned Value   : Address upon success
* Comments         :
*    This function returns the base register address of the C90 flash controller.
*
*END*----------------------------------------------------------------------*/

pointer _bsp_get_c90_address (uint_32 base_addr)
{
    return (pointer)MPXS20_FLASH0_BASE;
}
