#ifndef _at49bvxx_prv_h_
#define _at49bvxx_prv_h_
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
* $FileName: at49bvxx_prv.h$
* $Version : 3.8.1.0$
* $Date    : Oct-4-2011$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for the Atmel AT49BVxx devices
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                     FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

boolean _at49bvxx_program(IO_FLASHX_STRUCT_PTR, char_ptr, char_ptr,_mem_size);
boolean _at49bvxx_sector_erase(IO_FLASHX_STRUCT_PTR, char_ptr, _mem_size);
boolean _at49bvxx_erase_chip(IO_FLASHX_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif //_at49bvxx_prv_h_
/* EOF */
