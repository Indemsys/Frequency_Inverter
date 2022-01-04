/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2011 Embedded Access Inc
* All Rights Reserved
*
****************************************************************************
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
* $FileName: lwadc_mpxsxx_prv.h$
* $Version : 3.8.2.0$
* $Date    : Sep-17-2012$
*
* Comments:
*
*   CPU specific LWADC driver header file
*
*END************************************************************************/

#ifndef __lwadc_mpxsxx_prv_h__
#define __lwadc_mpxsxx_prv_h__

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern VPXADC_REG_STRUCT_PTR _bsp_get_adc_base_address(uint_32 device);
extern boolean _bsp_adc_enable_access(uint_32 device);
extern uint_32 _bsp_get_adc_channel_mask(uint_32 device);

#ifdef __cplusplus
}
#endif

#endif //__lwadc_mpxsxx_prv_h__
