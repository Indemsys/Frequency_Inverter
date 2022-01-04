/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2011 Embedded Access Inc
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
* $FileName: lwadc.h$
* $Version : 3.8.2.0$
* $Date    : Sep-17-2012$
*
* Comments:
*
*   LWADC driver header file
*
*END************************************************************************/
#ifndef __lwadc_h__
#define __lwadc_h__

typedef enum {
    LWADC_RESOLUTION=1,
    LWADC_REFERENCE,
    LWADC_FREQUENCY,
    LWADC_DIVIDER,
    LWADC_DIFFERENTIAL,
    LWADC_POWER_DOWN,
    LWADC_NUMERATOR ,
    LWADC_DENOMINATOR,
    LWADC_FORMAT,
    LWADC_INPUT_CONVERSION_ENABLE
} LWADC_ATTRIBUTE;

#define LWADC_FORMAT_LEFT_JUSTIFIED     1
#define LWADC_FORMAT_RIGHT_JUSTIFIED    2

typedef uint_32 LWADC_VALUE, * LWADC_VALUE_PTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

boolean _lwadc_init( const LWADC_INIT_STRUCT * init_ptr);
boolean _lwadc_init_input( LWADC_STRUCT_PTR lwadc_ptr, uint_32 input);
boolean _lwadc_read_raw( LWADC_STRUCT_PTR lwadc_ptr,  LWADC_VALUE_PTR outSample);
boolean _lwadc_read( LWADC_STRUCT_PTR lwadc_ptr, LWADC_VALUE_PTR outSample);
boolean _lwadc_wait_next( LWADC_STRUCT_PTR lwadc_ptr);
boolean _lwadc_read_average( LWADC_STRUCT_PTR lwadc_ptr, uint_32 num_samples, LWADC_VALUE_PTR outSample);
boolean _lwadc_set_attribute( LWADC_STRUCT_PTR lwadc_ptr, LWADC_ATTRIBUTE attribute, uint_32 value);
boolean _lwadc_get_attribute( LWADC_STRUCT_PTR lwadc_ptr, LWADC_ATTRIBUTE attribute, uint_32_ptr value_ptr);

#ifdef __cplusplus
}
#endif

#endif //__lwadc_h__
