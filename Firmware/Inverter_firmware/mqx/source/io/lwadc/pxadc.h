/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2011 Embedded Access Inc
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
* $FileName: pxadc.h$
* $Version : 3.8.1.0$
* $Date    : Sep-26-2012$
*
* Comments:
*
*   CPU specific ADC driver header file
*
*END************************************************************************/

#ifndef __pxadc_h__
#define __pxadc_h__

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#ifdef __cplusplus
extern "C" {
#endif


#define ADC_MCR_OWREN            0x80000000
#define ADC_MCR_WLSIDE           0x40000000
#define ADC_MCR_MODE_SCAN        0x20000000
#define ADC_MCR_NSTART           0x01000000
#define ADC_MCR_JTRGEN           0x00400000
#define ADC_MCR_JEDGE            0x00200000
#define ADC_MCR_JSTART           0x00100000
#define ADC_MCR_REF_RANGE_EXP    0x00080000
#define ADC_MCR_CTUEN            0x00020000
#define ADC_MCR_STCL             0x00008000
#define ADC_MCR_ADCLKSEL         0x00000100
#define ADC_MCR_ABORT_CHAN       0x00000080
#define ADC_MCR_ABORT            0x00000040
#define ADC_MCR_ACKO             0x00000020
#define ADC_MCR_PWDN             0x00000001

#define ADC_MSR_NSTART           0x01000000
#define ADC_MSR_JABORT           0x00800000
#define ADC_MSR_JSTART           0x00100000
#define ADC_MSR_REF_RANGE        0x00080000
#define ADC_MSR_SELF_TEST_S      0x00040000
#define ADC_MSR_CTUSTART         0x00010000
#define ADC_MSR_CHADDR_MASK      0x0000fe00
#define ADC_MSR_ACKO             0x00000020
#define ADC_MSR_ADCSTATUS_MASK   0x00000007

#define ADC_MSR_ADCSTATUS_IDLE         0x00000000
#define ADC_MSR_ADCSTATUS_POWER_DOWN   0x00000001
#define ADC_MSR_ADCSTATUS_WAIT_STATE   0x00000002
#define ADC_MSR_ADCSTATUS_SAMPLE       0x00000004
#define ADC_MSR_ADCSTATUS_CONVERSION   0x00000006

#define ADC_ISR_REF_RANGE        0x00008000
#define ADC_ISR_EOCTU            0x00000010
#define ADC_ISR_JEOC             0x00000008
#define ADC_ISR_JECH             0x00000004
#define ADC_ISR_EOC              0x00000002
#define ADC_ISR_ECH              0x00000001


#define ADC_IMR_MSK_REF_RANGE        0x00008000
#define ADC_IMR_MSK_EOCTU            0x00000010
#define ADC_IMR_MSK_JEOC             0x00000008
#define ADC_IMR_MSK_JECH             0x00000004
#define ADC_IMR_MSK_EOC              0x00000002
#define ADC_IMR_MSK_ECH              0x00000001

#define ADC_CDR_VALID                0x00080000
#define ADC_CDR_OVERW                0x00040000
#define ADC_CDR_RESULT_MASK          0x00030000
#if (defined BSP_TWRPXD20 || defined BSP_TWRPXD10 || defined BSP_TWRPXN20)
#define ADC_CDR_CDATA_MASK           0x000003FF
#else if (defined BSP_TWRPXS20 || defined BSP_TWRPXS30)
#define ADC_CDR_CDATA_MASK           0x00000FFF
#endif
typedef struct pxadc_reg_struct {
    uint_32 MCR;        /* 0x0000 Main Configuration Register (MCR) R/W 0x0000_0001 on page 11-9 */
    uint_32 MSR;        /* 0x0004 Main Status Register (MSR) R 0x0000_0001 on page 11-11 */
    RESERVED_REGISTER(0x0008, 0x0010);
    uint_32 ISR;        /* 0x0010 Interrupt Status Register (ISR) R/W 0x0000_0000 on page 11-12 # */
    uint_32 CEOCFR0;    /* 0x0014 Channel Pending Register 0 (CEOCFR0) R/W 0x0000_0000 on page 11-13 */
    uint_32 CEOCFR1;    /* 0x0018 Channel Pending Register 0 (CEOCFR0) R/W 0x0000_0000 on page 11-13 */
    uint_32 CEOCFR2;    /* 0x001C Channel Pending Register 0 (CEOCFR0) R/W 0x0000_0000 on page 11-13 */
    uint_32 IMR;        /* 0x0020 Interrupt Mask Register (IMR) R/W 0x0000_0000 on page 11-13 */
    uint_32 CIMR0;      /* 0x0024 Channel Interrupt Mask Register 0 (CIMR0) R/W 0x0000_0000 on page 11-14 */
    uint_32 CIMR1;      /* 0x0028 Channel Interrupt Mask Register 0 (CIMR0) R/W 0x0000_0000 on page 11-14 */
    uint_32 CIMR2;      /* 0x002C Channel Interrupt Mask Register 0 (CIMR0) R/W 0x0000_0000 on page 11-14 */
    uint_32 WTISR;      /* 0x0030 Watchdog Threshold Interrupt Status Register (WTISR) R/W 0x0000_0000 on page 11-15 */
    uint_32 WTIMR;      /* 0x0034 Watchdog Threshold Interrupt Mark Register (WTIMR) R/W 0x0000_0000 on page 11-15 */
    RESERVED_REGISTER(0x0038, 0x0040);
    uint_32 DMAE;       /* 0x0040 DMA Enable Register (DMAE) R/W 0x0000_0000 on page 11-16 */
    uint_32 DMAR0;      /* 0x0044 DMA Channel Select Register 0 (DMAR1) R/W 0x0000_0000 on page 11-16 */
    uint_32 DMAR1;      /* 0x0048 DMA Channel Select Register 0 (DMAR1) R/W 0x0000_0000 on page 11-16 */
    uint_32 DMAR2;      /* 0x004C DMA Channel Select Register 0 (DMAR2) R/W 0x0000_0000 on page 11-16 */
    uint_32 TRC0_3[4];  /* 0x0050 */
    uint_32 THRHLR0_3[4];    // 0x0060 Threshold Register 0 (THRHLR0) R/W 0x0FFF_000 0 on page 11-17 */
    RESERVED_REGISTER(0x0070, 0x0080);
    uint_32 PSCR;       /* 0x0080 */
    uint_32 PSR0;       /* 0x0084 */
    uint_32 PSR1;       /* 0x0088 */
    uint_32 PSR2;       /* 0x008C */
    RESERVED_REGISTER(0x0090, 0x0094);
    uint_32 CTR0;       /* 0x0094 Conversion Timing Register 0 (CTR0) R/W 0x0000_0203 on page 11-19 */
    uint_32 CTR1;       /* 0x0098 Conversion Timing Register 0 (CTR0) R/W 0x0000_0203 on page 11-19 */
    uint_32 CTR2;       /* 0x009C Conversion Timing Register 1 (CTR1) R/W 0x0000_0203 on page 11-21 */
    RESERVED_REGISTER(0x00A0, 0x00A4);
    uint_32 NCMR0;      /* 0x00A4 */
    uint_32 NCMR1;      /* 0x00A8 */
    uint_32 NCMR2;      /* 0x00AC */
    RESERVED_REGISTER(0x00B0, 0x00B4);
    uint_32 JCRM0;      /* 0x00B4 */
    uint_32 JCRM1;      /* 0x00B8 */
    uint_32 JCRM2;      /* 0x00BC */
    uint_32 OFFWR;      /* 0x00C0 */
    uint_32 DSDR;       /* 0x00C4 */
    uint_32 PDEDR;      /* 0x00C8 */
    RESERVED_REGISTER(0x00CC, 0x0100);
    uint_32 CDR[96];
    uint_32 THRHLR4_15[12];    /* 0x0280 Threshold Register 4 (THRHLR4) R/W 0x0FFF_000 0 on page 11-17 */
    uint_32 CWSEL0;     /* 0x02B0 Channel Watchdog Selection Register 0 (CWSEL0) R/W 0x0000_0000 on page 11-24 */
    uint_32 CWSEL1;     /* 0x02B4 Channel Watchdog Selection Register 1 (CWSEL1) R/W 0x0000_0000 on page 11-24 */
    RESERVED_REGISTER(0x02B8, 0x02E0);
    uint_32 CWENR0;     /* 0x02E0 Channel Watchdog Enable Register 0(CWENR0) R/W 0x0000_0000 on page 11-25 */
    RESERVED_REGISTER(0x02E4, 0x02F0);
    uint_32 AWORR0;     /* 0x02F0 Analog Watchdog Out of Range Register 0 (AWORR0) R/W 0x0000_0000 on page 11-26 */
    RESERVED_REGISTER(0x02F4, 0x0340);
    uint_32 STCR1;      /* 0x0340 Self-Test Configuration Register 1 (STCR1) R/W 0x1818_2507 on page 11-26 */
    uint_32 STCR2;      /* 0x0344 Self-Test Configuration Register 2 (STCR2) R/W 0x0000_0005 on page 11-27 */
    uint_32 STCR3;      /* 0x0348 Self-Test Configuration Register 3 (STCR3) R/W 0x0000_0300 on page 11-29 */
    uint_32 STBRR;      /* 0x034C Self-Test Baud Rate Register (STBRR) R/W 0x0005_0000 on page 11-30 */
    uint_32 STSR1;      /* 0x0350 Self-Test Status Register 1 (STSR1) R/W 0x0000_0000 on page 11-31 */
    uint_32 STSR2;      /* 0x0354 Self-Test Status Register 2 (STSR2) R 0x0000_0000 on page 11-33 */
    uint_32 STSR3;      /* 0x0358 Self-Test Status Register 3 (STSR3) R 0x0000_0000 on page 11-33 */
    uint_32 STSR4;      /* 0x035C Self-Test Status Register 4 (STSR4) R 0x0000_0000 on page 11-34–0x036F RESERVED_REGISTER(0x0360, 0x0370); */
    uint_32 STDR1;      /* 0x0370 Self-Test Data Register 1 (STDR1) R 0x0000_0000 on page 11-35 */
    uint_32 STDR2;      /* 0x0374 Self-Test Data Register 2 (STDR2) R 0x0000_0000 on page 11-35 0x0378–0x037F RESERVED_REGISTER(0x0378, 0x0380); */
    uint_32 STAW0R;     /* 0x0380 Self-Test Analog Watchdog Register 0 (STAW0R) R/W 0x0727_04C5 on page 11-36 */
    uint_32 STAW1AR;    /* 0x0384 Self-Test Analog Watchdog Register 1A (STAW1AR) R/W 0x0003_0001 on page 11-37 */
    uint_32 STAW1BR;    /* 0x0388 Self-Test Analog Watchdog Register 1B (STAW1BR) R/W 0x03E8_0ED 0 on page 11-37 */
    uint_32 STAW2R;     /* 0x038C Self-Test Analog Watchdog Register 2 (STAW2R) R/W 0x0000_0FF 9 on page 11-38 */
    uint_32 STAW3R;     /* 0x0390 Self-Test Analog Watchdog Register 3 (STAW3R) R/W 0x086D_079 3 on page 11-38 */
    uint_32 STAW4R;     /* 0x0394 Self-Test Analog Watchdog Register 4 (STAW4R) R/W 0x0828_07D 8 on page 11-39 */
    uint_32 STAW5R;     /* 0x0398 Self-Test Analog Watchdog Register 5 (STAW5R) R/W 0x0010_0010 on page 11-40 */
    RESERVED_REGISTER(0x039C,0x4000);

} PXADC_REG_STRUCT;

typedef volatile struct pxadc_reg_struct * VPXADC_REG_STRUCT_PTR;


#ifdef __cplusplus
}
#endif

#endif

/* EOF */
