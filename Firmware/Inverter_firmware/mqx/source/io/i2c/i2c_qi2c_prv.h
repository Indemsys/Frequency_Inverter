#ifndef _i2c_qi2c_prv_h
#define _i2c_qi2c_prv_h
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
* $FileName: i2c_qi2c_prv.h$
* $Version : 3.8.3.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the I2C drivers for PPC family.
*
*END************************************************************************/



/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** QI2C_INFO_STRUCT
** Run time state information for each serial channel
*/
typedef struct qi2c_info_struct
{
    /* Current initialized values */
    QI2C_INIT_STRUCT                  INIT;

    VQI2C_REG_STRUCT_PTR              I2C_PTR;

    /* The previous interrupt handler and data */
    void                  (_CODE_PTR_ OLD_ISR)(pointer);
    pointer                           OLD_ISR_DATA;

    /* Interrupt vector */
    uint_32                           VECTOR;
   
    /* Actual baudrate table index */
    uint_8                            BAUD_INDEX;

    /* Actual mode */
    uint_8                            MODE;

    /* Actual state */
    uint_8                            STATE;

    /* Destination address */
    uint_8                            ADDRESSEE;
   
   /* Operation flags */
    uint_8                            OPERATION;

    /* Interrupt disabling workaround flag */
    uint_8                            INT_REQUEST;
    /* Number of bytes requested for receive */
    uint_32                           RX_REQUEST;

    /* Pointer to the buffer to use for Tx/Rx data */
    uchar_ptr                         RX_BUFFER;

    /* Rx write index */
    uint_32                           RX_IN;

    /* Rx read index */
    uint_32                           RX_OUT;

    /* Pointer to the buffer to use for current Tx data */
    uchar_ptr                         TX_BUFFER;

    /* Tx write index */
    uint_32                           TX_IN;

    /* Tx read index */
    uint_32                           TX_OUT;

    /* Statistical information */
    I2C_STATISTICS_STRUCT             STATISTICS;

} QI2C_INFO_STRUCT, _PTR_ QI2C_INFO_STRUCT_PTR;

typedef volatile struct qi2c_info_struct * VQI2C_INFO_STRUCT_PTR; 

/*
** QI2C_BAUDRATE_STRUCT
*/
typedef struct qi2c_baudrate_struct
{
    /* Baudrate */
    uint_32                           BAUD_RATE;

    /* Divider */
    uint_8                            IC;

} QI2C_BAUDRATE_STRUCT, _PTR_ QI2C_BAUDRATE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern pointer _bsp_get_qi2c_base_address (uint_8);
extern uint_32 _bsp_get_qi2c_vector (uint_8);

#ifdef __cplusplus
}
#endif


#endif
