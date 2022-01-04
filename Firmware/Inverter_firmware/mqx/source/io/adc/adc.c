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
* $FileName: adc.c$
* $Version : 3.8.36.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the generic ADC driver functions
*
*END************************************************************************/
#include "mqx.h"
#include "bsp.h"
#include "fio.h"
#include "fio_prv.h"
#include <string.h>
#include "io.h"
#include "adc_conf.h"
#include "adc.h"
#include "adc_prv.h"

#ifndef ADC_MAX_CHANNELS
    #define ADC_MAX_CHANNELS ADC_MAX_SW_CHANNELS
#endif

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a adc driver.
*
*END*********************************************************************/
_mqx_uint _io_adc_install
   (
      /* [IN] A string that identifies the device for fopen */
      char_ptr  identifier,
      /* pointer to a HW specific structure */
      pointer   init
   ) 
   
{ /* Body */
   ADC_DRIVER_BUNDLE_PTR adc_driver_bundle;
   _mqx_uint result;

   if (NULL == (adc_driver_bundle = (ADC_DRIVER_BUNDLE_PTR) _mem_alloc(sizeof(ADC_DRIVER_BUNDLE))))
      return ADC_ERROR_ALLOC; /* memory allocation error */

   _mem_zero(adc_driver_bundle, sizeof(ADC_DRIVER_BUNDLE));

   if (IO_OK == (result = _adc_hw_install(identifier, adc_driver_bundle, init)))
      if (IO_OK == (result = _adt_hw_install(identifier, adc_driver_bundle, init)))
         result = _io_dev_install(identifier,
            _adc_open,
            _adc_close,
            _adc_read,
            _adc_write,
            _adc_ioctl,
            adc_driver_bundle);

   if (result != IO_OK)
      _mem_free(adc_driver_bundle); 

   return result;
} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_open
* Returned Value   : _mqx_int - ADC_ERROR_* or ADC_OK
* Comments         : Opens and initializes ADC driver.
* 
*END*********************************************************************/
_mqx_int _adc_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR fd_ptr,
       
      /* [IN] the remaining portion of the name of the device */
      char_ptr   open_name_ptr,

      /* [IN] the initialization parameters (ADC_INIT_STRUCT_PTR) */
      char_ptr   flags
   )
{ /* Body */
   ADC_DRIVER_BUNDLE_PTR       adc_driver_bundle = (ADC_DRIVER_BUNDLE_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
   char_ptr                    file_name_ptr = fd_ptr->DEV_PTR->IDENTIFIER; /* use driver name temporarily */
   _mqx_int                    status;

   while (*file_name_ptr++ != 0)
      open_name_ptr++; /* move to the file name */
   file_name_ptr = open_name_ptr; /* throw out file_name_ptr and assign the correct file name */

   if (*file_name_ptr == 0) {
      /* 
      ** ADC DEVICE DRIVER OPENED WITHOUT CHANNEL SPECIFIED
      */                                         
      ADC_INIT_STRUCT_PTR  adc_init = (ADC_INIT_STRUCT_PTR) flags;

      if (adc_init == NULL)
         return ADC_ERROR_PARAM; /* mandatory open parameter missing */

      if (NULL == adc_driver_bundle->adc) {
         adc_driver_bundle->adc = (ADC_PTR) _mem_alloc_zero(sizeof(ADC));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
         if (adc_driver_bundle->adc == NULL)
            return ADC_ERROR_ALLOC; /* memory allocation error */
#endif
         adc_driver_bundle->adt = (ADT_PTR) _mem_alloc_zero(sizeof(ADT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
         if (adc_driver_bundle->adt == NULL) {
            _mem_free(adc_driver_bundle->adt);
            return ADC_ERROR_ALLOC; /* memory allocation error */
         }
#endif
      } else
         return ADC_ERROR_OPENED; /* cannot open more than one instance of ADC driver */
      
      adc_driver_bundle->adc->g.resolution = adc_init->resolution;
      adc_driver_bundle->adt->g.run = 0;

      if (IO_OK != (status = _adc_hw_init(fd_ptr, adc_driver_bundle->adc))) { /* init cpu according to adc_general_config */
         _mem_free(adc_driver_bundle->adc);
         _mem_free(adc_driver_bundle->adt);
         adc_driver_bundle->adc = NULL;
         adc_driver_bundle->adt = NULL;
      }
      else if (IO_OK != (status = _adt_hw_init(fd_ptr, adc_driver_bundle->adt))) { /* init cpu according to adt_general_config */
         _mem_free(adc_driver_bundle->adc);
         _mem_free(adc_driver_bundle->adt);
         adc_driver_bundle->adc = NULL;
         adc_driver_bundle->adt = NULL;
      }
      
      //fd_ptr->DEV_DATA_PTR = NULL; /* not needed: we dont need to store any additional data */

      return status;
   }
   else {
      /*
      ** CHANNEL TO BE OPENED, FILENAME IS IN OPEN_NAME_PTR
      */
      ADC_INIT_CHANNEL_STRUCT_PTR ch_init = (ADC_INIT_CHANNEL_STRUCT_PTR) flags;
      ADC_CHANNEL_BUNDLE_PTR      adc_channel_bundle;
      ADC_CHANNEL_PTR             *adc_ch;
      ADT_CHANNEL_PTR             *adt_ch;
      _mqx_uint                   ch = 0, i;
    
      adc_ch = adc_driver_bundle->adc_ch;
      adt_ch = adc_driver_bundle->adt_ch;
      
      if (ch_init == NULL)
         return ADC_ERROR_PARAM; /* mandatory open parameter missing */

      if (NULL == adc_driver_bundle->adc)
         return ADC_ERROR_MISSING_DEVICE; /* general device was not opened */

      if (ch_init->number_samples < 1)
         return ADC_ERROR_BAD_PARAM; /* number of samples must be more than 0 */

//      open_name_ptr = file_name_ptr; // not needed, already done

#if ADC_STORE_FILENAMES
      for (ch = 0; ch < ADC_MAX_CHANNELS; ch++)
         if (!strcmp((const char *) adc_driver_bundle->adc_name[ch], open_name_ptr))
            return ADC_ERROR_OPENED; /* file with such name already opened */
#endif
      for (ch = 0; (ch < ADC_MAX_CHANNELS) && (adc_driver_bundle->adc_name[ch] != NULL); ch++)
      { /* searching for free channel in the bundle */
      }
      
      if (ch == ADC_MAX_CHANNELS)
         return ADC_ERROR_FULL; /* no free channel */

      /* variable 'ch' now contains valid channel number */
      adc_channel_bundle = (ADC_CHANNEL_BUNDLE_PTR) _mem_alloc_zero(sizeof(ADC_CHANNEL_BUNDLE));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
      if (adc_channel_bundle == NULL) {
         status = ADC_ERROR_ALLOC; /* memory allocation error */
         goto error_channel_0;
      }
#endif

      if (adc_ch[ch] == NULL) {
         adc_ch[ch] = (ADC_CHANNEL_PTR) _mem_alloc_zero(sizeof(ADC_CHANNEL));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
         if (adc_ch[ch] == NULL) {
            status = ADC_ERROR_ALLOC; /* memory allocation error */
            goto error_channel_1;
         }
#endif
         adt_ch[ch] = (ADT_CHANNEL_PTR) _mem_alloc_zero(sizeof(ADT_CHANNEL));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
         if (adt_ch[ch] == NULL) {
            status = ADC_ERROR_ALLOC; /* memory allocation error */
            goto error_channel_2;
         }
#endif
      } else
         return IO_ERROR; /* channel already in use */

      adc_channel_bundle->adc_ch = adc_ch[ch];
      adc_channel_bundle->adt_ch = adt_ch[ch];
      /* store file context for opened channel */
      fd_ptr->DEV_DATA_PTR = adc_channel_bundle; /* store information about channel */

      /* allocate channel buffer */
      adc_ch[ch]->g.buffer_start = _mem_alloc_zero((ch_init->buffer_size + 1) * sizeof(ADC_RESULT_STRUCT)); /* zero is needed for ADC_CHANNEL_ACCUMULATE */
      adc_ch[ch]->g.buffer_end = adc_ch[ch]->g.buffer_start + (ch_init->buffer_size + 1);

      if (adc_ch[ch]->g.buffer_start == NULL) {
         status = ADC_ERROR_ALLOC; /* memory allocation error */
         goto error_channel_3;
      }

      // initialize channel
      adc_ch[ch]->g.buffer_task = adc_ch[ch]->g.buffer_start;
      adc_ch[ch]->g.buffer_driver = adc_ch[ch]->g.buffer_start;

      adc_ch[ch]->g.number = ch;
      adc_ch[ch]->g.init_flags = ch_init->flags;
      adc_ch[ch]->g.source = ch_init->source;

      adt_ch[ch]->g.trigger = ch_init->trigger;
      adt_ch[ch]->g.offset = ch_init->time_offset;
      adt_ch[ch]->g.period = ch_init->time_period;
      
      adt_ch[ch]->g.number = ch;
      adt_ch[ch]->g.init_flags = ch_init->flags;
      adt_ch[ch]->g.samples_preset = ch_init->number_samples - 1;
      adt_ch[ch]->g.samples = adt_ch[ch]->g.samples_preset;

#if MQX_USE_LWEVENTS
      adt_ch[ch]->g.complete_event = ch_init->complete_event;
      if (ch_init->event_mask == 0)
         adt_ch[ch]->g.event_mask = 0x01;
      else
         adt_ch[ch]->g.event_mask = ch_init->event_mask;
#endif

      if (IO_OK != (status = _adc_hw_channel_init(fd_ptr, adc_ch[ch]))) /* this should initialize ADC converter hardware */
         goto error_channel_4;

      if (IO_OK != (status = _adt_hw_channel_init(fd_ptr, adt_ch[ch]))) /* this should initialize ADC timer hardware */
         goto error_channel_5;

#if ADC_STORE_FILENAMES
      /* allocate filename buffer */
      if (NULL == (adc_driver_bundle->adc_name[ch] = _mem_alloc_zero(strlen(open_name_ptr) + 1))) /* +1 for zero-ending char */
         goto error_channel_6;
      for (i = 0; open_name_ptr[i] != 0; i++)
         adc_driver_bundle->adc_name[ch][i] = open_name_ptr[i];
      adc_driver_bundle->adc_name[ch][i] = 0;
#else
      adc_driver_bundle->adc_name[ch] = (pointer) 0xFFFFFFFF; /* channel is used */
#endif
      
      /* if ADC_CHANNEL_START_NOW was defined, then start channel */
      if (!(ch_init->flags & ADC_CHANNEL_START_TRIGGERED))
         return _adc_ioctl(fd_ptr, ADC_IOCTL_RUN_CHANNEL, NULL);
      
      return IO_OK;     
      
error_channel_6:
      _adt_hw_channel_deinit(fd_ptr, adt_ch[ch]);
error_channel_5:
      _adc_hw_channel_deinit(fd_ptr, adc_ch[ch]);
error_channel_4:
      _mem_free(adc_ch[ch]->g.buffer_start);
error_channel_3:
      _mem_free(adt_ch[ch]);
      adt_ch[ch] = NULL;
error_channel_2:
      _mem_free(adc_ch[ch]);
      adc_ch[ch] = NULL;
error_channel_1:
      _mem_free(adc_channel_bundle);
error_channel_0:
      return status;
   }


} /* Endbody */

/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_close
* Returned Value   : ERROR CODE
* Comments         : Closes ADC driver or channel
* 
*END*********************************************************************/
_mqx_int _adc_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR f
   )
{ /* Body */
   ADC_DRIVER_BUNDLE_PTR adc_driver_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
   ADC_CHANNEL_BUNDLE_PTR adc_channel_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
   ADC_CHANNEL_PTR *adc_ch = adc_driver_bundle->adc_ch;
   ADT_CHANNEL_PTR *adt_ch = adc_driver_bundle->adt_ch;
   _mqx_int        i, status;

   if (adc_channel_bundle == NULL) { /* we close ADC */
      /* let's check if there is no opened channel */
      for (i = 0; i < ADC_MAX_CHANNELS; i++)
         if (adc_ch[i] != NULL)
            return ADC_ERROR_NONEMPTY; /* cannot close ADC driver: there are still open channels */
      if (IO_OK != (status = _adt_hw_deinit(f, adc_driver_bundle->adt)))
         return status;
      if (IO_OK != (status = _adc_hw_deinit(f, adc_driver_bundle->adc)))
         return status;
      _mem_free(adc_driver_bundle->adt);
      adc_driver_bundle->adt = NULL; /* Needed for succesfull re-open */
      _mem_free(adc_driver_bundle->adc);
      adc_driver_bundle->adc = NULL; /* Needed for succesfull re-open */
   }
   else { /* we close ADC channel */
      i = adc_channel_bundle->adc_ch->g.number; /* get item number of channel in the array */
      _adt_stop(f, &adc_channel_bundle->adt_ch->g, 0);
      if (IO_OK != (status = _adt_hw_channel_deinit(f, adc_channel_bundle->adt_ch)))
         return status;
      if (IO_OK != (status = _adc_hw_channel_deinit(f, adc_channel_bundle->adc_ch)))
         return status;
#if ADC_STORE_FILENAMES
      _mem_free(adc_driver_bundle->adc_name[i]);
#endif
      adc_driver_bundle->adc_name[i] = NULL;
      _mem_free(adc_channel_bundle->adc_ch->g.buffer_start);
      _mem_free(adc_channel_bundle->adc_ch);
      _mem_free(adc_channel_bundle->adt_ch);
      _mem_free(adc_channel_bundle);
      adc_ch[i] = NULL;
      adt_ch[i] = NULL;
   }

   return IO_OK;
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_read
* Returned Value   : number of characters read
* Comments         : Reads data from ADC driver
* 
*END*********************************************************************/
_mqx_int _adc_read
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR f,

      /* [IN] where the characters are to be stored */
      char_ptr   data_ptr,

      /* [IN] the number of characters to input */
      _mqx_int   num
   )
{ /* Body */
    _mqx_int cp_cnt;
    ADC_RESULT_STRUCT _PTR_ dst = (ADC_RESULT_STRUCT _PTR_) data_ptr;
    ADC_CHANNEL_BUNDLE_PTR adc_channel_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
    ADC_CHANNEL_GENERIC_PTR ch_conf = &adc_channel_bundle->adc_ch->g;
    
    if (ch_conf->buffer_task == NULL)
        return 0; /* channel not run yet */

    cp_cnt = (ch_conf->buffer_driver - ch_conf->buffer_task);
    if (cp_cnt < 0)
        cp_cnt += (ch_conf->buffer_end - ch_conf->buffer_start);
    
    if (cp_cnt > (num / sizeof(ADC_RESULT_STRUCT)))
        cp_cnt = (num / sizeof(ADC_RESULT_STRUCT));

    while (cp_cnt) {
        *dst = *ch_conf->buffer_task;
        dst++;
        ch_conf->buffer_task->result = 0; /* zero buffer to be usable next time (applicable for ADC_CHANNEL_ACCUMULATE) */
        if (++ch_conf->buffer_task >= ch_conf->buffer_end)
            ch_conf->buffer_task = ch_conf->buffer_start;

        cp_cnt--;
    }
    
    return (char_ptr)dst - data_ptr;
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_write
* Returned Value   : number of characters written
* Comments         : Does nothing, ADC is solely read-only driver
* 
*END*********************************************************************/
_mqx_int _adc_write
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR f,

      /* [IN] where the characters are */
      char_ptr   data_ptr,

      /* [IN] the number of characters to output */
      _mqx_int   num
   )
{ /* Body */
   return 0;
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _adc_ioctl
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/
_mqx_int _adc_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR f,

      /* [IN] the ioctl command */
      _mqx_uint  cmd,

      /* [IN] the ioctl parameters */
      pointer    param_ptr
   )
{ /* Body */
//    ADC_DRIVER_BUNDLE_PTR adc_driver_bundle = (ADC_DRIVER_BUNDLE_PTR) f->DEV_PTR->DRIVER_INIT_PTR;
    ADC_CHANNEL_BUNDLE_PTR adc_channel_bundle = (ADC_CHANNEL_BUNDLE_PTR) f->DEV_DATA_PTR;
//    ADC_CHANNEL_PTR *adc_ch = adc_driver_bundle->adc_ch;
//    ADT_CHANNEL_PTR *adt_ch = adc_driver_bundle->adt_ch;
    uint_32_ptr               uparam_ptr = param_ptr;
    _mqx_int                  status1, status2;

    switch (cmd) 
    {
        case ADC_IOCTL_RUN_CHANNEL:
            /* Runs specific channel */
            return _adt_trigger(f, &adc_channel_bundle->adt_ch->g, 0);
        case ADC_IOCTL_FIRE_TRIGGER:
            /* Runs channels associated with trigger mask */
            return _adt_trigger(f, NULL, (ADT_TRIGGER_MASK) param_ptr);
        case ADC_IOCTL_STOP_CHANNEL:
            /* Stops specific channel */
            return _adt_stop(f, &adc_channel_bundle->adt_ch->g, 0);
        case ADC_IOCTL_STOP_CHANNELS:
            /* Stops channels associated with trigger mask */
            return _adt_stop(f, NULL, (ADT_TRIGGER_MASK) param_ptr);
        case ADC_IOCTL_PAUSE_CHANNEL:
            /* Stops specific channel */
            return _adt_pause(f, &adc_channel_bundle->adt_ch->g, 0);
        case ADC_IOCTL_PAUSE_CHANNELS:
            /* Stops channels associated with trigger mask */
            return _adt_pause(f, NULL, (ADT_TRIGGER_MASK) param_ptr);
        case ADC_IOCTL_RESUME_CHANNEL:
            /* Resumes specific channel */
            return _adt_resume(f, &adc_channel_bundle->adt_ch->g, 0);
        case ADC_IOCTL_RESUME_CHANNELS:
            /* Resumes channels associated with trigger mask */
            return _adt_resume(f, NULL, (ADT_TRIGGER_MASK)param_ptr);
        case IO_IOCTL_DEVICE_IDENTIFY:  
            /* return the device identify */
            uparam_ptr[0] = IO_DEV_TYPE_PHYS_ADC;
            uparam_ptr[1] = 0;
            uparam_ptr[2] = IO_DEV_ATTR_READ;
            return IO_OK;

        default:
            /* hardware specific ioctl commands */
            status1 = _adc_hw_ioctl(f, cmd, param_ptr);
            if ((status1 == IO_OK) || (status1 == IO_ERROR_INVALID_IOCTL_CMD)) {
                status2 = _adt_hw_ioctl(f, cmd, param_ptr);
                if (status2 == IO_ERROR_INVALID_IOCTL_CMD)
                    return status1;
                return status2;
            }
            else
                return status1;
    }
} /* Endbody */
