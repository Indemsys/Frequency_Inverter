/**HEADER********************************************************************
*
* Copyright (c) 2008-2011 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2011 Embedded Access Inc.;
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
* $FileName: sema4_vybrid.c$
* $Version : 3.8.1.0$
* $Date    : Sep-25-2012$
*
* Comments:
*
*   This file contains the MPXS30 specific driver functions for the SEMA4.
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>



static const pointer sema4_address[] = { (pointer)SEMA4_BASE_PTR };

#if PSP_MQX_CPU_IS_VYBRID_A5
static const uint_32 sema4_vector[] = { INT_Directed_CA5_int0 };
#else
static const uint_32 sema4_vector[] = { 20 };
#endif

SEMA4_MemMapPtr _bsp_get_sema4_base_address(uint_32 device)
{
   if (device < ELEMENTS_OF(sema4_address)) {
      return (SEMA4_MemMapPtr) sema4_address[device];
   }
   return NULL;
}

uint_32 _bsp_get_sema4_vector(uint_32 device)
{
   if (device < ELEMENTS_OF(sema4_vector)) {
      return  sema4_vector[device];
   }
   return 0;
}

