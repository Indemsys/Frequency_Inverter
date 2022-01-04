/**HEADER***********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
******************************************************************************** 
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
********************************************************************************
*
* $FileName: init_hw.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains flash boot code to initialize chip selects,
*   disable the watchdog timer and initialize the PLL.
*
*END***************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "bsp_prv.h"


/*FUNCTION*---------------------------------------------------------------------
  Заглушка 
  Вызывается из _io_pccardflexbus_open
*END*-------------------------------------------------------------------------*/
void _bsp_flexbus_pccard_setup (const uint_32 base_address)
{
  // Заглушка 
}



/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : init_hardware
* Returned Value   : void
* Comments         :
*   Initialize Kinetis device.
* 
*   Вызывается из __cmain -> __low_level_init до __iar_data_init3 
*END*-------------------------------------------------------------------------*/

void init_hardware(void)
{

#if PE_LDD_VERSION
    /*  Watch Dog disabled by CPU bean (need to setup in CPU Inspector) */
    __pe_initialize_hardware();
#else
    _bsp_initialize_hardware();
#endif

    _bsp_chip_pins_init(); // Инициализируем все выводы чипа

}
