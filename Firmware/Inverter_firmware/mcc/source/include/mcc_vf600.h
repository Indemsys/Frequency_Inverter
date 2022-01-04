/*HEADER*********************************************************************
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
***************************************************************************
* $FileName: mcc_vf600.h$
* $Version : 3.8.1.0$
* $Date    : Jul-3-2012$
*
* Comments:
*
*   This file contains function prototypes for Vybrid-specific 
*   MCC library functions
*
*END************************************************************************/

#ifndef __MCC_VF600__
#define __MCC_VF600__

#define MCC_VECTOR_NUMBER_INVALID     (0)

unsigned int mcc_get_cpu_to_cpu_vector(unsigned int);
void mcc_clear_cpu_to_cpu_interrupt(unsigned int);
void mcc_triger_cpu_to_cpu_interrupt(void);

#endif /* __MCC_VF600__ */
