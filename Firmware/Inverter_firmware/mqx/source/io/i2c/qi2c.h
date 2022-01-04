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
* $FileName: qi2c.h$
* $Version : 3.8.4.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the type definitions for the mcf54xx I2C module.
*
*END************************************************************************/

#ifndef __qi2c_h__
#define __qi2c_h__

#define __qi2c_h__version "$Version:3.8.4.0$"
#define __qi2c_h__date    "$Date:Jun-6-2012$"

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------------------------*/
/*
** I2C registers
*/

/* Bit definitions and macros for QI2C_I2ADR */
#define QI2C_I2ADR_ADR(x)                 (((x)&0x7F)<<0x1)

/* Bit definitions and macros for QI2C_I2FDR */
#define QI2C_I2FDR_IC(x)                  (((x)&0x3F)<<0)

/* Bit definitions and macros for QI2C_I2CR */
#define QI2C_I2CR_DMAEN                   (0x2)
#define QI2C_I2CR_RSTA                    (0x4)
#define QI2C_I2CR_NOAK                    (0x8)
#define QI2C_I2CR_TX                      (0x10)
#define QI2C_I2CR_MS                      (0x20)
#define QI2C_I2CR_IBIE                    (0x40)
#define QI2C_I2CR_MDIS                    (0x80)

/* Bit definitions and macros for QI2C_I2SR */
#define QI2C_I2SR_RXAK                    (0x1)
#define QI2C_I2SR_IBIF                    (0x2)
#define QI2C_I2SR_SRW                     (0x4)
#define QI2C_I2SR_IBAL                    (0x10)
#define QI2C_I2SR_IBB                     (0x20)
#define QI2C_I2SR_IAAS                    (0x40)
#define QI2C_I2SR_TCF                     (0x80)


/* Bit definitions and macros for QI2C_I2IC */
#define QI2C_I2IC_BIIE                    (0x80)

#define QI2C_REG(r) uchar r


/*------------------------------------------------------------------------*/


/*
** QI2C_STRUCT
** This structure defines what one i2c register look like
*/

typedef struct qi2c_reg_struct
{
    QI2C_REG(I2ADR);          // I2C address register
    QI2C_REG(I2FDR);          // I2C frequency divider register
    QI2C_REG(I2CR);           // I2C control register
    QI2C_REG(I2SR);           // I2C status register
    QI2C_REG(I2DR);           // I2C data I/O register
    QI2C_REG(I2IC);           // I2C Interrupt Configuration

} QI2C_REG_STRUCT;
typedef volatile struct qi2c_reg_struct _PTR_ VQI2C_REG_STRUCT_PTR;

#ifdef __cplusplus
}
#endif

#endif  //__qi2c_h__
