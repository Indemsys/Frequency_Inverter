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
* $FileName: core_mutex.c$
* $Version : 3.8.4.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*   This file contains the source functions for functions required to
*   control the PBRIDGE.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include <core_mutex.h>

static CORE_MUTEX_COMPONENT_PTR _core_mutex_component_ptr = NULL;

/*FUNCTION****************************************************************
*
* Function Name    : _core_mutext_get_component_ptr
* Returned Value   : CORE_MUTEX_COMPONENT_PTR
* Comments         :
*    This function gets the core mutex component pointer.
*
*END*********************************************************************/

CORE_MUTEX_COMPONENT_PTR _core_mutext_get_component_ptr(void)
{
   return _core_mutex_component_ptr;
}


/*FUNCTION****************************************************************
*
* Function Name    : _core_mutext_set_component_ptr
* Returned Value   :
* Comments         :
*    This function sets the core mutex component pointer.
*
*END*********************************************************************/

void _core_mutext_set_component_ptr(CORE_MUTEX_COMPONENT_PTR p)
{
   _core_mutex_component_ptr=p;
}
