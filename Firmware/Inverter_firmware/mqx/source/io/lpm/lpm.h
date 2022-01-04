
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
* $FileName: lpm.h$
* $Version : 3.8.4.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   Low Power Manager header file.
*
*END************************************************************************/

#ifndef __lpm_h__
#define __lpm_h__

/*-------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

typedef enum {
    LPM_NOTIFICATION_TYPE_PRE,
    LPM_NOTIFICATION_TYPE_POST
} LPM_NOTIFICATION_TYPE;

typedef enum {
    LPM_NOTIFICATION_RESULT_OK,
    LPM_NOTIFICATION_RESULT_ERROR
} LPM_NOTIFICATION_RESULT;


/*-------------------------------------------------------------------------*/
/*
**                            MACRO DECLARATIONS
*/


/*-------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

typedef struct lpm_notification_struct {
    /* When the notification happens */
    LPM_NOTIFICATION_TYPE   NOTIFICATION_TYPE;
    
    /* Current system operation mode */
    LPM_OPERATION_MODE      OPERATION_MODE;
    
    /* Current system clock configuration */
    BSP_CLOCK_CONFIGURATION CLOCK_CONFIGURATION;
    
} LPM_NOTIFICATION_STRUCT, _PTR_ LPM_NOTIFICATION_STRUCT_PTR;

typedef LPM_NOTIFICATION_RESULT (_CODE_PTR_ LPM_NOTIFICATION_CALLBACK)(LPM_NOTIFICATION_STRUCT_PTR, pointer);

typedef struct lpm_registration_struct {
    /* Callback called when system clock configuration changes */
    LPM_NOTIFICATION_CALLBACK CLOCK_CONFIGURATION_CALLBACK;

    /* Callback called when system operation mode changes */
    LPM_NOTIFICATION_CALLBACK OPERATION_MODE_CALLBACK;

    /* The order (priority) of notifications among other drivers */
    _mqx_uint                 DEPENDENCY_LEVEL;
   
} LPM_REGISTRATION_STRUCT, _PTR_ LPM_REGISTRATION_STRUCT_PTR;


/*-------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif


extern _mqx_uint                _lpm_install (const LPM_CPU_OPERATION_MODE _PTR_, LPM_OPERATION_MODE);
extern _mqx_uint                _lpm_uninstall (void);
extern _mqx_uint                _lpm_register_driver (const LPM_REGISTRATION_STRUCT_PTR, const pointer, _mqx_uint_ptr);
extern _mqx_uint                _lpm_unregister_driver (_mqx_uint);
extern _mqx_uint                _lpm_set_clock_configuration (BSP_CLOCK_CONFIGURATION);
extern BSP_CLOCK_CONFIGURATION  _lpm_get_clock_configuration (void);
extern _mqx_uint                _lpm_set_operation_mode (LPM_OPERATION_MODE);
extern LPM_OPERATION_MODE       _lpm_get_operation_mode (void);
extern _mqx_uint                _lpm_idle_sleep_setup (boolean);


#ifdef __cplusplus
}
#endif


#endif

/* EOF */
