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
* $FileName: mcc_config.h$
* $Version : 3.8.1.0$
* $Date    : Jul-3-2012$
*
* Comments:
*
*   This is the main MCC configuration file
*
*END************************************************************************/

#ifndef __MCC_CONFIG__
#define __MCC_CONFIG__

/* used OS */
#define MCC_OS_USED                    (MCC_MQX)

/* base address of shared memory */
#define MCC_BASE_ADDRESS               (BSP_SHARED_RAM_START)

/* size (in bytes) and number of receive buffers */
#define MCC_ATTR_NUM_RECEIVE_BUFFERS   (10)
#define MCC_ATTR_BUFFER_SIZE_IN_BYTES  (1024)

/* maximum number of receive endpoints (application specific setting),
 * do not assign it to a value greater than 255 ! */
#define MCC_ATTR_MAX_RECEIVE_ENDPOINTS (5)

/* size of the signal queue */
#define MCC_MAX_OUTSTANDING_SIGNALS    (10)

/* number of cores */
#define MCC_NUM_CORES                  (2)

/* core number */
#define MCC_CORE_NUMBER                (_psp_core_num())

/* semaphore number */
#define MCC_SHMEM_SEMAPHORE_NUMBER      (1)
#define MCC_PRINTF_SEMAPHORE_NUMBER     (2)
#define MCC_I2C_SEMAPHORE_NUMBER        (3)
#define MCC_RESERVED1_SEMAPHORE_NUMBER  (4)
#define MCC_RESERVED2_SEMAPHORE_NUMBER  (5)

#endif /* __MCC_CONFIG__ */
