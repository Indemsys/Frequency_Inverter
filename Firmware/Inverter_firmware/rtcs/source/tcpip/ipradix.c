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
* $FileName: ipradix.c$
* $Version : 3.8.4.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the implementation of the IP
*   radix tree.
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP4 

#include "rtcs_prv.h"
#include "tcpip.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IPRADIX_init
* Returned Value  : void
* Comments  :  Initializes the radix tree
*
*END*-----------------------------------------------------------------*/

void IPRADIX_init
   (
      IPRADIX_NODE_PTR  root
         /* [IN/OUT] the root of the radix tree */
   )
{ /* Body */

   root->IP       = 0x00000000;
   root->MASK     = 0x00000000;
   root->PARENT   = NULL;
   root->CHILD[0] = NULL;
   root->CHILD[1] = NULL;
   root->DATA     = NULL;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IPRADIX_findbest
* Returned Value  : void
* Comments  :  Searches the radix tree for a best IP network match
*
*END*-----------------------------------------------------------------*/

void IPRADIX_findbest
   (
      IPRADIX_NODE_PTR     root,    /* [IN] the root of the radix tree */
      _ip_address          ip,      /* [IN] the IP address to find */
      boolean (_CODE_PTR_  test)(pointer, pointer),
                                    /* [IN] application-specific filter */
      pointer              data     /* [IN] data for test() */
   )
{ /* Body */
   IPRADIX_NODE_PTR  node = root;
   IPRADIX_NODE_PTR  nextnode;
   _ip_address       bit;
   uint_32           child;

   /* First find the best match */
   for (;; node = nextnode) {
      bit = (~node->MASK >> 1) + 1;
      child = (ip & bit) ? 1 : 0;
      nextnode = node->CHILD[child];
      if (!nextnode) break;
      if ((ip & nextnode->MASK) != nextnode->IP) break;
   } /* Endfor */

   /* Now backtrack until we find a node that test() accepts */
   for (; node; node = node->PARENT) {
      if (node->DATA && test(node->DATA, data)) break;
   } /* Endfor */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IPRADIX_insert
* Returned Value  : void
* Comments  :  Searches the radix tree for an exact IP network match,
*              and if not found, creates a new node.
*
*END*-----------------------------------------------------------------*/

void IPRADIX_insert
   (
      IPRADIX_NODE_PTR     root,    /* [IN] the root of the radix tree */
      _ip_address          ip,      /* [IN] the IP address to insert */
      _ip_address          mask,    /* [IN] the IP network mask */
      _rtcs_part           part,    /* [IN] the node partition */
      void (_CODE_PTR_     insert)(pointer _PTR_, pointer),
                                    /* [IN] application-specific insert */
      pointer              data     /* [IN] data for insert() */
   )
{ /* Body */
   IPRADIX_NODE_PTR  node = root;
   IPRADIX_NODE_PTR  nextnode, newnode;
   _ip_address       bit;
   _ip_address       newmask;
   uint_32           child;

   /* First find the best match */
   for (;; node = nextnode) {
      bit = (~node->MASK >> 1) + 1;
      child = (ip & bit) ? 1 : 0;
      nextnode = node->CHILD[child];
      if (!nextnode) break;
      if (mask < nextnode->MASK) break;
      if ((ip & nextnode->MASK) != nextnode->IP) break;
   } /* Endfor */

   if (part) {

      /* The best match has a child that isn't a match.  Insert new node */
      if ((mask > node->MASK) && nextnode) {
         newmask = ~mask | (ip ^ nextnode->IP);
         newmask |= newmask >> 1;
         newmask |= newmask >> 2;
         newmask |= newmask >> 4;
         newmask |= newmask >> 8;
         newmask |= newmask >> 16;
         bit = (newmask >> 1) + 1;
         newmask = ~newmask;
         newnode = RTCS_part_alloc(part);
         if (!newnode) return;
         newnode->IP     = ip & newmask;
         newnode->MASK   = newmask;
         newnode->PARENT = node;
         if (nextnode->IP & bit) {
            newnode->CHILD[0] = NULL;
            newnode->CHILD[1] = nextnode;
         } else {
            newnode->CHILD[0] = nextnode;
            newnode->CHILD[1] = NULL;
         } /* Endif */
         newnode->DATA = NULL;
         node->CHILD[child] = newnode;
         nextnode->PARENT = newnode;
         node = newnode;
         child = (ip & bit) ? 1 : 0;
      } /* Endif */

      /* Now the best match has a NULL child.  Create new leaf */
      if (mask > node->MASK) {
         newnode = RTCS_part_alloc(part);
         if (newnode) {
            newnode->IP       = ip;
            newnode->MASK     = mask;
            newnode->PARENT   = node;
            newnode->CHILD[0] = NULL;
            newnode->CHILD[1] = NULL;
            newnode->DATA     = NULL;
            node->CHILD[child] = newnode;
            node = newnode;
         } /* Endif */
      } /* Endif */

   } /* Endif */

   /* If we have an exact match, call insert() */
   if (mask == node->MASK) {
      insert(&node->DATA, data);
   } /* Endif */

   /* insert() may have deleted DATA, so delete unnecessary node(s) */
   for (; node; node = nextnode) {
      if (node->DATA) break;
      nextnode = node->PARENT;
      if (!nextnode) break;
      if (node->CHILD[0] && node->CHILD[1]) break;
      child = (node == nextnode->CHILD[0]) ? 0 : 1;
      if (node->CHILD[0]) {
         nextnode->CHILD[child] = node->CHILD[0];
         node->CHILD[0]->PARENT = nextnode;
      } else if (node->CHILD[1]) {
         nextnode->CHILD[child] = node->CHILD[1];
         node->CHILD[1]->PARENT = nextnode;
      } else {
         nextnode->CHILD[child] = NULL;
      } /* Endif */
      RTCS_part_free(node);
   } /* Endfor */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : IPRADIX_walk
* Returned Value  : void
* Comments  :  Traverses the entire radix tree.
*
*END*-----------------------------------------------------------------*/

void IPRADIX_walk
   (
      IPRADIX_NODE_PTR     root,    /* [IN] the root of the radix tree */
      boolean (_CODE_PTR_  test)(_ip_address, _ip_address, pointer, pointer),
                                    /* [IN] application-specific test */
      pointer              data     /* [IN] data for find() */
   )
{ /* Body */
   IPRADIX_NODE_PTR  node = root, lastnode = NULL;
   uint_32           next;

   for (; node;) {
      if (lastnode == node->PARENT) {
         if (test(node->IP, node->MASK, node->DATA, data)) break;
         if (node->CHILD[0]) {
            next = 0;
         } else if (node->CHILD[1]) {
            next = 1;
         } else {
            next = 2;
         } /* Endif */
      } else if (lastnode == node->CHILD[0]) {
         if (node->CHILD[1]) {
            next = 1;
         } else {
            next = 2;
         } /* Endif */
      } else {
            next = 2;
      } /* Endif */
      lastnode = node;
      switch (next) {
      case 0: node = node->CHILD[0]; break;
      case 1: node = node->CHILD[1]; break;
      case 2: node = node->PARENT;   break;
      } /* Endswitch */
   } /* Endfor */

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/* EOF */
