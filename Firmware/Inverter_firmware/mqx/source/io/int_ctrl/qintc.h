/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: qintc.h$
* $Version : 3.8.3.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the definitions for the INTC module.
*
*END************************************************************************/


#ifndef __qintc_h__
#define __qintc_h__


/* INTC */
#define QINTC_BCR_VTES     (1<<5)
#define QINTC_BCR_HVEN     (1<<0)

#define QINTC_CPR_PRI      (0x0000000F)

#define QINTC_IACKR_VTBA   (0xFFFFF800)
#define QINTC_IACKR_INTVEC (0x000007FC)

#define QINTC_SSCIR_SET    (1<<1)
#define QINTC_SSCIR_CLR    (1<<0)

#define QINTC_PSR_PRI      (0x0000000F)

/* PRI values */
#define QINTC_PRI_MAX      (15) /* Priority 15 (highest) */
#define QINTC_PRI_MIN      (0)  /* Priority 0 (lowest) */


/* Register access */
typedef struct qintc_reg_struct
{
    uint_32 BCR;        /* block configuration register */
    uint_32 R_0x08;
    uint_32 CPR;        /* current priority register */
    uint_32 R_0x0C;
    uint_32 IACKR;      /* interrupt acknowledge register */
    uint_32 R_0x14;
    uint_32 EOIR;       /* end-of-interrupt register */
    uint_32 R_0x1C;
    uint_8  SSCIR[8];   /* software set/clear interrupt registers */
    uint_32 R_0x28[6];
    uint_8  PSR[256];   /* priority select registers */

} QINTC_REG_STRUCT;

typedef volatile struct qintc_reg_struct * VQINTC_REG_STRUCT_PTR;


#endif
