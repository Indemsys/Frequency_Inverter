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
* $FileName: ipradix.h$
* $Version : 3.5.6.0$
* $Date    : Dec-8-2009$
*
* Comments:
*
*   IP Radix Tree definitions
*
*END************************************************************************/

#ifndef __ipradix_h__
#define __ipradix_h__

/***************************************
**
** Type definitions
**
*/

typedef struct ipradix_node {
   pointer              DATA;    /* must be first field */
   _ip_address          IP;
   _ip_address          MASK;
   struct ipradix_node _PTR_ PARENT;
   struct ipradix_node _PTR_ CHILD[2];
} IPRADIX_NODE, _PTR_ IPRADIX_NODE_PTR;


/***************************************
**
** Prototypes
**
*/

void IPRADIX_init(IPRADIX_NODE_PTR);

void IPRADIX_findbest
(
   IPRADIX_NODE_PTR     root,    /* [IN] the root of the radix tree */
   _ip_address          ip,      /* [IN] the IP address to search for */
   boolean (_CODE_PTR_  test)(pointer, pointer),
                                 /* [IN] application-specific test */
   pointer              data     /* [IN] data for test() */
);

void IPRADIX_insert
(
   IPRADIX_NODE_PTR     root,    /* [IN] the root of the radix tree */
   _ip_address          ip,      /* [IN] the IP address to insert */
   _ip_address          mask,    /* [IN] the IP network mask */
   _rtcs_part           part,    /* [IN] the node partition */
   void (_CODE_PTR_     insert)(pointer _PTR_, pointer),
                                 /* [IN] application-specific insert */
   pointer              data     /* [IN] data for insert() */
);

void IPRADIX_walk
(
   IPRADIX_NODE_PTR     root,    /* [IN] the root of the radix tree */
   boolean (_CODE_PTR_  test)(_ip_address, _ip_address, pointer, pointer),
                                 /* [IN] application-specific test */
   pointer              data     /* [IN] data for test() */
);

#endif
/* EOF */
