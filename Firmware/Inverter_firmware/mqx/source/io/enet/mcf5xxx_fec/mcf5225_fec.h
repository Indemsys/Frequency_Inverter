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
* $FileName: mcf5225_fec.h$
* $Version : 3.0.1.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   This file contains the definitions of constants and structures
*   required for the ethernet drivers for the MCF5225x processor
*
*END************************************************************************/
#ifndef _mcf5225_fec_h
#define _mcf5225_fec_h 1

#define MCF5225_FEC0 0

#define MCF5XXX_FEC_DEVICE_COUNT 1
   
#define MCF5XXX_FEC_RX_BUFFER_ALIGNMENT   16  
#define MCF5XXX_FEC_TX_BUFFER_ALIGNMENT   4   
#define MCF5XXX_FEC_BD_ALIGNMENT          16 // may be 4.

// Needs to be here, because a SMALL packet must be a multiple of the RX_BUFFER_ALIGNMENT
#define MCF5XXX_SMALL_PACKET_SIZE         ENET_ALIGN(64,MCF5XXX_FEC_RX_BUFFER_ALIGNMENT)
       
// temporary - these should be generic PSP functions.
#define _psp_set_int_prio_and_level _mcf52xx_int_init
#define _psp_int_mask(x)            _mcf52xx_int_mask(x)

#endif
/* EOF */
