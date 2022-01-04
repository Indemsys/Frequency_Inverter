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
* $FileName: serl_mk60.c$
* $Version : 3.7.2.0$
* $Date    : Feb-7-2011$
*
* Comments:
*
*   This file contains board-specific serial initialization functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "serl_kuart.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_serial_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding UART
*
*END*----------------------------------------------------------------------*/
pointer _bsp_get_serial_base_address(uint_8 dev_num) {
    pointer addr;

    switch(dev_num) {
	case 0:
	    addr = (pointer)UART0_BASE_PTR;
	    break;
	case 1:
	    addr = (pointer)UART1_BASE_PTR;
	    break;
	case 2:
	    addr = (pointer)UART2_BASE_PTR;
	    break;	
	case 3:
	    addr = (pointer)UART3_BASE_PTR;
	    break;	
 	case 4:
	    addr = (pointer)UART4_BASE_PTR;
	    break;	
 	case 5:
	    addr = (pointer)UART5_BASE_PTR;
	    break;	
        default:
	    addr = 0;
    }
    
    return addr;
}

