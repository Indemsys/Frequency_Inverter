/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: kinetis_mpu.h$
* $Version : 3.8.4.0$
* $Date    : Aug-30-2011$
*
* Comments:
*
*   This file contains header for basic mpu settings.
*
*END************************************************************************/

#ifndef __kinetis_mpu_h__
#define __kinetis_mpu_h__

#ifdef __cplusplus
extern "C" {
#endif
    
_mqx_uint _kinetis_mpu_init(void);
_mqx_uint _kinetis_mpu_enable(void);
_mqx_uint _kinetis_mpu_disable(void);
_mqx_uint _kinetis_mpu_add_region(uchar_ptr start, uchar_ptr end, _mqx_uint flags);
_mqx_uint _kinetis_mpu_sw_check(uint_32 addr, _mem_size size, uint_32 flags);
_mqx_uint _kinetis_mpu_sw_check_mask(uint_32 addr, _mem_size size, uint_32 flags, uint_32 mask);
    
#ifdef __cplusplus
}
#endif

#endif
