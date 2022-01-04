/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor
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
* $FileName: psp_supp.c$
* $Version : 3.8.11.0$
* $Date    : Sep-24-2012$
*
* Comments:
*   This file contains the support functions for byte/word and others 
*   manipulations.
*
*END************************************************************************/

#include "mqx_inc.h"

#ifdef __CODEWARRIOR__

asm uint_16 _psp_swap2byte(uint_16 n) {
	rev16 r0, r0
	bx lr
}

asm uint_32 _psp_swap4byte(uint_32 n) {
	rev r0, r0
	bx lr
}

asm void __set_BASEPRI(uint_32 basePri) {
    msr basepri, r0
    bx lr
}

#elif defined(__GNUC__) /* gcc */


/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_swap2byte
 * Returned Value   : uint_16
 * Comments         : Swap 2 bytes
 *   ticks
 *
 *END*----------------------------------------------------------------------*/
 
uint_16 _psp_swap2byte(uint_16 n)
{ /* Body */
    uchar temp[2];

    temp[1] = *((uchar *)&n);
    temp[0] = ((uchar *)&n)[1];
    return (*(uint_16 *)temp);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_swap4byte
 * Returned Value   : uint_32
 * Comments         : Swap 4 bytes
 *   ticks
 *
 *END*----------------------------------------------------------------------*/
 
uint_32 _psp_swap4byte(uint_32 n)
{ /* Body */
    uchar temp[4];

    temp[3] = *((uchar *)&n);
    temp[2] = ((uchar *)&n)[1];
    temp[1] = ((uchar *)&n)[2];
    temp[0] = ((uchar *)&n)[3];
    return (*(uint_32 *)temp);
} /* Endbody */

#elif defined(__CC_ARM) /* Keil */

__asm void __set_BASEPRI(uint_32 basePri)   
{   
  msr basepri, r0   
  bx lr   
}   

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_swap2byte
 * Returned Value   : uint_16
 * Comments         : Swap 2 bytes
 *   ticks
 *
 *END*----------------------------------------------------------------------*/
 
uint_16 _psp_swap2byte(uint_16 n)
{ /* Body */
    uchar temp[2];

    temp[1] = *((uchar *)&n);
    temp[0] = ((uchar *)&n)[1];
    return (*(uint_16 *)temp);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_swap4byte
 * Returned Value   : uint_32
 * Comments         : Swap 4 bytes
 *   ticks
 *
 *END*----------------------------------------------------------------------*/
 
uint_32 _psp_swap4byte(uint_32 n)
{ /* Body */
    uchar temp[4];

    temp[3] = *((uchar *)&n);
    temp[2] = ((uchar *)&n)[1];
    temp[1] = ((uchar *)&n)[2];
    temp[0] = ((uchar *)&n)[3];
    return (*(uint_32 *)temp);
} /* Endbody */

#elif defined(__ICCARM__)

/* IAR EWARM library contains all needed code */

#else /* __CODEWARRIOR__, __ICCARM__ */

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_swap2byte
 * Returned Value   : uint_16
 * Comments         : Swap 2 bytes
 *   ticks
 *
 *END*----------------------------------------------------------------------*/
 
uint_16 _psp_swap2byte(uint_16 n)
{ /* Body */
    uchar temp[2];

    temp[1] = *((uchar *)&n);
    temp[0] = ((uchar *)&n)[1];
    return (*(uint_16 *)temp);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
 * 
 * Function Name    : _psp_swap4byte
 * Returned Value   : uint_32
 * Comments         : Swap 4 bytes
 *   ticks
 *
 *END*----------------------------------------------------------------------*/
 
uint_32 _psp_swap4byte(uint_32 n)
{ /* Body */
    uchar temp[4];

    temp[3] = *((uchar *)&n);
    temp[2] = ((uchar *)&n)[1];
    temp[1] = ((uchar *)&n)[2];
    temp[0] = ((uchar *)&n)[3];
    return (*(uint_32 *)temp);
} /* Endbody */

#endif /* __CODE_WARRIOR__, __ICCARM__ */
