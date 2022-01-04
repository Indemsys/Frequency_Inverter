/**HEADER********************************************************************
*
* Copyright (c) 2008-2012 Freescale Semiconductor;
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
* $FileName: comp.c$
* $Version : 3.8.4.0$
* $Date    : Oct-3-2012$
*
* Comments:
*
*   This file contains runtime support for the IAR Compiler.
*
*END************************************************************************/

#include "mqx.h"


/* IAR EWARM init function prototypes */
int __low_level_init(void);


pointer malloc(_mem_size);
pointer calloc(_mem_size, _mem_size);
void    free(pointer);
int     exit(int);


extern void init_hardware(void);

#if MQX_ENABLE_CPP || BSPCFG_ENABLE_CPP
extern void __cexit_call_dtors(void);
#endif /* MQX_ENABLE_CPP || BSPCFG_ENABLE_CPP */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : __low_level_init
* Returned Value   : void
* Comments         :
*   Call function init_hardware() in BSP libraty to initialize CPU registers
*
*END*----------------------------------------------------------------------*/

int __low_level_init(void)
{
    /* Initialize device. */
    init_hardware();
    return (1);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : malloc
* Returned Value   : pointer
* Comments         :
*   Override C/C++ runtime heap allocation function in IAR's DLIB
*
*END*----------------------------------------------------------------------*/

pointer malloc(_mem_size bytes)
{
    return _mem_alloc_system(bytes);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : calloc
* Returned Value   : pointer
* Comments         :
*   Override C/C++ runtime heap allocation function in IAR's DLIB
*
*END*----------------------------------------------------------------------*/

pointer calloc(_mem_size n, _mem_size z)
{
    return _mem_alloc_system_zero(n*z);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : free
* Returned Value   : void
* Comments         :
*   Override C/C++ runtime heap deallocation function in IAR's DLIB
*
*END*----------------------------------------------------------------------*/

void free(pointer p)
{
    _mem_free(p);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : exit
* Returned Value   : should not return
* Comments         :
*   Replacement for Codewarrior's exit function
*
*END*----------------------------------------------------------------------*/
#if MQX_EXIT_ENABLED
int exit(int status)
{
#if MQX_ENABLE_CPP || BSPCFG_ENABLE_CPP
    /* Destroy all constructed global objects */
    __cexit_call_dtors();
#endif /* MQX_ENABLE_CPP || BSPCFG_ENABLE_CPP */

    /*
    ** Change for whatever is appropriate for your board
    ** and application.  Perhaps a software reset or
    ** some kind of trap/call to the kernel soft re-boot.
    */
    while (TRUE) {
    }

    return(0);
}

#else  /* MQX_EXIT_ENABLED */

/* int exit(int) is supplied by IAR's DLIB */

#endif /* MQX_EXIT_ENABLED */
/* EOF */
