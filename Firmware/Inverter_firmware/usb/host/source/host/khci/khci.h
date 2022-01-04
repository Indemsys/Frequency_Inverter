/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: khci.h$
* $Version : 3.8.13.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file contains KHCI-specific implementations of USB interfaces
*
*END************************************************************************/

#ifndef __khci_h__
#define __khci_h__

/* Basic scheduling packets algorithm, schedules next packet to the beginning of next frame
*/
//#define KHCICFG_BASIC_SCHEDULING

/* Scheduling based on internal "deadtime" register THSLD; you can choose additional value passed to the register
** Note that the register value depends on delay on line from host to device and is experimentally set to 0x65,
** which should be enough for the absolute majority of cases.
*/
#define KHCICFG_THSLD_DELAY 0x65

#endif
