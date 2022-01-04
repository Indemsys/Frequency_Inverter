#ifndef __hwtimer_pit_h__
#define __hwtimer_pit_h__
/*HEADER********************************************************************
 *
 * Copyright (c) 2013 Freescale Semiconductor;
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
 * $FileName: hwtimer_pit.h$
 * $Version : 4.0.1$
 * $Date : Jan-18-2013$
 *
 * Comments:
 *
 *   This include file is used to define constants and data types for the
 *   hwtimer low level PIT modul.
 *
 *END************************************************************************/
#include "hwtimer.h"

/*! \brief Instance of HWTIMER_DEVIF_STRUCT structure initialized \n
 * with pointers to API functions the  pit driver implements
 */
extern const HWTIMER_DEVIF_STRUCT pit_devif;

#endif
