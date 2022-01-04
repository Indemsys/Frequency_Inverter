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
* $FileName: mqx_dll.c$
* $Version : 3.8.3.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   This file contains the source for the doubly-linked list library.
*
*END************************************************************************/

#include "mqx_dll.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_node_init
* Returned Value   : void
* Comments         :
*    Constructor for MQX_DLL_NODE structures.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_node_init
(
   MQX_DLL_NODE_PTR node_ptr
)
{
   /* nodes not on a list must not point to nodes on a list */
   node_ptr->next_ptr = 0;
   node_ptr->prev_ptr = 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_list_init
* Returned Value   : void
* Comments         :
*    Constructor for MQX_DLL_LIST structures.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_list_init
(
   MQX_DLL_LIST_PTR list_ptr
)
{
   /* empty lists do not point to any nodes */
   list_ptr->head_ptr = 0;
   list_ptr->tail_ptr = 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_insert_at_head
* Returned Value   : void
* Comments         :
*    Given a list and a node, insert the node at the head of the list.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_insert_at_head
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr
)
{
   /* is list empty? */
   if (list_ptr->head_ptr != 0)
   {
      node_ptr->next_ptr = list_ptr->head_ptr;
      list_ptr->head_ptr->prev_ptr = node_ptr;
   }
   else
   {
      list_ptr->tail_ptr = node_ptr;
   }

   /* no need to zero node_ptr->prev_ptr here */
   list_ptr->head_ptr = node_ptr;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_insert_at_tail
* Returned Value   : void
* Comments         :
*    Given a list and a node, insert the node at the tail of the list.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_insert_at_tail
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr
)
{
   /* is list empty? */
   if (list_ptr->tail_ptr != 0)
   {
      node_ptr->prev_ptr = list_ptr->tail_ptr;
      list_ptr->tail_ptr->next_ptr = node_ptr;
   }
   else
   {
      list_ptr->head_ptr = node_ptr;
   }

   /* no need to zero node_ptr->next_ptr here */   
   list_ptr->tail_ptr = node_ptr;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_insert_after_node
* Returned Value   : void
* Comments         :
*    Given a list, a reference node on the list, and a node to insert, insert
* the node after the reference node.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_insert_after_node
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr,
   MQX_DLL_NODE_PTR next_ptr
)
{
   /* is node_ptr at the tail? */
   if (node_ptr->next_ptr != 0)
   {
      next_ptr->next_ptr = node_ptr->next_ptr;
      node_ptr->next_ptr->prev_ptr = next_ptr;
   }
   else
   {
      /* no need to zero next_ptr->next_ptr here */
      list_ptr->tail_ptr = next_ptr;
   }
   
   node_ptr->next_ptr = next_ptr;
   next_ptr->prev_ptr = node_ptr;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_insert_before_node
* Returned Value   : void
* Comments         :
*    Given a list, a reference node on the list, and a node to insert, insert
* the node before the reference node.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_insert_before_node
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr,
   MQX_DLL_NODE_PTR prev_ptr
)
{
   /* is node_ptr at the head? */
   if (node_ptr->prev_ptr != 0)
   {
      prev_ptr->prev_ptr = node_ptr->prev_ptr;
      node_ptr->prev_ptr->next_ptr = prev_ptr;
   }
   else
   {
      /* no need to zero prev_ptr->prev_ptr here */
      list_ptr->head_ptr = prev_ptr;
   }
   
   node_ptr->prev_ptr = prev_ptr;
   prev_ptr->next_ptr = node_ptr;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_remove_node
* Returned Value   : void
* Comments         :
*    Given a list and a node on the list, remove the node from the list.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_remove_node
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr
)
{
   /* is the node at the tail? */
   if (node_ptr->next_ptr == 0)
   {
      list_ptr->tail_ptr = node_ptr->prev_ptr;
   }
   else
   {
      node_ptr->next_ptr->prev_ptr = node_ptr->prev_ptr;
   }
   
   /* is the node at the head? */
   if (node_ptr->prev_ptr == 0)
   {
      list_ptr->head_ptr = node_ptr->next_ptr;
   }
   else
   {
      node_ptr->prev_ptr->next_ptr = node_ptr->next_ptr;
   }
   
   /* nodes not on a list must not point to nodes on a list */
   node_ptr->next_ptr = 0;
   node_ptr->prev_ptr = 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_create_sublist
* Returned Value   : void
* Comments         :
*    Given a list and a range of consecutive nodes on the list, remove all
* nodes within the range (inclusive) and place them on a new list.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_create_sublist
(
   MQX_DLL_LIST_PTR  list_ptr,
   MQX_DLL_NODE_PTR  sublist_head_ptr,    /* must not be null */
   MQX_DLL_NODE_PTR  sublist_tail_ptr,    /* must not be null */
   MQX_DLL_LIST_PTR  sublist_ptr          /* output */
)
{
   /* create the sublist */
   sublist_ptr->head_ptr = sublist_head_ptr;
   sublist_ptr->tail_ptr = sublist_tail_ptr;
   
   /* update the head pointer if needed */
   if (sublist_head_ptr == list_ptr->head_ptr)
   {
      list_ptr->head_ptr = sublist_tail_ptr->next_ptr;
      
      if (list_ptr->head_ptr != 0)
      {
         list_ptr->head_ptr->prev_ptr = 0;
         sublist_tail_ptr->next_ptr = 0;
      }
      else
      {
         /* update the tail pointer */
         list_ptr->tail_ptr = 0;
      }
   }
   else
   {
      if (sublist_tail_ptr->next_ptr == 0)
      {
         /* update the tail pointer */
         list_ptr->tail_ptr = sublist_head_ptr->prev_ptr;
         list_ptr->tail_ptr->next_ptr = 0;
      }
      else
      {
         /* update the head node's previous pointer */
         sublist_tail_ptr->next_ptr->prev_ptr = sublist_head_ptr->prev_ptr;
         sublist_head_ptr->prev_ptr->next_ptr = sublist_tail_ptr->next_ptr;
         
         sublist_tail_ptr->next_ptr = 0;
      }
      
      sublist_head_ptr->prev_ptr = 0;
   }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_insert_sublist_at_tail
* Returned Value   : void
* Comments         :
*    Given a list and a sublist, remove all nodes on the sublist and insert
* them at the tail of the list.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_insert_sublist_at_tail
(
   MQX_DLL_LIST_PTR  sublist_ptr,
   MQX_DLL_LIST_PTR  list_ptr
)
{
   if (list_ptr->tail_ptr == 0)
   {
      list_ptr->head_ptr = sublist_ptr->head_ptr;
   }
   else
   {
      list_ptr->tail_ptr->next_ptr = sublist_ptr->head_ptr;
      sublist_ptr->head_ptr->prev_ptr = list_ptr->tail_ptr;
   }
   
   list_ptr->tail_ptr = sublist_ptr->tail_ptr;
   
   sublist_ptr->tail_ptr = 0;
   sublist_ptr->head_ptr = 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mqx_dll_insert_sublist_at_head
* Returned Value   : void
* Comments         :
*    Given a list and a sublist, remove all nodes on the sublist and insert
* them at the head of the list.
*
*END*----------------------------------------------------------------------*/
void _mqx_dll_insert_sublist_at_head
(
   MQX_DLL_LIST_PTR  sublist_ptr,
   MQX_DLL_LIST_PTR  list_ptr
)
{
   if (list_ptr->head_ptr == 0)
   {
      list_ptr->tail_ptr = sublist_ptr->tail_ptr;
   }
   else
   {
      list_ptr->head_ptr->prev_ptr = sublist_ptr->tail_ptr;
      sublist_ptr->tail_ptr->next_ptr = list_ptr->head_ptr;
   }
   
   list_ptr->head_ptr = sublist_ptr->head_ptr;
   
   sublist_ptr->tail_ptr = 0;
   sublist_ptr->head_ptr = 0;
}

/* EOF */
