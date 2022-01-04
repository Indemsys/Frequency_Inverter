#ifndef __mqx_dll_h__
#define __mqx_dll_h__
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
* $FileName: mqx_dll.h$
* $Version : 3.8.3.0$
* $Date    : Sep-19-2011$
*
* Comments:
*
*   Doubly-linked list library header file
*
*END************************************************************************/

#include "mqx.h"

typedef struct mqx_dll_node
{
   struct mqx_dll_node _PTR_ next_ptr;
   struct mqx_dll_node _PTR_ prev_ptr;
} MQX_DLL_NODE, _PTR_ MQX_DLL_NODE_PTR;

typedef struct mqx_dll_list
{
   struct mqx_dll_node _PTR_ head_ptr;
   struct mqx_dll_node _PTR_ tail_ptr;
} MQX_DLL_LIST, _PTR_ MQX_DLL_LIST_PTR;

#define _mqx_dll_get_next_node(node_ptr, next_ptr_ptr)   (*(next_ptr_ptr) = (node_ptr)->next_ptr)
#define _mqx_dll_get_prev_node(node_ptr, prev_ptr_ptr)   (*(prev_ptr_ptr) = (node_ptr)->prev_ptr)
#define _mqx_dll_get_head_node(list_ptr, head_ptr_ptr)   (*(head_ptr_ptr) = (list_ptr)->head_ptr)
#define _mqx_dll_get_tail_node(list_ptr, tail_ptr_ptr)   (*(tail_ptr_ptr) = (list_ptr)->tail_ptr)

#define _mqx_dll_is_list_empty(list_ptr)                 ((list_ptr)->head_ptr == 0)
#define _mqx_dll_is_head_node(list_ptr, node_ptr)        ((node_ptr) == (list_ptr)->head_ptr)
#define _mqx_dll_is_tail_node(list_ptr, node_ptr)        ((node_ptr) == (list_ptr)->tail_ptr)
#define _mqx_dll_is_next_node(node_ptr, next_ptr)        ((next_ptr) == (node_ptr)->next_ptr)
#define _mqx_dll_is_prev_node(node_ptr, prev_ptr)        ((prev_ptr) == (node_ptr)->prev_ptr)

#ifdef __cplusplus
extern "C" {
#endif

void _mqx_dll_node_init
(
   MQX_DLL_NODE_PTR node_ptr
);

void _mqx_dll_list_init
(
   MQX_DLL_LIST_PTR list_ptr
);

void _mqx_dll_insert_at_head
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr
);

void _mqx_dll_insert_at_tail
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr
);

void _mqx_dll_insert_after_node
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr,
   MQX_DLL_NODE_PTR next_ptr
);

void _mqx_dll_insert_before_node
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr,
   MQX_DLL_NODE_PTR prev_ptr
);

void _mqx_dll_remove_node
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr
);

void _mqx_dll_create_sublist
(
   MQX_DLL_LIST_PTR  list_ptr,
   MQX_DLL_NODE_PTR  sublist_head_ptr,
   MQX_DLL_NODE_PTR  sublist_tail_ptr,
   MQX_DLL_LIST_PTR  sublist_ptr
);

void _mqx_dll_insert_sublist_at_tail
(
   MQX_DLL_LIST_PTR  sublist_ptr,
   MQX_DLL_LIST_PTR  list_ptr
);

void _mqx_dll_insert_sublist_at_head
(
   MQX_DLL_LIST_PTR  sublist_ptr,
   MQX_DLL_LIST_PTR  list_ptr
);

#ifdef __cplusplus
}
#endif

#endif
