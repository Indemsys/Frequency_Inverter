/**HEADER**********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
*******************************************************************************
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
*******************************************************************************
*
* $FileName: vectors.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*    This file contains the exception vector table and flash configuration.
*
*END***************************************************************************/

#include "mqx_inc.h"
#include "bsp.h" 

extern unsigned long __BOOT_STACK_ADDRESS[];

/* Configuration values for flash controller */
#define CONFIG_1        (uint_32)0xffffffff
#define CONFIG_2        (uint_32)0xffffffff
#define CONFIG_3        (uint_32)0xffffffff
#define CONFIG_4        (uint_32)0xfffffffe

#define BOOT_START __boot
    
extern void __boot(void);

#ifndef __CC_ARM
extern vector_entry __VECTOR_TABLE_ROM_START[]; /* defined in linker command file */
extern vector_entry __VECTOR_TABLE_RAM_START[]; /* defined in linker command file */
#endif

void _svc_handler(void);
void _pend_svc(void);

#ifdef __ICCARM__
    #pragma language = extended
    #pragma segment  = "CSTACK"
    
    #pragma location = ".flashx"
    #pragma segment  = ".flashx"
    /* Workaround to define __FLASHX_START_ADDR at the beginning of .flashx */
    const uchar __FLASHX_START_ADDR[1];
    /* Here you can put anything to your flashx area */
    
    #if BSPCFG_ENABLE_CFMPROTECT
        #pragma location = ".cfmconfig"
        #pragma segment  = ".cfmconfig"
        const uint_32 _cfm[4] = {
            CONFIG_1,
            CONFIG_2,
            CONFIG_3,
            CONFIG_4
        };
    #endif /* BSPCFG_ENABLE_CFMPROTECT */
#elif defined(__CC_ARM) || defined(__GNUC__)
    /* flashx location defined in io/flashx/freescale/flash_mk60.h for KEIL */
    __attribute__((section(".cfmconfig"))) const uint_32 _cfm[4] __attribute__((used)) = {
        CONFIG_1,
        CONFIG_2,
        CONFIG_3,
        CONFIG_4
    };
#else /* CodeWarrior compiler assumed */
    #pragma define_section flashx    ".flashx" ".flashx" ".flashx"  far_abs R
    /* Here you can put anything to your flashx area */
        
    #if BSPCFG_ENABLE_CFMPROTECT
        /* Pragma to place the flash configuration field on correct location defined in linker file. */
        #pragma define_section cfmconfig ".cfmconfig" ".cfmconfig" ".cfmconfig" far_abs R
        #pragma explicit_zero_data on
        __declspec(cfmconfig)  uint_32 _cfm[4] = {
            CONFIG_1,
            CONFIG_2,
            CONFIG_3,
            CONFIG_4
        };
    #endif /* BSPCFG_ENABLE_CFMPROTECT */
#endif /* CodeWarrior compiler assumed */

typedef union { vector_entry __fun; void * __ptr; } intvec_elem;

#if MQX_ROM_VECTORS
    #define DEFAULT_VECTOR  _int_kernel_isr
#else
    static void __boot_exception(void) {
        while(1);
    }
    
    #define DEFAULT_VECTOR  __boot_exception
    
    #if   defined(__ICCARM__)
        #pragma language = extended
        #pragma location = ".vectors_ram"
        #pragma segment  = ".vectors_ram"
        intvec_elem ram_vector[256] @ ".vectors_ram" =
    #elif defined(__CC_ARM) || defined(__GNUC__)
        __attribute__((section(".vectors_ram"))) vector_entry ram_vector[256] __attribute__((used)) =
    #else /* CodeWarrior compiler assumed */
        #pragma  define_section vectors_ram ".vectors_ram" far_abs RW
        /* 
         * Array for exception vectors in ram + space (6 words)
         * for CW (when CW debugger handles exceptions, using(rewrite)
         * VBR+0x408 address
         */
        __declspec(vectors_ram) vector_entry ram_vector[256 + 6] = 
    #endif /* CodeWarrior compiler assumed */
    {
        (vector_entry)__BOOT_STACK_ADDRESS,
        BOOT_START,         /* 0x01  0x00000004   -   ivINT_Initial_Program_Counter     */
        _int_kernel_isr,    /* 0x02  0x00000008   -   ivINT_NMI                         */
        _int_kernel_isr,    /* 0x03  0x0000000C   -   ivINT_Hard_Fault                  */
        _int_kernel_isr,    /* 0x04  0x00000010   -   ivINT_Mem_Manage_Fault            */
        _int_kernel_isr,    /* 0x05  0x00000014   -   ivINT_Bus_Fault                   */
        _int_kernel_isr,    /* 0x06  0x00000018   -   ivINT_Usage_Fault                 */
        0,                  /* 0x07  0x0000001C   -   ivINT_Reserved7                   */
        0,                  /* 0x08  0x00000020   -   ivINT_Reserved8                   */
        0,                  /* 0x09  0x00000024   -   ivINT_Reserved9                   */
        0,                  /* 0x0A  0x00000028   -   ivINT_Reserved10                  */
        _svc_handler,       /* 0x0B  0x0000002C   -   ivINT_SVCall                      */
        _int_kernel_isr,    /* 0x0C  0x00000030   -   ivINT_DebugMonitor                */
        0,                  /* 0x0D  0x00000034   -   ivINT_Reserved13                  */
        _pend_svc,          /* 0x0E  0x00000038   -   ivINT_PendableSrvReq              */
        _int_kernel_isr     /* 0x0F  0x0000003C   -   ivINT_SysTick                     */
    };
#endif
/*
** exception vector table - this table is really used
*/
#ifdef __ICCARM__
    #pragma language = extended
    #pragma segment  = "CSTACK"

    #pragma location = ".intvec"
    #pragma segment  = ".intvec"
    const intvec_elem __vector_table[] =
#elif defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".vectors_rom"))) const vector_entry __vector_table[256] __attribute__((used)) =
#else /* CodeWarrior compiler assumed */
    #pragma  define_section vectors_rom ".vectors_rom" ".vectors_rom" ".vectors_rom" far_abs R
    __declspec(vectors_rom) vector_entry rom_vector[] = 
#endif /* CodeWarrior compiler assumed */
{
    (vector_entry)__BOOT_STACK_ADDRESS,
    BOOT_START,             /* 0x01  0x00000004   -   ivINT_Initial_Program_Counter     */
    DEFAULT_VECTOR,         /* 0x02  0x00000008   -   ivINT_NMI                         */
    DEFAULT_VECTOR,         /* 0x03  0x0000000C   -   ivINT_Hard_Fault                  */
    DEFAULT_VECTOR,         /* 0x04  0x00000010   -   ivINT_Mem_Manage_Fault            */
    DEFAULT_VECTOR,         /* 0x05  0x00000014   -   ivINT_Bus_Fault                   */
    DEFAULT_VECTOR,         /* 0x06  0x00000018   -   ivINT_Usage_Fault                 */
    0,                      /* 0x07  0x0000001C   -   ivINT_Reserved7                   */
    0,                      /* 0x08  0x00000020   -   ivINT_Reserved8                   */
    0,                      /* 0x09  0x00000024   -   ivINT_Reserved9                   */
    0,                      /* 0x0A  0x00000028   -   ivINT_Reserved10                  */
    _svc_handler,           /* 0x0B  0x0000002C   -   ivINT_SVCall                      */
    DEFAULT_VECTOR,         /* 0x0C  0x00000030   -   ivINT_DebugMonitor                */
    0,                      /* 0x0D  0x00000034   -   ivINT_Reserved13                  */
    _pend_svc,              /* 0x0E  0x00000038   -   ivINT_PendableSrvReq              */
    DEFAULT_VECTOR,         /* 0x0F  0x0000003C   -   ivINT_SysTick                     */
    /* Cortex external interrupt vectors                                                */
    DEFAULT_VECTOR,         /* 0x10  0x00000040   -   ivINT_DMA0                        */
    DEFAULT_VECTOR,         /* 0x11  0x00000044   -   ivINT_DMA1                        */
    DEFAULT_VECTOR,         /* 0x12  0x00000048   -   ivINT_DMA2                        */
    DEFAULT_VECTOR,         /* 0x13  0x0000004C   -   ivINT_DMA3                        */
    DEFAULT_VECTOR,         /* 0x14  0x00000050   -   ivINT_DMA4                        */
    DEFAULT_VECTOR,         /* 0x15  0x00000054   -   ivINT_DMA5                        */
    DEFAULT_VECTOR,         /* 0x16  0x00000058   -   ivINT_DMA6                        */
    DEFAULT_VECTOR,         /* 0x17  0x0000005C   -   ivINT_DMA7                        */
    DEFAULT_VECTOR,         /* 0x18  0x00000060   -   ivINT_DMA8                        */
    DEFAULT_VECTOR,         /* 0x19  0x00000064   -   ivINT_DMA9                        */
    DEFAULT_VECTOR,         /* 0x1A  0x00000068   -   ivINT_DMA10                       */
    DEFAULT_VECTOR,         /* 0x1B  0x0000006C   -   ivINT_DMA11                       */
    DEFAULT_VECTOR,         /* 0x1C  0x00000070   -   ivINT_DMA12                       */
    DEFAULT_VECTOR,         /* 0x1D  0x00000074   -   ivINT_DMA13                       */
    DEFAULT_VECTOR,         /* 0x1E  0x00000078   -   ivINT_DMA14                       */
    DEFAULT_VECTOR,         /* 0x1F  0x0000007C   -   ivINT_DMA15                       */
    DEFAULT_VECTOR,         /* 0x20  0x00000080   -   ivINT_DMA_Error                   */
    DEFAULT_VECTOR,         /* 0x21  0x00000084   -   ivINT_MCM                         */
    DEFAULT_VECTOR,         /* 0x22  0x00000088   -   ivINT_FTFL                        */
    DEFAULT_VECTOR,         /* 0x23  0x0000008C   -   ivINT_Read_Collision              */
    DEFAULT_VECTOR,         /* 0x24  0x00000090   -   ivINT_LVD_LVW                     */
    DEFAULT_VECTOR,         /* 0x25  0x00000094   -   ivINT_LLW                         */
    DEFAULT_VECTOR,         /* 0x26  0x00000098   -   ivINT_Watchdog                    */
    DEFAULT_VECTOR,         /* 0x27  0x0000009C   -   ivINT_RNG                         */
    DEFAULT_VECTOR,         /* 0x28  0x000000A0   -   ivINT_I2C0                        */
    DEFAULT_VECTOR,         /* 0x29  0x000000A4   -   ivINT_I2C1                        */
    DEFAULT_VECTOR,         /* 0x2A  0x000000A8   -   ivINT_SPI0                        */
    DEFAULT_VECTOR,         /* 0x2B  0x000000AC   -   ivINT_SPI1                        */
    DEFAULT_VECTOR,         /* 0x2C  0x000000B0   -   ivINT_SPI2                        */
    DEFAULT_VECTOR,         /* 0x2D  0x000000B4   -   ivINT_CAN0_ORed_Message_buffer    */
    DEFAULT_VECTOR,         /* 0x2E  0x000000B8   -   ivINT_CAN0_Bus_Off                */
    DEFAULT_VECTOR,         /* 0x2F  0x000000BC   -   ivINT_CAN0_Error                  */
    DEFAULT_VECTOR,         /* 0x30  0x000000C0   -   ivINT_CAN0_Tx_Warning             */
    DEFAULT_VECTOR,         /* 0x31  0x000000C4   -   ivINT_CAN0_Rx_Warning             */
    DEFAULT_VECTOR,         /* 0x32  0x000000C8   -   ivINT_CAN0_Wake_Up                */
    DEFAULT_VECTOR,         /* 0x33  0x000000CC   -   ivINT_Reserved51                  */
    DEFAULT_VECTOR,         /* 0x34  0x000000D0   -   ivINT_Reserved52                  */
    DEFAULT_VECTOR,         /* 0x35  0x000000D4   -   ivINT_CAN1_ORed_Message_buffer    */
    DEFAULT_VECTOR,         /* 0x36  0x000000D8   -   ivINT_CAN1_Bus_Off                */
    DEFAULT_VECTOR,         /* 0x37  0x000000DC   -   ivINT_CAN1_Error                  */
    DEFAULT_VECTOR,         /* 0x38  0x000000E0   -   ivINT_CAN1_Tx_Warning             */
    DEFAULT_VECTOR,         /* 0x39  0x000000E4   -   ivINT_CAN1_Rx_Warning             */
    DEFAULT_VECTOR,         /* 0x3A  0x000000E8   -   ivINT_CAN1_Wake_Up                */
    DEFAULT_VECTOR,         /* 0x3B  0x000000EC   -   ivINT_Reserved59                  */
    DEFAULT_VECTOR,         /* 0x3C  0x000000F0   -   ivINT_Reserved60                  */
    DEFAULT_VECTOR,         /* 0x3D  0x000000F4   -   ivINT_UART0_RX_TX                 */
    DEFAULT_VECTOR,         /* 0x3E  0x000000F8   -   ivINT_UART0_ERR                   */
    DEFAULT_VECTOR,         /* 0x3F  0x000000FC   -   ivINT_UART1_RX_TX                 */
    DEFAULT_VECTOR,         /* 0x40  0x00000100   -   ivINT_UART1_ERR                   */
    DEFAULT_VECTOR,         /* 0x41  0x00000104   -   ivINT_UART2_RX_TX                 */
    DEFAULT_VECTOR,         /* 0x42  0x00000108   -   ivINT_UART2_ERR                   */
    DEFAULT_VECTOR,         /* 0x43  0x0000010C   -   ivINT_UART3_RX_TX                 */
    DEFAULT_VECTOR,         /* 0x44  0x00000110   -   ivINT_UART3_ERR                   */
    DEFAULT_VECTOR,         /* 0x45  0x00000114   -   ivINT_UART4_RX_TX                 */
    DEFAULT_VECTOR,         /* 0x46  0x00000118   -   ivINT_UART4_ERR                   */
    DEFAULT_VECTOR,         /* 0x47  0x0000011C   -   ivINT_UART5_RX_TX                 */
    DEFAULT_VECTOR,         /* 0x48  0x00000120   -   ivINT_UART5_ERR                   */
    DEFAULT_VECTOR,         /* 0x49  0x00000124   -   ivINT_ADC0                        */
    DEFAULT_VECTOR,         /* 0x4A  0x00000128   -   ivINT_ADC1                        */
    DEFAULT_VECTOR,         /* 0x4B  0x0000012C   -   ivINT_CMP0                        */
    DEFAULT_VECTOR,         /* 0x4C  0x00000130   -   ivINT_CMP1                        */
    DEFAULT_VECTOR,         /* 0x4D  0x00000134   -   ivINT_CMP2                        */
    DEFAULT_VECTOR,         /* 0x4E  0x00000138   -   ivINT_FTM0                        */
    DEFAULT_VECTOR,         /* 0x4F  0x0000013C   -   ivINT_FTM1                        */
    DEFAULT_VECTOR,         /* 0x50  0x00000140   -   ivINT_FTM2                        */
    DEFAULT_VECTOR,         /* 0x51  0x00000144   -   ivINT_CMT                         */
    DEFAULT_VECTOR,         /* 0x52  0x00000148   -   ivINT_RTC                         */
    DEFAULT_VECTOR,         /* 0x53  0x0000014C   -   ivINT_Reserved83                  */
    DEFAULT_VECTOR,         /* 0x54  0x00000150   -   ivINT_PIT0                        */
    DEFAULT_VECTOR,         /* 0x55  0x00000154   -   ivINT_PIT1                        */
    DEFAULT_VECTOR,         /* 0x56  0x00000158   -   ivINT_PIT2                        */
    DEFAULT_VECTOR,         /* 0x57  0x0000015C   -   ivINT_PIT3                        */
    DEFAULT_VECTOR,         /* 0x58  0x00000160   -   ivINT_PDB0                        */
    DEFAULT_VECTOR,         /* 0x59  0x00000164   -   ivINT_USB0                        */
    DEFAULT_VECTOR,         /* 0x5A  0x00000168   -   ivINT_USBDCD                      */
    DEFAULT_VECTOR,         /* 0x5B  0x0000016C   -   ivINT_ENET_1588_Timer             */
    DEFAULT_VECTOR,         /* 0x5C  0x00000170   -   ivINT_ENET_Transmit               */
    DEFAULT_VECTOR,         /* 0x5D  0x00000174   -   ivINT_ENET_Receive                */
    DEFAULT_VECTOR,         /* 0x5E  0x00000178   -   ivINT_ENET_Error                  */
    DEFAULT_VECTOR,         /* 0x5F  0x0000017C   -   ivINT_I2S0                        */
    DEFAULT_VECTOR,         /* 0x60  0x00000180   -   ivINT_SDHC                        */
    DEFAULT_VECTOR,         /* 0x61  0x00000184   -   ivINT_DAC0                        */
    DEFAULT_VECTOR,         /* 0x62  0x00000188   -   ivINT_DAC1                        */
    DEFAULT_VECTOR,         /* 0x63  0x0000018C   -   ivINT_TSI0                        */
    DEFAULT_VECTOR,         /* 0x64  0x00000190   -   ivINT_MCG                         */
    DEFAULT_VECTOR,         /* 0x65  0x00000194   -   ivINT_LPTimer                     */
    DEFAULT_VECTOR,         /* 0x66  0x00000198   -   ivINT_Reserved102                 */
    DEFAULT_VECTOR,         /* 0x67  0x0000019C   -   ivINT_PORTA                       */
    DEFAULT_VECTOR,         /* 0x68  0x000001A0   -   ivINT_PORTB                       */
    DEFAULT_VECTOR,         /* 0x69  0x000001A4   -   ivINT_PORTC                       */
    DEFAULT_VECTOR,         /* 0x6A  0x000001A8   -   ivINT_PORTD                       */
    DEFAULT_VECTOR,         /* 0x6B  0x000001AC   -   ivINT_PORTE                       */
    DEFAULT_VECTOR,         /* 0x6C  0x000001B0   -   ivINT_Reserved108                 */
    DEFAULT_VECTOR,         /* 0x6D  0x000001B4   -   ivINT_Reserved109                 */
    DEFAULT_VECTOR,         /* 0x6E  0x000001B8   -   ivINT_Reserved110                 */
    DEFAULT_VECTOR,         /* 0x6F  0x000001BC   -   ivINT_Reserved111                 */
    DEFAULT_VECTOR,         /* 0x70  0x000001C0   -   ivINT_Reserved112                 */
    DEFAULT_VECTOR,         /* 0x71  0x000001C4   -   ivINT_Reserved113                 */
    DEFAULT_VECTOR,         /* 0x72  0x000001C8   -   ivINT_Reserved114                 */
    DEFAULT_VECTOR,         /* 0x73  0x000001CC   -   ivINT_Reserved115                 */
    DEFAULT_VECTOR,         /* 0x74  0x000001D0   -   ivINT_Reserved116                 */
    DEFAULT_VECTOR,         /* 0x75  0x000001D4   -   ivINT_Reserved117                 */
    DEFAULT_VECTOR,         /* 0x76  0x000001D8   -   ivINT_Reserved118                 */
    DEFAULT_VECTOR,         /* 0x77  0x000001DC   -   ivINT_Reserved119                 */
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR
};
