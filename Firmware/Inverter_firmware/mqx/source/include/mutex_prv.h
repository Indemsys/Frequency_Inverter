/*HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: mutex_prv.h$
* $Version : 3.8.0.1$
* $Date    : Feb-20-2012$
*
* Comments:
*
*   This include file is used to define constants and data types
*   private to the mutex component.
*
*END************************************************************************/
#ifndef __mutex_prv_h__
#define __mutex_prv_h__ 1

/*--------------------------------------------------------------------------*/
/* 
 *                      CONSTANT DECLARATIONS
 */

/*!
 * \cond DOXYGEN_PRIVATE
 * 
 * \brief This structure defines the mutex component data structure.
 */
typedef struct mutex_component_struct 
{

   /*! \brief A queue of all created mutexes. */
   QUEUE_STRUCT MUTEXES;
      
   /*! \brief A validation field for mutexes. */
   _mqx_uint     VALID;
     
} MUTEX_COMPONENT_STRUCT, _PTR_ MUTEX_COMPONENT_STRUCT_PTR;
/*! \endcond */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern void _mutex_cleanup(TD_STRUCT_PTR);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __mutex_prv_h__ */
/* EOF */
