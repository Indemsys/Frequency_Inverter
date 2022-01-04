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
* $FileName: adc_kadc.h$
* $Version : 3.8.11.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   CPU specific ADC driver header file
*
*END************************************************************************/

#ifndef __adc_kadc_h__
#define __adc_kadc_h__

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE:
** For 13 bit differential mode use ADC_RESOLUTION_12BIT
** For 11 bit differential mode use ADC_RESOLUTION_10BIT
** For 9 bit differential mode use ADC_RESOLUTION_8BIT
*/
#define ADC_RESOLUTION_16BIT    ADC_CFG1_MODE(0x03)
#define ADC_RESOLUTION_12BIT    ADC_CFG1_MODE(0x01)
#define ADC_RESOLUTION_10BIT    ADC_CFG1_MODE(0x02)
#define ADC_RESOLUTION_8BIT     ADC_CFG1_MODE(0x00)
#define ADC_RESOLUTION_DEFAULT (ADC_RESOLUTION_16BIT)

#define ADC_SOURCE_CHANNEL(x)   (x)
#define ADC_SOURCE_MUXSEL_SHIFT (9)
#define ADC_SOURCE_MUXSEL_X     (0x0000) /* none of A / B is forced to use */
#define ADC_SOURCE_MUXSEL_A     (0x0200) /* the A pair is forced to use */
#define ADC_SOURCE_MUXSEL_B     (0x0400) /* the B pair is forced to use */
#define ADC_SOURCE_DIFF         (0x0800)
#define ADC_SOURCE_MODULE_SHIFT (12)
#define ADC_SOURCE_MODULE(x)    (x << ADC_SOURCE_MODULE_SHIFT)
#define ADC_SOURCE_MODULE_ANY   ADC_SOURCE_MODULE(0x00)

#define ADC_GET_CHANNEL(x)  ((x) & 0x001F)
#define ADC_GET_MUXSEL(x)   ((x) & 0x0600)
#define ADC_GET_DIFF(x)     ((x) & 0x0800)
#define ADC_GET_MODULE(x)   ((x) & 0xF000)


#define ADC_PDB_TRIGGER           (0x10000) /* HW specific trigger(s) must be controlled by hardware */

/*
**  IOCTL ADC16 hardware specific commands used in driver
*/
#define ADC_IOCTL_CALIBRATE          _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 0)
#define ADC_IOCTL_SET_CALIBRATION    _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 1)
#define ADC_IOCTL_GET_CALIBRATION    _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 2)
#define ADC_IOCTL_SET_LONG_SAMPLE    _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 3)
#define ADC_IOCTL_SET_SHORT_SAMPLE   _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 4)
#define ADC_IOCTL_SET_HIGH_SPEED     _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 5)
#define ADC_IOCTL_SET_LOW_SPEED      _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 6)
#define ADC_IOCTL_SET_HW_AVERAGING   _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 7)
#define ADC_IOCTL_SET_IDELAY         _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 8)
#define ADC_IOCTL_SET_IDELAYREG      _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 9)
#define ADC_IOCTL_SET_IDELAY_FCN     _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 10)
#define ADC_IOCTL_SET_ERROR_FCN      _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 11)
#define ADC_IOCTL_SET_IDELAY_PROCESS _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 12)
#define ADC_IOCTL_SET_INT_PROCESS    _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 13)
#define ADC_IOCTL_SET_BASE_PERIOD    _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 14)
#define ADC_IOCTL_TRIM_BASE_PERIOD   _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 15)
#define ADC_IOCTL_SET_OFFSET         _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 16)
#define ADC_IOCTL_SET_PLUS_GAIN      _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 17)
#define ADC_IOCTL_SET_MINUS_GAIN     _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 18)
#define ADC_IOCTL_SET_THOLD          _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 19)
#define ADC_IOCTL_SET_THOLD_FCN      _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 20)
#define ADC_IOCTL_SET_DELAYREG       _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 21)
#define ADC_IOCTL_SET_TRIGGER        _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 22)
#define ADC_IOCTL_SET_REFERENCE      _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 23)
#define ADC_IOCTL_SET_PGA_GAIN       _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 24)
#define ADC_IOCTL_GET_PGA_GAIN       _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 25)
#define ADC_IOCTL_ENABLE_CHOPPING    _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 26)
#define ADC_IOCTL_DISABLE_CHOPPING   _IO(IO_TYPE_ADC, ADC_IOCTL_HW_SPECIFIC + 27)

/*
**  ADC16 hardware specific errors
*/
#define ADC_ERROR_PERIOD             (ADC_ERROR_BASE | (ADC_ERROR_HW_SPECIFIC + 0x00)) /* cannot run if basic period was not set */
#define ADC_ERROR_HWTRIGGER          (ADC_ERROR_BASE | (ADC_ERROR_HW_SPECIFIC + 0x01)) /* only HW trigger is supported */
#define ADC_ERROR_NOT_SUPPORTED      (ADC_ERROR_BASE | (ADC_ERROR_HW_SPECIFIC + 0x02)) /* the operation is not supported because of HW limitations */

/*
** kadc16_calib_struct
** Structure used to store ADC16 calibration data
*/
typedef struct kadc_calib_struct
{
    uint_16 OFS;     /*** Offset Correction Register                        ***/
    uint_16 PG;      /*** Plus-Side Gain Register                           ***/
#if ADC_HAS_DIFFERENTIAL_CHANNELS
    uint_16 MG;      /*** Minus-Side Gain Register                          ***/
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */
    uchar   CLPD;    /*** Plus-Side General Calibration Value Register D    ***/
    uchar   CLPS;    /*** Plus-Side General Calibration Value Register S    ***/
    uint_16 CLP4;    /*** Plus-Side General Calibration Value Register 4    ***/
    uint_16 CLP3;    /*** Plus-Side General Calibration Value Register 3    ***/
    uchar   CLP2;    /*** Plus-Side General Calibration Value Register 2    ***/
    uchar   CLP1;    /*** Plus-Side General Calibration Value Register 1    ***/
    uchar   CLP0;    /*** Plus-Side General Calibration Value Register 0    ***/
#if ADC_HAS_DIFFERENTIAL_CHANNELS
    uchar   CLMD;    /*** Minus-Side General Calibration Value Register D   ***/
    uchar   CLMS;    /*** Minus-Side General Calibration Value Register S   ***/
    uint_16 CLM4;    /*** Minus-Side General Calibration Value Register 4   ***/
    uint_16 CLM3;    /*** Minus-Side General Calibration Value Register 3   ***/
    uchar   CLM2;    /*** Minus-Side General Calibration Value Register 2   ***/
    uchar   CLM1;    /*** Minus-Side General Calibration Value Register 1   ***/
    uchar   CLM0;    /*** Minus-Side General Calibration Value Register 0   ***/
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */
} KADC_CALIB_STRUCT;
typedef volatile struct kadc_calib_struct _PTR_ KADC_CALIB_STRUCT_PTR;

typedef enum {
    ADC_CLK_BUSCLK,     /* selects BUSCLK */
    ADC_CLK_BUSCLK2,    /* selects BUSCLK/2 */
    ADC_CLK_BUSCLK_ANY, /* selects BUSCLK or BUSCLK/2 */
    ADC_CLK_ALTERNATE,
    ADC_CLK_ASYNC
} ADC_CLOCK_SOURCE;

typedef enum {
    ADC_DIV_1 = 0,
    ADC_DIV_2 = 1,
    ADC_DIV_4 = 2,
    ADC_DIV_8 = 3,
    ADC_DIV_ANY = -1 /* selects automatically the fastest one */
} ADC_CLOCK_DIV;

typedef enum {
    ADC_LPC_LOW =    ADC_CFG1_ADLPC_MASK, /* use low power of ADC (reduces max. ADCCLK frequency) */
    ADC_LPC_NORMAL = 0                    /* use normal power of ADC */
} ADC_LPC;

typedef enum {
    ADC_HSC_NORMAL = ADC_CFG2_ADHSC_MASK, /* use normal speed */
    ADC_HSC_HIGH = 0                      /* use high speed */
} ADC_HSC;

typedef enum {
    ADC_PDB_COMP1OUT = 0,   /* Comparator 1 output trigger */
    ADC_PDB_COMP2OUT = 4,   /* Comparator 2 output trigger */
    ADC_PDB_EXTRIG   = 16,  /* External event trigger */
    ADC_PDB_SWTRIG   = 28   /* Software trigger */
} ADC_PDB_TRIGSEL;

typedef enum {
    ADC_REF_VREF, /* internal voltage reference */
    ADC_REF_VALT, /* alternate voltage reference */
    ADC_REF_VBG   /* internal bandgap Vbgh and Vbgl */
} ADC_REFERENCE;

#if ADC_HAS_PGA
typedef enum {
    ADC_PGA_GAIN_1   = ADC_PGA_PGAG(0),
    ADC_PGA_GAIN_2   = ADC_PGA_PGAG(1),
    ADC_PGA_GAIN_4   = ADC_PGA_PGAG(2),
    ADC_PGA_GAIN_8   = ADC_PGA_PGAG(3),
    ADC_PGA_GAIN_16  = ADC_PGA_PGAG(4),
    ADC_PGA_GAIN_32  = ADC_PGA_PGAG(5),
    ADC_PGA_GAIN_64  = ADC_PGA_PGAG(6)
} ADC_PGA_GAIN;
#endif /* ADC_HAS_PGA */
/*
** PDB INSTALL_STRUCT
**
** This structure defines the initialization parameters to be used
** for PDB initialization within ADC init
*/
typedef struct kpdb_install_struct
{
    /* PDB interrupt vector */
    uint_32 IDELAY_VECTOR;

    /* PDB interrupt priority */
    uint_32 IDELAY_PRIORITY;

    /* PDB race condition interrupt vector */
    uint_32 ERROR_VECTOR;

    /* PDB race condition interrupt priority */
    uint_32 ERROR_PRIORITY;

} KPDB_INIT_STRUCT, _PTR_ KPDB_INIT_STRUCT_PTR;

/*
** ADC_INSTALL_STRUCT
**
** This structure defines the initialization parameters to be used
** when ADC driver is  port is installed.
*/
typedef struct kadc_install_struct
{
    /* The number of ADC peripheral, use adc_t enum from PSP */
    uint_8 ADC_NUMBER;

    /* The clock source */
    ADC_CLOCK_SOURCE CLOCK_SOURCE;

    /* The clock divisor for ADC */
    ADC_CLOCK_DIV CLOCK_DIV;

    /* ADC high speed control, see ADC_HSC enum */
    ADC_HSC SPEED;

    /* ADC low power control, see ADC_LPC enum */
    ADC_LPC POWER;

    /* The calibration data pointer */
    uint_8_ptr CALIBRATION_DATA_PTR;

    /* ADC interrupt vector */
    uint_32 ADC_VECTOR;

    /* ADC interrupt vector */
    uint_32 ADC_PRIORITY;

    /* KPDB init structure */
    const KPDB_INIT_STRUCT * PDB_INIT;

} KADC_INIT_STRUCT, _PTR_ KADC_INIT_STRUCT_PTR;

typedef void (_CODE_PTR_ PDB_INT_FCN)(void);

pointer  _bsp_get_adc_base_address(_mqx_uint);
pointer  _bsp_get_pdb_base_address(void);
int_16   _pdb_get_prescaler(void);
_mqx_int _pdb_set_prescaler(int_16 prescaler, int_16 multiplier);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
