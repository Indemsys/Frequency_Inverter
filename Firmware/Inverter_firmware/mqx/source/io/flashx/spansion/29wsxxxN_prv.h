#ifndef _29wsxxxN_prv_h_
#define _29wsxxxN_prv_h_
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
* $FileName: 29wsxxxN_prv.h$
* $Version : 3.8.1.0$
* $Date    : Oct-4-2011$
*
* Comments:
*
*   The file contains functions prototype, defines, structure
*   definitions specific for the Spansion S29ws128N,256N and 512N
*   flash devices.
*   Revision History:
*   Date          Version        Changes
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                     FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

boolean _29wsxxxN_program(IO_FLASHX_STRUCT_PTR, char_ptr, char_ptr, _mem_size);
boolean _29wsxxxN_erase(IO_FLASHX_STRUCT_PTR, char_ptr, _mem_size);
boolean _29wsxxxN_chip_erase(IO_FLASHX_STRUCT_PTR);
boolean _29wsxxxN_test_lock_bits(IO_FLASHX_STRUCT_PTR);
boolean _29wsxxxN_write_protect(IO_FLASHX_STRUCT_PTR, boolean);
_mqx_int _29wsxxxN_ioctl(IO_FLASHX_STRUCT_PTR, _mqx_uint, pointer); 

#ifdef __cplusplus
}
#endif

#endif //_29wsxxxN_prv_h_
/* EOF */
