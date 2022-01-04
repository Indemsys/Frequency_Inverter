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
* $FileName: lwgpio_siul.h$
* $Version : 3.8.7.0$
* $Date    : Sep-17-2012$
*
* Comments:
*
*   The file contains definitions used in user program and/or in other
*   kernel modules to access GPIO pins
*
*END************************************************************************/
#ifndef __lwgpio_siul_h__
#define __lwgpio_siul_h__ 1


#define SIUL_PCR_SMC_MASK ((uint_16)(1<<14))
#define SIUL_PCR_APC_MASK ((uint_16)(1<<13))
#define SIUL_PCR_PA_SHIFT 10
#define SIUL_PCR_PA_MASK  ((uint_16)(3<<SIUL_PCR_PA_SHIFT))
#define SIUL_PCR_OBE_MASK ((uint_16)(1<<9))
#define SIUL_PCR_IBE_MASK ((uint_16)(1<<8))
#define SIUL_PCR_DIR_MASK (SIUL_PCR_IBE_MASK | SIUL_PCR_OBE_MASK)
#define SIUL_PCR_ODE_MASK ((uint_16)(1<<5))
#define SIUL_PCR_SRC_MASK ((uint_16)(7<<2))
#define SIUL_PCR_WPE_MASK ((uint_16)(1<<1))
#define SIUL_PCR_WPS_MASK ((uint_16)(1<<0))


/* register access structure */
typedef struct siul_reg_struct {
    uint_32 padding_0;

    uint_32 MIDR1;
    uint_32 MIDR2;

    uint_32 reserved_0C;
    uint_32 reserved_10;

    uint_32 ISR;
    uint_32 IRER;

    uint_32 reserved_1C;
    uint_32 reserved_20;
    uint_32 reserved_24;

    uint_32 IREER;
    uint_32 IFEER;
    uint_32 IFER;

    uint_32 reserved_34;
    uint_32 reserved_38;
    uint_32 reserved_3C;

    /*
    arrays fill whole available space,
    including reserved area following acually implemented registers
    (possible expansion)
    */

    /* access to individual pins */
    uint_16 PCR[608];   
    uint_8  PSMI[256];
    uint_8  GPDO[512];
    uint_8  GPDI[1024];

    /* parallel access */
    uint_32 PGPDO[16];
    uint_32 PGPDI[16];
    uint_32 MPGPDO[224];

    /* interrupt filtering */
    uint_32 IFMC[32];
    uint_32 IFCP;

} SIUL_REG_STRUCT, * SIUL_REG_STRUCT_PTR;

typedef volatile struct siul_reg_struct * VSIUL_REG_STRUCT_PTR;


typedef struct {
    VSIUL_REG_STRUCT_PTR siul_ptr; /* pointer to peripheral registers */
    uint_32 id; /* pin ID */
    int_8 eirq; /* EIRQ assigned to this pin */
} LWGPIO_STRUCT, _PTR_ LWGPIO_STRUCT_PTR;

/* Pin attributes */
#define LWGPIO_ATTR_BASE            (0x100)
#define LWGPIO_ATTR_SLEW_RATE       (LWGPIO_ATTR_BASE+1)

/*attributes value*/
#define LWGPIO_AVAL_SLEW_RATE_FAST      (1)
#define LWGPIO_AVAL_SLEW_RATE_SLOW      (0)

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

pointer _bsp_get_gpio_base_address(void);
boolean _bsp_lwgpio_enable_access(uint_32 device);

int _bsp_siul_pin_to_eirq(uint_32 id);
int _bsp_siul_int_get_vector(uint_32 eirq);

#ifdef __cplusplus
}
#endif

#endif //__lwgpio_siul_h__
