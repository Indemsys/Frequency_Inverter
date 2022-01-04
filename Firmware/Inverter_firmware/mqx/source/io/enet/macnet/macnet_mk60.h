/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: macnet_mk60.h$
* $Version : 3.8.10.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the ethernet drivers for the mk60 processor
*
*END************************************************************************/
#ifndef _macnet_mk60_h
#define _macnet_mk60_h

#define MACNET_DEVICE_0                0

#define MACNET_DEVICE_COUNT            1
   
#define MACNET_RX_BUFFER_ALIGNMENT     16  
#define MACNET_TX_BUFFER_ALIGNMENT     16
#define MACNET_BD_ALIGNMENT            16

/* Needs to be here, because a SMALL packet must be a multiple of the RX_BUFFER_ALIGNMENT */
#define MACNET_SMALL_PACKET_SIZE           ENET_ALIGN(64, MACNET_RX_BUFFER_ALIGNMENT)
       
/* MACNET 1588 TIMER the precise 1588timebase is linked to */
#define MACNET_PTP_TIMER  2 

/* EXTAL is used for clocking the 1588 timer */
#define MACNET_1588_CLOCK_SRC (BSP_CLOCK_SRC)

/* Value of the Timer Period Register - should be initialized to 1000000000
   to represent a timer wrap around of one second */
#define MACNET_1588_ATPER_VALUE (1000000000)

/* 1588 timer increment */
#define MACNET_1588_CLOCK_INC (MACNET_1588_ATPER_VALUE / (MACNET_1588_CLOCK_SRC))

#endif /* _macnet_mk60_h */

