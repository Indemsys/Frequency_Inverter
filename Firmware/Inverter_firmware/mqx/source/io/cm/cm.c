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
* $FileName: cm.c$
* $Version : 3.8.2.0$
* $Date    : Aug-29-2011$
*
* Comments:
*   Clock manager generic functions
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

/*FUNCTION*********************************************************************
* 
* Function Name   : _cm_set_clock_configuration
*
* Input Params    : clock_configuration – runtime clock configuration 
*
* Returned Value  : CM_OK       – if clock mode change was successful
*                   CM_ERROR    – if clock mode change was not successful
*
* Comments        : Function changes all internal clock configurations. 
*                   It calls underlying PE method  SetClockConfiguration() 
*                   that manages switching to clock configuration predefined 
*                   by in PE CPU wizard and calls. 
*
*END**************************************************************************/

_mqx_int _cm_set_clock_configuration
(
    /* [IN] runtime clock configuration */
    const BSP_CLOCK_CONFIGURATION clock_configuration
)
{
    return (_mqx_int)_bsp_set_clock_configuration(clock_configuration);
}


/*FUNCTION*********************************************************************
* 
* Function Name   : _cm_get_clock_configuration
*
* Input Params    : clock_configuration – runtime clock configuration 
*
* Returned Value  : number of active  clock configuration
*
* Comments        : Function changes all internal clock configurations. 
*                   It calls underlying PE method  GetClockConfiguration() 
*                   that returns active clock configuration predefined 
*                   by in PE CPU wizard and calls. 
*
*END**************************************************************************/

BSP_CLOCK_CONFIGURATION _cm_get_clock_configuration
(
    void
)
{
    return _bsp_get_clock_configuration();
}

/*FUNCTION*********************************************************************
* 
* Function Name   : _cm_get_clock
*
* Input Params    : clock_configuration – runtime clock configuration
*                   clock_source        – clock source 
*
* Returned Value  : input frequency in [Hz]
*
* Comments        : Function returns clock frequency specified by clock_source 
*                   and clock_configuration. 
*                   It looks up in CpuClockConfigurations field 
*                   defined in PE_LDD.c for relevant clock.
*
*END**************************************************************************/

uint32_t _cm_get_clock
(
    /* [IN] runtime clock configuration */
    const BSP_CLOCK_CONFIGURATION   clock_configuration,
    /* [IN] clock source index */
    const CM_CLOCK_SOURCE           clock_source
)
{
    return _bsp_get_clock(clock_configuration, clock_source);
}

