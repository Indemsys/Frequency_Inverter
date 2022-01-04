/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: rtcsdate.c$
* $Version : 3.6.3.0$
* $Date    : Aug-27-2010$
*
* Comments:
*
*   This file contains the interface to the RTOS
*   date services.
*
*END************************************************************************/

#include <rtcsrtos.h>


/* Factor to convert milliseconds into fraction of a second */
#define SECOND_FRACTION_TO_MILLISECONDS     (0xFFFFFFFF / 1000)

/*
** Time, in secondes, between Jan 1 0h 1900, and Jan 1 0h 1970:
**
** 17 leap years: 17 days + 365 days * 70 years = 25567 days
** 25567 days * 24 hours/day * 60 minutes/hour * 60 seconds/minute
** = 2208988800 seconds
*/
#define SECONDS_1900_1970  2208988800u


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_date_get
* Returned Value   : void
* Comments  :  Gets the current date in seconds and fraction of seconds
*     since 0h Jan 1 1900
*
*END*-----------------------------------------------------------------*/

void RTCS_date_get
   (
      uint_32 _PTR_ seconds,
      /* [OUT] Elapsed seconds since 0h Jan 1 1900 */
      uint_32 _PTR_ fraction
      /* [OUT] Second fraction in 2^-32 second units */
   )
{ /* Body */
   TIME_STRUCT time;

   _time_get(&time);
   *seconds = time.SECONDS + SECONDS_1900_1970;
   *fraction = time.MILLISECONDS * SECOND_FRACTION_TO_MILLISECONDS;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_date_set
* Returned Value   : void
* Comments  :  Sets the current date in seconds and fraction of seconds
*     since 0h Jan 1 1900
*
*END*-----------------------------------------------------------------*/

void RTCS_date_set
   (
      uint_32  seconds,
      /* [IN] Elapsed seconds since 0h Jan 1 1900 */
      uint_32  fraction
      /* [IN] Fraction of a second in 2^-32 second units */
   )
{ /* Body */
   TIME_STRUCT time;

   time.SECONDS = seconds - SECONDS_1900_1970;
   time.MILLISECONDS = fraction / SECOND_FRACTION_TO_MILLISECONDS;
   _time_set(&time);
} /* Endbody */


/* EOF */
