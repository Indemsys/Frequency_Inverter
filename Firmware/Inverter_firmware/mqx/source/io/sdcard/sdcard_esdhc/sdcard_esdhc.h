#ifndef __sdcard_esdhc_h__
#define __sdcard_esdhc_h__
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
* $FileName: sdcard_esdhc.h$
* $Version : 3.8.3.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   The file contains functions prototypes, defines, structure 
*   definitions private to the SD eSDHC communication.
*
*END************************************************************************/


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/


/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/


/* Internal functions to SD eSDHC communication */

#ifdef __cplusplus
extern "C" {
#endif


extern  boolean _io_sdcard_esdhc_init(MQX_FILE_PTR);
extern _mqx_int _io_sdcard_esdhc_read_block(MQX_FILE_PTR, uchar_ptr, uint_32, uint_32);
extern _mqx_int _io_sdcard_esdhc_write_block(MQX_FILE_PTR, uchar_ptr, uint_32, uint_32);


#ifdef __cplusplus
}
#endif


#endif
