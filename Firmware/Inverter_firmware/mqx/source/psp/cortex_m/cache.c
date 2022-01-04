/**HEADER********************************************************************
*
* Copyright (c) 2008-2009 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2010 Embedded Access Inc.;
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
* $FileName: cache.c$
* $Version : 3.8.3.0$
* $Date    : Jul-30-2012$
*
* Comments:
*
*   This file contains functions for use with the cortex caches.
*
*END************************************************************************/
#include "mqx_inc.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _dcache_enable
* Returned Value   : none
* Comments         :
*   This function enables the data cache
*
*END*----------------------------------------------------------------------*/
void _dcache_enable()
{
    /* Enable cache, enable write buffer, invalidate cache */
    LMEM_PSCCR = (LMEM_PSCCR_GO_MASK | LMEM_PSCCR_INVW1_MASK | LMEM_PSCCR_INVW0_MASK | LMEM_PSCCR_ENWRBUF_MASK | LMEM_PSCCR_ENCACHE_MASK);
    /* wait until the command completes */
    while (LMEM_PSCCR & LMEM_PSCCR_GO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
* Function Name    : _dcache_disable
* Returned Value   : none
* Comments         :
*   This function disables the data cache
*
*END*----------------------------------------------------------------------*/
void _dcache_disable()
{
    LMEM_PSCCR &= ~(LMEM_PSCCR | LMEM_PSCCR_ENWRBUF_MASK | LMEM_PSCCR_ENCACHE_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _dcache_invalidate
* Returned Value   : none
* Comments         :
*   This function invalidates the entire data cache
*
*END*----------------------------------------------------------------------*/
void _dcache_invalidate()
{
    LMEM_PSCCR |= LMEM_PSCCR_GO_MASK | LMEM_PSCCR_INVW0_MASK | LMEM_PSCCR_INVW1_MASK;
    /* wait until the command completes */
    while (LMEM_PSCCR & LMEM_PSCCR_GO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _dcache_invalidate_line
* Comments         :
*    This function is called to invalidate a line
*    out of the data cache.
*
*END*------------------------------------------------------------------------*/
void _dcache_invalidate_line(pointer addr)
{
    /* Invalidate by physical address */
    LMEM_PSCLCR = LMEM_PSCLCR_LADSEL_MASK | (0x1u << LMEM_PSCLCR_LCMD_SHIFT);
    /* Set physical address and activate command */
    LMEM_PSCSAR = ((uint_32)addr & ~0x03u) | LMEM_PSCSAR_LGO_MASK;
    /* wait until the command completes */
    while (LMEM_PSCSAR & LMEM_PSCSAR_LGO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _dcache_invalidate_mlines
* Returned Value   : none
* Comments         :
*
*  This function is called to Invalidate the data cache.
*
*END*-----------------------------------------------------------------------*/
void _dcache_invalidate_mlines(pointer addr, _mem_size length)
{
    pointer end_addr = (pointer)((uint_32)addr + length);
    addr = (pointer) ((uint_32)addr & ~(PSP_CACHE_LINE_SIZE - 1));
    do
    {
        _dcache_invalidate_line(addr);
        addr = (pointer) ((uint_32)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _dcache_flush
* Returned Value   : none
* Comments         :
*   This function enables the data cache
*
*END*----------------------------------------------------------------------*/
void _dcache_flush()
{
    LMEM_PSCCR |= LMEM_PSCCR_GO_MASK |  LMEM_PSCCR_PUSHW0_MASK | LMEM_PSCCR_PUSHW1_MASK ;
    /* wait until the command completes */
    while (LMEM_PSCCR & LMEM_PSCCR_GO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _dcache_flush_line
* Comments         :
*   This function is called to push (flush and invalidate) a line
*   out of the data cache.
*
*END*------------------------------------------------------------------------*/
void _dcache_flush_line(pointer addr)
{
    /* Invalidate by physical address */
    LMEM_PSCLCR = LMEM_PSCLCR_LADSEL_MASK | (0x2u << LMEM_PSCLCR_LCMD_SHIFT);
    /* Set physical address and activate command */
    LMEM_PSCSAR = ((uint_32)addr & ~0x03u) | LMEM_PSCSAR_LGO_MASK;
    /* wait until the command completes */
    while (LMEM_PSCSAR & LMEM_PSCSAR_LGO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _dcache_flush_mlines
* Returned Value   : none
* Comments         :
*
*  This function is called to flush the data cache by performing cache
*  copybacks.  It must determine how many cache lines need to be copied
*  back and then perform the copybacks.
*
*END*-----------------------------------------------------------------------*/
void _dcache_flush_mlines(pointer addr, _mem_size length)
{
    pointer end_addr = (pointer)((uint_32)addr + length);
    addr = (pointer) ((uint_32)addr & ~(PSP_CACHE_LINE_SIZE - 1));
    do
    {
        _dcache_flush_line(addr);
        addr = (pointer) ((uint_32)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _icache_enable
* Returned Value   : none
* Comments         :
*   This function enables the data cache
*
*END*----------------------------------------------------------------------*/
void _icache_enable()
{
    /* Enable cache, enable write buffer, invalidate cache */
    LMEM_PCCCR = (LMEM_PCCCR_GO_MASK | LMEM_PCCCR_INVW1_MASK | LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_ENWRBUF_MASK | LMEM_PCCCR_ENCACHE_MASK);
    /* wait until the command completes */
    while (LMEM_PCCCR & LMEM_PCCCR_GO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _icache_disable
* Returned Value   : none
* Comments         :
*   This function disables the data cache
*
*END*----------------------------------------------------------------------*/
void _icache_disable()
{
    LMEM_PCCCR &= ~(LMEM_PCCCR | LMEM_PCCCR_ENWRBUF_MASK | LMEM_PCCCR_ENCACHE_MASK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _icache_invalidate
* Returned Value   : none
* Comments         :
*   This function enables the data cache
*
*END*----------------------------------------------------------------------*/
void _icache_invalidate()
{
    LMEM_PCCCR |= LMEM_PCCCR_GO_MASK | LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_INVW1_MASK;
    /* wait until the command completes */
    while (LMEM_PCCCR & LMEM_PCCCR_GO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _icache_invalidate_line
* Comments         :
*   This function is called to invalidate a line
*   out of the code cache.
*
*END*------------------------------------------------------------------------*/
void _icache_invalidate_line(pointer addr)
{
    /* Invalidate by physical address */
    LMEM_PCCLCR = LMEM_PCCLCR_LADSEL_MASK | (0x1u << LMEM_PCCLCR_LCMD_SHIFT);
    /* Set physical address and activate command */
    LMEM_PCCSAR = ((uint_32)addr & ~0x03u) | LMEM_PCCSAR_LGO_MASK;
    /* wait until the command completes */
    while (LMEM_PCCSAR & LMEM_PCCSAR_LGO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _icache_invalidate_mlines
* Comments         :
*
*  This function is responsible for performing an instruction cache
*  invalidate.  It must determine how many cache lines need to be invalidated
*  and then perform the invalidation.
*
*END*----------------------------------------------------------------------*/
void _icache_invalidate_mlines(pointer addr, _mem_size length)
{
    pointer end_addr = (pointer)((uint_32)addr + length);
    addr = (pointer) ((uint_32)addr & ~(PSP_CACHE_LINE_SIZE - 1));
    do
    {
        _icache_invalidate_line(addr);
        addr = (pointer) ((uint_32)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _icache_flush
* Returned Value   : none
* Comments         :
*   This function flushes the data cache
*
*END*----------------------------------------------------------------------*/
void _icache_flush()
{
    LMEM_PCCCR |= LMEM_PCCCR_GO_MASK | LMEM_PCCCR_PUSHW0_MASK | LMEM_PCCCR_PUSHW1_MASK;
    /* wait until the command completes */
    while (LMEM_PCCCR & LMEM_PCCCR_GO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _icache_flush_line
* Comments         :
*   This function is called to push (flush and invalidate) a line
*   out of the instruction cache.
*
*END*------------------------------------------------------------------------*/
void _icache_flush_line(pointer addr)
{
    /* Invalidate by physical address */
    LMEM_PCCLCR = LMEM_PCCLCR_LADSEL_MASK | (0x2u << LMEM_PCCLCR_LCMD_SHIFT);
    /* Set physical address and activate command */
    LMEM_PCCSAR = ((uint_32)addr & ~0x03u) | LMEM_PCCSAR_LGO_MASK;
    /* wait until the command completes */
    while (LMEM_PCCSAR & LMEM_PCCSAR_LGO_MASK)
    {}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _icache_flush_mlines
* Returned Value   : none
* Comments         :
*
*  This function is called to flush the data cache by performing cache
*  copybacks.  It must determine how many cache lines need to be copied
*  back and then perform the copybacks.
*
*END*-----------------------------------------------------------------------*/
void _icache_flush_mlines(pointer addr, _mem_size length)
{
    pointer end_addr = (pointer)((uint_32)addr + length);
    addr = (pointer) ((uint_32)addr & ~(PSP_CACHE_LINE_SIZE - 1));
    do
    {
        _icache_flush_line(addr);
        addr = (pointer) ((uint_32)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
}
