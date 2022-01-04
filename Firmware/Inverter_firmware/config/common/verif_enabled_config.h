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
* $FileName: verif_enabled_config.h$
* $Version : 3.8.7.0$
* $Date    : Sep-23-2011$
*
* Comments:
*
*   MQX configuration set: verification options enabled
*
*END************************************************************************/

#ifndef __verif_enabled_config_h__
#define __verif_enabled_config_h__

#ifndef MQX_CHECK_VALIDITY                
#define MQX_CHECK_VALIDITY                  1
#endif

#ifndef MQX_MONITOR_STACK                 
#define MQX_MONITOR_STACK                   1
#endif

#ifndef MQX_CHECK_ERRORS                  
#define MQX_CHECK_ERRORS                    1
#endif

#ifndef MQX_TASK_CREATION_BLOCKS          
#define MQX_TASK_CREATION_BLOCKS            1
#endif

#ifndef MQX_CHECK_MEMORY_ALLOCATION_ERRORS
#define MQX_CHECK_MEMORY_ALLOCATION_ERRORS  1
#endif

#ifndef MQX_VERIFY_KERNEL_DATA   
#define MQX_VERIFY_KERNEL_DATA              1
#endif

#endif
/* EOF */
