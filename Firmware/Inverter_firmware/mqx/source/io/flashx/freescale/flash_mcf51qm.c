/**HEADER********************************************************************
 * 
 * Copyright (c) 2010 Freescale Semiconductor;
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
 * $FileName: flash_mcf51qm.c$
 * $Version : 3.8.3.0$
 * $Date    : Oct-10-2011$
 *
 * Comments:
 *
 *   The file contains device specific flash functions.
 *
 *END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "flash_ftfl.h"
#include "flash_ftfl_prv.h"
#include "flash_mcf51qm.h"

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _bsp_get_ftfl_address
 * Returned Value   : Address upon success
 * Comments         :
 *    This function returns the base register address of the FTFL.
 *
 *END*----------------------------------------------------------------------*/
pointer _bsp_get_ftfl_address
(
      void
)
{

   return (pointer)(FTFL_BASE_PTR);   

}
