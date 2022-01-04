/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
***************************************************************************** 
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
*****************************************************************************
*
* $FileName: lpm_prv.h$
* $Version : 3.8.3.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   Low Power Manager private definitions.
*
*END************************************************************************/

#ifndef __lpm_prv_h__
#define __lpm_prv_h__


/*-------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/


/*-------------------------------------------------------------------------*/
/*
**                            MACRO DECLARATIONS
*/


/*-------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

typedef struct lpm_driver_entry_struct {
    /* Previous in the driver list */
    struct lpm_driver_entry_struct _PTR_ PREVIOUS;

    /* Next in the driver list */
    struct lpm_driver_entry_struct _PTR_ NEXT;
    
    /* Driver registration record */
    LPM_REGISTRATION_STRUCT              REGISTRATION;

    /* Driver specific data passed to callbacks */
    pointer                              DATA;
    
    /* Unique registration identification */
    _mqx_uint                            ID;

} LPM_DRIVER_ENTRY_STRUCT, _PTR_ LPM_DRIVER_ENTRY_STRUCT_PTR;

typedef struct lpm_state_struct {
    /* CPU core operation mode behavior specification */
    const LPM_CPU_OPERATION_MODE _PTR_ CPU_OPERATION_MODES;
    
    /* Current system operation mode */
    LPM_OPERATION_MODE                 OPERATION_MODE;
    
    /* List of registered drivers */
    LPM_DRIVER_ENTRY_STRUCT_PTR        DRIVER_ENTRIES;
    
    /* Unique ID counter */
    _mqx_uint                          COUNTER;
    
    /* LPM functions synchronization */
    LWSEM_STRUCT                       SEMAPHORE;
    
    /* Whether idle sleep is turned ON */
    boolean                            IDLE_SLEEP;
    
} LPM_STATE_STRUCT, _PTR_ LPM_STATE_STRUCT_PTR;


/*-------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#endif

/* EOF */
