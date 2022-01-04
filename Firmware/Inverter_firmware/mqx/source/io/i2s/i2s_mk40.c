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
* $FileName: i2s_mk40.c$
* $Version : 3.8.4.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains board-specific I2S initialization functions.
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include "i2s.h"
#include "i2s_ki2s.h"
#include "i2s_ki2s_prv.h"

/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_i2s_base_address
* Returned Value   : address if successful, NULL otherwise
* Comments         :
*    This function returns the base register address of the corresponding I2S device.
*
*END*********************************************************************/

pointer _bsp_get_i2s_base_address
(
   uint_8 dev_num
)
{
   pointer addr;

   switch (dev_num)
   {
      case 0:
         addr = (pointer) I2S0_BASE_PTR;
         break;
      default:
        addr = NULL;
        break;
    }
    return addr;
}


/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_i2s_vector
* Returned Value   : vector number if successful, 0 otherwise
* Comments         :
*    This function returns desired interrupt vector number for specified I2S device.
*
*END*********************************************************************/

uint_32 _bsp_get_i2s_vector
(
   uint_8 dev_num
)
{
   uint_32 vector;
   switch (dev_num)
   {
      case 0:
         vector = INT_I2S0;
         break;
      default:
         vector = 0;
         break;
    }
    return vector;
}
/* EOF */
