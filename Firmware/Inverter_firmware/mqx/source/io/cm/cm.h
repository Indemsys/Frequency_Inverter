/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: cm.h$
* $Version : 3.8.2.0$
* $Date    : Aug-29-2011$
*
* Comments:
*   Clock manager header file.
*
*END************************************************************************/

#ifndef __cm_h__
    #define __cm_h__

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
**
**  Clock manager public types and constants
**
***************************************************************************/
#define CM_ERR_OK           0x00U /* OK */
#define CM_ERR_SPEED        0x01U /* This device does not work in the active speed mode. */
#define CM_ERR_RANGE        0x02U /* Parameter out of range. */
#define CM_ERR_VALUE        0x03U /* Parameter of incorrect value. */
#define CM_ERR_FAILED       0x1BU /* Requested functionality or process failed. */
#define CM_ERR_PARAM_MODE   0x81U /* Invalid mode. */
    
_mqx_int _cm_set_clock_configuration
(
    /* [IN] runtime clock configuration */
    const BSP_CLOCK_CONFIGURATION clock_configuration
);

BSP_CLOCK_CONFIGURATION _cm_get_clock_configuration
(
    void
);

uint32_t _cm_get_clock
(
    /* [IN] clock configuration */
    const BSP_CLOCK_CONFIGURATION   clock_configuration,
    /* [IN] clock source index */
    const CM_CLOCK_SOURCE           clock_source
);


#ifdef __cplusplus
}
#endif


#endif /* __cm_h__ */
