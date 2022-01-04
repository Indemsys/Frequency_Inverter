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
* $FileName: enstat.c$
* $Version : 3.0.5.0$
* $Date    : Jun-23-2009$
*
* Comments:
*
*   This file contains the ENET_get_stats utility
*   function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "enet.h"
#include "enetprv.h"

#if BSPCFG_ENABLE_ENET_STATS
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ENET_get_stats
*  Returned Value : pointer to the statistics structure
*  Comments       :
*        Retrieves the Ethernet statistics for an initialized device.
*
*END*-----------------------------------------------------------------*/

ENET_STATS_PTR ENET_get_stats
   (
      /* [IN] the Ethernet state structure */
      _enet_handle   handle
   )
{
   return (ENET_STATS_PTR)&((ENET_CONTEXT_STRUCT_PTR)handle)->STATS;
} 

#endif
/* EOF */
