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
* $FileName: route.h$
* $Version : 3.5.2.0$
* $Date    : Jan-15-2010$
*
* Comments:
*
*   Definition for the route core functions
*
*END************************************************************************/

#ifndef __route_h__
#define __route_h__

/***************************************
**
** Constants
**
*/


/***************************************
**
** Type definitions
**
*/


/***************************************
**
** Prototypes
**
*/
void ROUTE_register(
   IP_ROUTE_FN_PTR   routefn  /* [IN] list of routing functions */
);

uint_32 ROUTE_new_bindif(
   IP_ROUTE_DIRECT_PTR   bindif   /* [IN] the outgoing binded interface */
);

IP_ROUTE_INDIRECT_PTR ROUTE_get(
   _ip_address    netaddr, /* [IN] the network address */
   _ip_address    netmask  /* [IN] the network mask */
);

uint_32 ROUTE_insert(
   IP_ROUTE_INDIRECT_PTR    gate,    /* [IN] the route entry to insert */
   _ip_address              netaddr, /* [IN] the network address */
   _ip_address              netmask  /* [IN] the network mask */

);

uint_32 ROUTE_remove(
   IP_ROUTE_INDIRECT_PTR    gate     /* [IN] the route entry to insert */
);

#endif
/* EOF */
