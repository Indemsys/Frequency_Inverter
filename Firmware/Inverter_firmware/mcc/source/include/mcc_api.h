/*HEADER*********************************************************************
* 
* Copyright (c) 2013 Freescale Semiconductor;
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
***************************************************************************
* $FileName: mcc_api.h$
* $Version : 3.8.1.0$
* $Date    : Jul-3-2012$
*
* Comments:
*
*   This file contains prototypes for MCC library API functions
*
*END************************************************************************/

#ifndef __MCC_API__
#define __MCC_API__

int mcc_initialize(MCC_NODE);
int mcc_destroy(MCC_NODE);
int mcc_create_endpoint(MCC_ENDPOINT*, MCC_PORT);
int mcc_destroy_endpoint(MCC_ENDPOINT*);
int mcc_send(MCC_ENDPOINT*, void*, MCC_MEM_SIZE, unsigned int);
int mcc_recv_copy(MCC_ENDPOINT*, void*, MCC_MEM_SIZE, MCC_MEM_SIZE*, unsigned int);
int mcc_recv_nocopy(MCC_ENDPOINT*, void**, MCC_MEM_SIZE*, unsigned int);
int mcc_msgs_available(MCC_ENDPOINT*, unsigned int*);
int mcc_free_buffer(void*);
int mcc_get_info(MCC_NODE, MCC_INFO_STRUCT*);

#endif /* __MCC_API__ */
