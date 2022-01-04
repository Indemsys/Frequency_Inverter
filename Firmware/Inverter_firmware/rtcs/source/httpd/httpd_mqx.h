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
* $FileName: httpd_mqx.h$
* $Version : 3.7.11.0$
* $Date    : Mar-9-2011$
*
* Comments:
*
*   HTTPD os specific part header.
*
*END************************************************************************/
#ifndef HTTPD_MQX_H_
#define HTTPD_MQX_H_

#include <rtcs.h>

#include <posix.h>
#include <fio.h>
#include <mutex.h>

#define HTTPD_SOCKET_INVALID            RTCS_SOCKET_ERROR

#define HTTPD_MALLOC(size)              _mem_alloc(size)
#define HTTPD_FREE(ptr)                 _mem_free(ptr)
#define HTTPD_MEMZERO(ptr, size)        _mem_zero(ptr, size)
#define HTTPD_MEMTYPE(ptr, type)        _mem_set_type(ptr,type)
#define HTTPD_GET_TIME(time)            _time_get(time)
#define HTTPD_DIFF_TIME_MS(time1, time2)    ((time2.SECONDS * 1000 + time2.MILLISECONDS) - (time1.SECONDS * 1000 + time1.MILLISECONDS))

#define SEEK_SET        IO_SEEK_SET

#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

typedef TIME_STRUCT HTTPD_TIME_STRUCT;
typedef MQX_FILE    HTTPD_FILE;

#endif /* HTTPD_MQX_H_ */
