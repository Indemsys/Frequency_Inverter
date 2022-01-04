/*HEADER*********************************************************************
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
* $FileName: str_utos.c$
* $Version : 3.0.3.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   This file contains the function for converting a _mqx_uint to a string.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "mqx_str.h"

const char _str_hex_array_internal[] = "0123456789ABCDEF";

/*!
 * \brief Converts the _mqx_uint value to a hexadecimal string.
 * 
 * \param[in]  num    The number to convert.
 * \param[out] string The address of the string to write to.
 * 
 * \see _strnlen
 */ 
void _str_mqx_uint_to_hex_string
   (
      register _mqx_uint num,
      register char_ptr  string
   )
{ /* Body */

#if (MQX_INT_SIZE_IN_BITS == 64)
   string[0]  = _str_hex_array_internal[(num >> 60)&0xF];
   string[1]  = _str_hex_array_internal[(num >> 56)&0xF];
   string[2]  = _str_hex_array_internal[(num >> 52)&0xF];
   string[3]  = _str_hex_array_internal[(num >> 48)&0xF];
   string[4]  = _str_hex_array_internal[(num >> 44)&0xF];
   string[5]  = _str_hex_array_internal[(num >> 40)&0xF];
   string[6]  = _str_hex_array_internal[(num >> 36)&0xF];
   string[7]  = _str_hex_array_internal[(num >> 32)&0xF];
   string[8]  = _str_hex_array_internal[(num >> 28)&0xF];
   string[9]  = _str_hex_array_internal[(num >> 24)&0xF];
   string[10] = _str_hex_array_internal[(num >> 20)&0xF];
   string[11] = _str_hex_array_internal[(num >> 16)&0xF];
   string[12] = _str_hex_array_internal[(num >> 12)&0xF];
   string[13] = _str_hex_array_internal[(num >>  8)&0xF];
   string[14] = _str_hex_array_internal[(num >>  4)&0xF];
   string[16] = _str_hex_array_internal[(num      )&0xF];
   string[17] = 0;
#elif (MQX_INT_SIZE_IN_BITS == 32)
   string[0] = _str_hex_array_internal[(num >> 28)&0xF];
   string[1] = _str_hex_array_internal[(num >> 24)&0xF];
   string[2] = _str_hex_array_internal[(num >> 20)&0xF];
   string[3] = _str_hex_array_internal[(num >> 16)&0xF];
   string[4] = _str_hex_array_internal[(num >> 12)&0xF];
   string[5] = _str_hex_array_internal[(num >>  8)&0xF];
   string[6] = _str_hex_array_internal[(num >>  4)&0xF];
   string[7] = _str_hex_array_internal[(num      )&0xF];
   string[8] = 0;
#elif (MQX_INT_SIZE_IN_BITS == 24)
   string[0] = _str_hex_array_internal[(num >> 20)&0xF];
   string[1] = _str_hex_array_internal[(num >> 16)&0xF];
   string[2] = _str_hex_array_internal[(num >> 12)&0xF];
   string[3] = _str_hex_array_internal[(num >>  8)&0xF];
   string[4] = _str_hex_array_internal[(num >>  4)&0xF];
   string[5] = _str_hex_array_internal[(num      )&0xF];
   string[6] = 0;
#elif (MQX_INT_SIZE_IN_BITS == 16)
   string[0] = _str_hex_array_internal[(num >> 12)&0xF];
   string[1] = _str_hex_array_internal[(num >>  8)&0xF];
   string[2] = _str_hex_array_internal[(num >>  4)&0xF];
   string[3] = _str_hex_array_internal[(num      )&0xF];
   string[4] = 0;
#endif

} /* Endbody */

/* EOF */
