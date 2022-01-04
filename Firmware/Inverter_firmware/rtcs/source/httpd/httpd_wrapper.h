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
* $FileName: httpd_wrapper.h$
* $Version : 3.7.4.0$
* $Date    : Mar-9-2011$
*
* Comments:
*
*   HTTPD wrapper for os specific functions.
*
*END************************************************************************/


#ifndef HTTPD_WRAPPER_H_
#define HTTPD_WRAPPER_H_

#include "httpd_mqx.h"

#ifndef HTTPD_MALLOC
#error HTTPD_MALLOC is not defined
#endif

#ifndef HTTPD_FREE
#error HTTPD_FREE is not defined
#endif

#ifndef HTTPD_TIME_GET
//#error HTTPD_TIME_GET is not defined
#endif

#ifndef HTTPD_SOCKET_INVALID
#error HTTPD_SOCKET_INVALID is not defined
#endif

#ifndef HTTP_MEMTYPE
#define HTTP_MEMTYPE(ptr,type)
#endif

#endif /* HTTPD_WRAPPER_H_ */
