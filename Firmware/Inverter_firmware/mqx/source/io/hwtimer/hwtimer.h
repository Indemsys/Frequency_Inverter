#ifndef __hwtimer_h__
#define __hwtimer_h__
/*HEADER********************************************************************
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
 **************************************************************************
 *
 * $FileName: hwtimer.h$
 * $Version : 4.0.1$
 * $Date : Jan-18-2013$
 *
 * Comments:
 *
 *   This include file is used to define constants and data types for the
 *   hwtimer component
 *
 *END************************************************************************/

#include <mqx.h>

#define HWTIMER_LL_CONTEXT_LEN 5
/* Type definitions */

typedef void  (_CODE_PTR_  HWTIMER_CALLBACK_FPTR)(pointer);
/*!
 * \brief hwtimer structure.
 *
 * This structure defines a hwtimer.
 * The context structure is passed to all API functions (besides other parameters).
 *
 * \warning Application should not access members of this structure directly.
 *
 * \see hwtimer_init
 * \see hwtimer_deinit
 * \see hwtimer_start
 * \see hwtimer_stop
 * \see hwtimer_set_freq
 * \see hwtimer_get_freq
 * \see hwtimer_set_period
 * \see hwtimer_get_period
 * \see hwtimer_get_modulo
 * \see hwtimer_get_time
 * \see hwtimer_get_ticks
 * \see hwtimer_callback_reg
 * \see hwtimer_callback_block
 * \see hwtimer_callback_unblock
 * \see hwtimer_callback_cancel
 */
typedef struct hwtimer
{
    /*! \brief Pointer to device interface structure */
    const struct hwtimer_devif_struct *   devif;
    /*! \brief Clock identifier used for obtaining timer's source clock. */
    uint_32                         clock_id;
    /*! \brief Actual total divider */
    uint_32                         divider;
    /*! \brief Determine how many subticks are in one tick */
    uint_32                         modulo;
    /*! \brief Number of elapsed ticks */
    volatile uint_64                ticks;
    /*! \brief Function pointer to be called when the timer expires. */
    HWTIMER_CALLBACK_FPTR           callback_func;
    /*! \brief Arbitrary pointer passed as parameter to the callback function. */
    pointer                         callback_data;
    /*! \brief Indicate pending callback. 
     *  If the timer overflows when callbacks are blocked the callback becomes pending. */
    volatile int                    callback_pending;
    /*! \brief Indicate blocked callback */
    int                             callback_blocked;
    /*! \brief Private storage locations for arbitrary data keeping the context of the lower layer driver */
    uint_32                         ll_context[HWTIMER_LL_CONTEXT_LEN];
} HWTIMER, _PTR_ HWTIMER_PTR;

/*!
 * \brief hwtimer_time structure.
 *
 * hwtimer time structure represents a timestamp consisting of timer elapsed periods (TICKS) and current value of the timer counter (SUBTICKS).
 *
 * \see hwtimer_get_time
 */
typedef struct hwtimer_time_struct
{
    /*! \brief Ticks of timer */
    uint_64 TICKS;
    /*! \brief Subticks of timer */
    uint_32 SUBTICKS;
} HWTIMER_TIME_STRUCT, _PTR_ HWTIMER_TIME_PTR;

typedef _mqx_int (_CODE_PTR_  HWTIMER_DEVIF_INIT_FPTR)(HWTIMER_PTR, uint_32, uint_32);
typedef _mqx_int (_CODE_PTR_  HWTIMER_DEVIF_DEINIT_FPTR)(HWTIMER_PTR);
typedef _mqx_int (_CODE_PTR_  HWTIMER_DEVIF_SET_DIV_FPTR)(HWTIMER_PTR, uint_32);
typedef _mqx_int (_CODE_PTR_  HWTIMER_DEVIF_START_FPTR)(HWTIMER_PTR);
typedef _mqx_int (_CODE_PTR_  HWTIMER_DEVIF_STOP_FPTR)(HWTIMER_PTR);
typedef _mqx_int (_CODE_PTR_  HWTIMER_DEVIF_RESET_FPTR)(HWTIMER_PTR);
typedef _mqx_int (_CODE_PTR_  HWTIMER_DEVIF_GET_TIME_FPTR)(HWTIMER_PTR, HWTIMER_TIME_PTR);

/*!
 * \brief hwtimer_devif structure.
 *
 * Each low layer driver exports instance of this structure initialized with pointers to API functions the driver implements.
 * The functions themselves should be declared as static (not exported directly).
 *
 * \see hwtimer_init
 * \see hwtimer_deinit
 */
typedef struct hwtimer_devif_struct
{
     /*! \brief Function pointer to lower layer initialization */
    HWTIMER_DEVIF_INIT_FPTR             INIT;
     /*! \brief Function pointer to lower layer de-initialization */
    HWTIMER_DEVIF_DEINIT_FPTR           DEINIT;
     /*! \brief Function pointer to lower layer set divider functionality */
    HWTIMER_DEVIF_SET_DIV_FPTR          SET_DIV;
     /*! \brief Function pointer to lower layer start functionality */
    HWTIMER_DEVIF_START_FPTR            START;
     /*! \brief Function pointer to lower layer stop functionality */
    HWTIMER_DEVIF_STOP_FPTR             STOP;
     /*! \brief Function pointer to lower layer get time functionality */
    HWTIMER_DEVIF_GET_TIME_FPTR         GET_TIME;
} HWTIMER_DEVIF_STRUCT, _PTR_ HWTIMER_DEVIF_STRUCT_PTR;


/* Function prototypes */
_mqx_int hwtimer_init(HWTIMER_PTR, const HWTIMER_DEVIF_STRUCT *, uint_32, uint_32);
_mqx_int hwtimer_deinit(HWTIMER_PTR);
_mqx_int hwtimer_set_freq(HWTIMER_PTR, uint_32, uint_32);
_mqx_int hwtimer_set_period(HWTIMER_PTR, uint_32, uint_32);
uint_32 hwtimer_get_freq(HWTIMER_PTR);
uint_32 hwtimer_get_period(HWTIMER_PTR);
_mqx_int hwtimer_start(HWTIMER_PTR);
_mqx_int hwtimer_stop(HWTIMER_PTR);
_mqx_int hwtimer_reset(HWTIMER_PTR);
uint_32 hwtimer_get_modulo(HWTIMER_PTR);
_mqx_int hwtimer_get_time(HWTIMER_PTR, HWTIMER_TIME_PTR);
uint_32 hwtimer_get_ticks(HWTIMER_PTR);
_mqx_int hwtimer_callback_reg(HWTIMER_PTR, HWTIMER_CALLBACK_FPTR, pointer);
_mqx_int hwtimer_callback_block(HWTIMER_PTR);
_mqx_int hwtimer_callback_unblock(HWTIMER_PTR);
_mqx_int hwtimer_callback_cancel(HWTIMER_PTR);

#endif
