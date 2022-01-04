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
* $FileName: init_lpm.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   Low Power Manager initialization definitions.
*
*END************************************************************************/

#ifndef __init_lpm_h__
    #define __init_lpm_h__

/*-------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

typedef enum 
{
    LPM_OPERATION_MODE_RUN = 0,
    LPM_OPERATION_MODE_WAIT,
    LPM_OPERATION_MODE_SLEEP,
    LPM_OPERATION_MODE_STOP,
    LPM_OPERATION_MODES          /* Number of operation modes available */
} LPM_OPERATION_MODE;


/*-------------------------------------------------------------------------*/
/*
**                            MACRO DECLARATIONS
*/


/*-------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/


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
