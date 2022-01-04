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
* $FileName: httpd_cnfg.h$
* $Version : 3.8.26.0$
* $Date    : Jul-2-2012$
*
* Comments:
*
*   HTTPD default configuration.
*
*END************************************************************************/

#ifndef HTTPD_CNFG_H_
#define HTTPD_CNFG_H_

#include "user_config.h"
#include "rtcscfg.h"

#if (HTTPDCFG_POLL_MODE + HTTPDCFG_STATIC_TASKS + HTTPDCFG_DYNAMIC_TASKS) != 1
#error Define one of HTTPDCFG_POLL_MODE or HTTPDCFG_STATIC_TASK or HTTPDCFG_DYNAMIC_TASK !
#endif

#endif // HTTPD_CNFG_H_
