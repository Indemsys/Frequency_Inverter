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
* $FileName: init_adc.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the settings for ADC1 converter
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

/*
** parameters of PDB initialization
*/
const KPDB_INIT_STRUCT _bsp_pdb_init = {
    /* PDB interrupt vector */
    INT_PDB0,
    /* PDB interrupt vector */
    BSP_PDB_VECTOR_PRIORITY,
     /* PDB interrupt vector */
};

/*
** parameters of ADC initialization
*/

const KADC_INIT_STRUCT _bsp_adc0_init = {
    /* The number of ADC peripheral, use adc_t enum from PSP */
    0,
    /* The clock source, selects the best from BUSCLK and BUSCLK/2 */
    ADC_CLK_BUSCLK_ANY,
    /* The clock divisor for ADC. use the fastest one */
    ADC_DIV_ANY,
    /* ADC high speed control, see ADC_HSC enum */
    ADC_HSC_NORMAL,
    /* ADC low power control, see ADC_LPC enum */
    ADC_LPC_NORMAL,
    /* The calibration data pointer */
    NULL,
    /* ADC interrupt vector */
    INT_ADC0,
    /* ADC interrupt vector */
    BSP_ADC0_VECTOR_PRIORITY,
    /* PDB init structure */
    &_bsp_pdb_init
};

/*
** parameters of ADC initialization
*/
const KADC_INIT_STRUCT _bsp_adc1_init = {
    /* The number of ADC peripheral, use adc_t enum from PSP */
    1,
    /* The clock source, selects the best from BUSCLK and BUSCLK/2 */
    ADC_CLK_BUSCLK_ANY,
    /* The clock divisor for ADC. use the fastest one */
    ADC_DIV_ANY,
    /* ADC high speed control, see ADC_HSC enum */
    ADC_HSC_NORMAL,
    /* ADC low power control, see ADC_LPC enum */
    ADC_LPC_NORMAL,
    /* The calibration data pointer */
    NULL,
    /* ADC interrupt vector */
    INT_ADC1,
    /* ADC interrupt vector */
    BSP_ADC1_VECTOR_PRIORITY,
    /* PDB init structure */
    &_bsp_pdb_init
};

const KADC_INIT_STRUCT _bsp_adc2_init = {
    /* The number of ADC peripheral, use adc_t enum from PSP */
    2,
    /* The clock source, selects the best from BUSCLK and BUSCLK/2 */
    ADC_CLK_BUSCLK_ANY,
    /* The clock divisor for ADC. use the fastest one */
    ADC_DIV_ANY,
    /* ADC high speed control, see ADC_HSC enum */
    ADC_HSC_NORMAL,
    /* ADC low power control, see ADC_LPC enum */
    ADC_LPC_NORMAL,
    /* The calibration data pointer */
    NULL,
    /* ADC interrupt vector */
    INT_ADC2,
    /* ADC interrupt vector */
    BSP_ADC2_VECTOR_PRIORITY,
    /* PDB init structure */
    &_bsp_pdb_init
};

const KADC_INIT_STRUCT _bsp_adc3_init = {
    /* The number of ADC peripheral, use adc_t enum from PSP */
    3,
    /* The clock source, selects the best from BUSCLK and BUSCLK/2 */
    ADC_CLK_BUSCLK_ANY,
    /* The clock divisor for ADC. use the fastest one */
    ADC_DIV_ANY,
    /* ADC high speed control, see ADC_HSC enum */
    ADC_HSC_NORMAL,
    /* ADC low power control, see ADC_LPC enum */
    ADC_LPC_NORMAL,
    /* The calibration data pointer */
    NULL,
    /* ADC interrupt vector */
    INT_ADC3,
    /* ADC interrupt vector */
    BSP_ADC3_VECTOR_PRIORITY,
    /* PDB init structure */
    &_bsp_pdb_init
};

/* EOF */
