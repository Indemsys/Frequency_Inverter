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
* $FileName: usb_misc_prv.h$
* $Version : 3.8.2.0$
* $Date    : Jul-30-2012$
*
* Comments:
*
*   This file contains USB Device API defines for state and function
*   returns.
*
*END************************************************************************/
#ifndef __usb_misc_prv_h__
#define __usb_misc_prv_h__ 1

#define USB_lock()                           _int_disable()
#define USB_unlock()                         _int_enable()
#define USB_install_isr(vector, isr, data)   _int_install_isr(vector, isr, data)

#if PSP_HAS_DATA_CACHE
// Not needed if we are allocating everything out of uncached memory
#define USB_dcache_invalidate()              _DCACHE_INVALIDATE()
#define USB_dcache_invalidate_line(p)        _DCACHE_INVALIDATE_LINE(p)
/* Based on the targets it should be modified, for coldfire it is MBYTES */
#define USB_dcache_invalidate_mlines(p,n)    _DCACHE_INVALIDATE_MBYTES(p, n)
#define USB_dcache_flush_line(p)             _DCACHE_FLUSH_LINE(p)
/* Based on the targets it should be modified, for coldfire it is MBYTES */
#define USB_dcache_flush_mlines(p,n)         _DCACHE_FLUSH_MBYTES(p, n)

#else

#define USB_dcache_invalidate()
#define USB_dcache_invalidate_line(p)
/* Based on the targets it should be modified, for coldfire it is MBYTES */
#define USB_dcache_invalidate_mlines(p,n)
#define USB_dcache_flush_line(p)
/* Based on the targets it should be modified, for coldfire it is MBYTES */
#define USB_dcache_flush_mlines(p,n)

#endif // PSP_HAS_DATA_CACHE

#if PSP_HAS_DATA_CACHE
    #define USB_mem_alloc_uncached(n)         _mem_alloc_system_uncached(n)
    #define USB_mem_alloc_uncached_zero(n)    _mem_alloc_system_zero_uncached(n)
    #define USB_mem_alloc_align_uncached(n,a) _mem_alloc_align_uncached(n, a)
#else
    #define USB_mem_alloc_uncached(n)         _mem_alloc_system(n)
    #define USB_mem_alloc_uncached_zero(n)    _mem_alloc_system_zero(n)
    #define USB_mem_alloc_align_uncached(n,a) _mem_alloc_align(n, a)
#endif // PSP_HAS_DATA_CACHE

#define USB_mem_alloc(n)                      _mem_alloc_system(n)
#define USB_mem_alloc_zero(n)                 _mem_alloc_system_zero(n)
#define USB_mem_free(ptr)                     _mem_free(ptr)
#define USB_mem_zero(ptr,n)                   _mem_zero(ptr,n)
#define USB_mem_copy(src,dst,n)               _mem_copy(src,dst,n)

#define USB_uncached                          volatile

#endif
