/*HEADER*********************************************************************
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
 * $FileName: name.c$
 * $Version : 3.8.1.0$
 * $Date    : Apr-13-2012$
 *
 * Comments:
 *
 *   This file contains functions of the Name component.
 *
 *END************************************************************************/

#include <string.h>
#include "mqx_inc.h"
#if MQX_USE_NAME
#include "name.h"
#include "name_prv.h"
#include "mqx_str.h"

/*!
 * \private
 *
 * \brief This function adds a name to the name component along with the 
 * _mqx_max_type number to be associate with the name.
 * 
 * \param[in] name_handle The handle returned by _name_internal_create.
 * \param[in] name        The name to be associated with the number.
 * \param[in] number      The number to be associated with the name.
 * \param[in] index       The index in the name table.
 * 
 * \return MQX_OK
 * \return MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the name component.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data is not valid.)
 * \return NAME_TABLE_FULL (Names database is full.)
 * \return NAME_EXISTS (Name is already in the names database.)
 */ 
_mqx_uint _name_add_internal_by_index
(
    pointer       name_handle,
    char _PTR_    name,
    _mqx_max_type number,
    _mqx_uint     index
)
{ /* Body */
    NAME_COMPONENT_STRUCT_PTR          base_name_manager_ptr;
    register NAME_COMPONENT_STRUCT_PTR name_manager_ptr;
    register NAME_STRUCT_PTR           name_ptr;
    _mqx_uint                          result = MQX_OK;

    name_manager_ptr = (NAME_COMPONENT_STRUCT_PTR) name_handle;
#if MQX_CHECK_ERRORS
    if (name_manager_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */
#if MQX_CHECK_VALIDITY
    if (name_manager_ptr->VALID != NAME_VALID)
    {
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    base_name_manager_ptr = name_manager_ptr;

    /* We are modifying the table, so lets get exclusive access */
    _int_disable();
#if MQX_CHECK_VALIDITY
    if (base_name_manager_ptr->VALID != NAME_VALID)
    {
        _int_enable();
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    _lwsem_wait((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));
    _int_enable();

    /* Scan to correct location in table, and if not found, insert it */
    while (index >= name_manager_ptr->NUMBER_IN_BLOCK)
    {
        index -= name_manager_ptr->NUMBER_IN_BLOCK;
        if (name_manager_ptr->NEXT_TABLE == NULL)
        {
            if (name_manager_ptr->GROW_NUMBER && ((name_manager_ptr->TOTAL_NUMBER + name_manager_ptr->GROW_NUMBER)
                            <= name_manager_ptr->MAX_NUMBER))
            {
                result = _name_create_handle_internal((pointer _PTR_) &name_manager_ptr->NEXT_TABLE,
                                name_manager_ptr->GROW_NUMBER, name_manager_ptr->GROW_NUMBER,
                                name_manager_ptr->MAX_NUMBER, (name_manager_ptr->TOTAL_NUMBER
                                                + name_manager_ptr->GROW_NUMBER));
                if (result != MQX_OK)
                {
                    break;
                }/* Endif */
            }
            else
            {
                result = NAME_TABLE_FULL;
                break;
            } /* Endif */
        } /* Endif */
        name_manager_ptr = name_manager_ptr->NEXT_TABLE;
#if MQX_CHECK_VALIDITY
        if (name_manager_ptr->VALID != NAME_VALID)
        {
            result = MQX_INVALID_COMPONENT_BASE;
            break;
        } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    } /* Endwhile */

    if (result == MQX_OK)
    {
        name_ptr = &name_manager_ptr->NAMES[index];
        if (name_ptr->NAME[0] == '\0')
        {
            strncpy(name_ptr->NAME, name, (_mqx_uint) NAME_MAX_NAME_SIZE - 1);
            name_ptr->NAME[NAME_MAX_NAME_SIZE - 1] = '\0';
            name_ptr->NUMBER = number;
            base_name_manager_ptr->NUMBER++;
        }
        else
        {
            result = NAME_EXISTS;
        } /* Endif */
    } /* Endif */

    _lwsem_post((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));

    return (result);
} /* Endbody */

/*!
 * \brief Adds the name and its associated number to the names database.
 * 
 * \param[in] name   The name to add.
 * \param[in] number The number to be associated with the name.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data is not valid.)
 * \return MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the name component.)
 * \return NAME_EXISTS (Name is already in the names database.)
 * \return NAME_TABLE_FULL (Names database is full.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * 
 * \warning Creates the name component with default values if it was not previously 
 * created.
 * \warning Cannot be called from an ISR.
 * 
 * \see _name_create_component
 * \see _name_delete
 * \see _name_find   
 */ 
_mqx_uint _name_add
(
    char _PTR_    name,
    _mqx_max_type number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                handle;
    _mqx_uint              result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_name_add, name, number);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_name_add, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    handle = kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT];
    if (handle == NULL)
    {
        result = _name_create_component(NAME_DEFAULT_INITIAL_NUMBER, NAME_DEFAULT_GROW_NUMBER,
                        NAME_DEFAULT_MAXIMUM_NUMBER);
        handle = kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT];
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
        if (handle == NULL)
        {
            _KLOGX2(KLOG_name_add, result);
            return (result);
        } /* Endif */
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */
    } /* Endif */

    result = _name_add_internal(handle, name, number);
    _KLOGX2(KLOG_name_add, result);
    return (result);

} /* Endbody */

/*!
 * \private
 * 
 * \brief This function adds a name to the name component along with the 
 * _mqx_max_type number associated with the name.
 * 
 * \param[in] name_handle The handle returned by _name_internal_create.
 * \param[in] name        The name to be associated with the number.
 * \param[in] number      The number to be associated with the name.
 * 
 * \return MQX_OK
 * \return MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the name component.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data is not valid.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * \return NAME_EXISTS (Name is already in the names database.)
 * \return NAME_TABLE_FULL (Names database is full.) 
 */ 
_mqx_uint _name_add_internal
(
    pointer       name_handle,
    char _PTR_    name,
    _mqx_max_type number
)
{ /* Body */
    NAME_COMPONENT_STRUCT_PTR          base_name_manager_ptr;
    register NAME_COMPONENT_STRUCT_PTR name_manager_ptr;
    register NAME_STRUCT_PTR           name_ptr;
    register NAME_STRUCT_PTR           saved_name_ptr;
    register _mqx_uint                 i;
    _mqx_uint                          result = MQX_OK;

#if MQX_CHECK_ERRORS
    if (*name == '\0')
    {
        /* Cannot add 0 length string name */
        return (NAME_TOO_SHORT);
    } /* Endif */
    if (_strnlen(name, NAME_MAX_NAME_SIZE) >= NAME_MAX_NAME_SIZE)
    {
        return (NAME_TOO_LONG);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    name_manager_ptr = (NAME_COMPONENT_STRUCT_PTR) name_handle;
#if MQX_CHECK_ERRORS
    if (name_manager_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */
    base_name_manager_ptr = name_manager_ptr;

    /* We are modifying the table, so lets get exclusive access */
    _int_disable();
#if MQX_CHECK_VALIDITY
    if ((name_manager_ptr->VALID != NAME_VALID) || (base_name_manager_ptr->VALID != NAME_VALID))
    {
        _int_enable();
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    _lwsem_wait((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));
    _int_enable();

    /* 
     * Scan to end of table, looking for a duplicate name..
     * simultaneously, remember the first empty slot found
     */
    saved_name_ptr = NULL;
    while (TRUE)
    {
        i = name_manager_ptr->NUMBER_IN_BLOCK + 1;
        name_ptr = &name_manager_ptr->NAMES[0];
        while (--i)
        {
            if (name_ptr->NAME[0] == '\0')
            {
                if (saved_name_ptr == NULL)
                {
                    saved_name_ptr = name_ptr;
                } /* Endif */
            }
            else
            {
                if (strncmp(name_ptr->NAME, name, (_mqx_uint) NAME_MAX_NAME_SIZE - 1) == 0)
                { /* MATCH */
                    result = NAME_EXISTS;
                    break;
                } /* Endif */
            } /* Endif */
            name_ptr++;
        } /* Endwhile */
        if ((name_manager_ptr->NEXT_TABLE == NULL) || (result != MQX_OK))
        {
            break;
        } /* Endif */
        name_manager_ptr = name_manager_ptr->NEXT_TABLE;
    } /* Endwhile */

    if (result == MQX_OK)
    {
        /* We have scanned the whole table, and the original name was not found */

        if (saved_name_ptr == NULL)
        {
            /* 
             * An empty slot was not found in the table so we must try to
             * grow the name table
             */
            if (base_name_manager_ptr->GROW_NUMBER && ((name_manager_ptr->TOTAL_NUMBER
                            + base_name_manager_ptr->GROW_NUMBER) <= base_name_manager_ptr->MAX_NUMBER))
            {
                result = _name_create_handle_internal((pointer _PTR_) &name_manager_ptr->NEXT_TABLE,
                                base_name_manager_ptr->GROW_NUMBER, base_name_manager_ptr->GROW_NUMBER,
                                base_name_manager_ptr->MAX_NUMBER, (name_manager_ptr->TOTAL_NUMBER
                                                + base_name_manager_ptr->GROW_NUMBER));
                if (result == MQX_OK)
                {
                    name_manager_ptr = name_manager_ptr->NEXT_TABLE;
                    saved_name_ptr = &name_manager_ptr->NAMES[0];
                } /* Endif */
            }
            else
            {
                result = NAME_TABLE_FULL;
            } /* Endif */
        } /* Endif */

        if (saved_name_ptr != NULL)
        {
            strncpy(saved_name_ptr->NAME, name, (_mqx_uint) NAME_MAX_NAME_SIZE - 1);
            saved_name_ptr->NAME[NAME_MAX_NAME_SIZE - 1] = '\0';
            saved_name_ptr->NUMBER = number;
            base_name_manager_ptr->NUMBER++;
        }/* Endif */
    }/* Endif */

    _lwsem_post((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));

    return (result);
} /* Endbody */

/*!
 * \private
 * 
 * \brief This function completely removes the specified name table.
 * 
 * \param[in] name_handle The name data structure pointer.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data is not valid.)
 */ 
_mqx_uint _name_destroy_handle_internal
(
    pointer name_handle
)
{ /* Body */
    register NAME_COMPONENT_STRUCT_PTR name_manager_ptr;
    register NAME_COMPONENT_STRUCT_PTR next_ptr;

    name_manager_ptr = (NAME_COMPONENT_STRUCT_PTR) name_handle;
#if MQX_CHECK_ERRORS
    if (name_manager_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    _int_disable();
#if MQX_CHECK_VALIDITY
    if (name_manager_ptr->VALID != NAME_VALID)
    {
        _int_enable();
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    _lwsem_wait((LWSEM_STRUCT_PTR) (&name_manager_ptr->SEM));
    name_manager_ptr->VALID = 0;
    _int_enable();

    _lwsem_destroy((LWSEM_STRUCT_PTR) (&name_manager_ptr->SEM));

    while (name_manager_ptr)
    {
        next_ptr = name_manager_ptr->NEXT_TABLE;
        name_manager_ptr->VALID = 0;
        _mem_free(name_manager_ptr);
        name_manager_ptr = next_ptr;
    } /* Endwhile */

    return (MQX_OK);

} /* Endbody */

/*!
 * \brief Creates the name component.
 * 
 * If an application previously called the function and maximum_number is greater 
 * than what was specified, MQX changes the maximum number of names to 
 * maximum_number.
 * \n If an application does not explicitly create the name component, MQX does 
 * so with the following default values the first time that a task calls _name_add().
 * \n Parameter default values:
 * \li Initial_number = 8(default)
 * \li Grow_number = 8(default)
 * \li Maximum_number = 0(unlimited) 
 *  
 * \param[in] initial_number Initial number of names that can be stored.
 * \param[in] grow_number    The number of names to be added when table is full.
 * \param[in] maximum_number The maximum number of names that can be stored.
 * 
 * \return MQX_OK (Success; one of:
 * \li Name component is created.
 * \li Name component was already created.)
 * \return MQX_OUT_OF_MEMORY (Failure: MQX cannot allocate memory for the name 
 * component.)
 * 
 * \see _name_add
 * \see _name_delete
 * \see _name_find   
 */ 
_mqx_uint _name_create_component
(
    _mqx_uint initial_number,
    _mqx_uint grow_number,
    _mqx_uint maximum_number
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    _mqx_uint                 result;
    pointer                   handle;
    NAME_COMPONENT_STRUCT_PTR component_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE4(KLOG_name_create_component, initial_number, grow_number, maximum_number);

    component_ptr = (NAME_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT];
    if (component_ptr != NULL)
    {
        if (maximum_number > component_ptr->MAX_NUMBER)
        {
            component_ptr->MAX_NUMBER = maximum_number;
        } /* Endif */
        _KLOGX2(KLOG_name_create_component, MQX_OK);
        return (MQX_OK);
    } /* Endif */

    result = _name_create_handle_internal(&handle, initial_number, grow_number, maximum_number, initial_number);

    /* We must exclude all ISRs at this point */
    if (result == MQX_OK)
    {
        _int_disable();
        if (kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT] != NULL)
        {
            _int_enable();
            _name_destroy_handle_internal(handle);
            component_ptr = (NAME_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT];
            if (maximum_number > component_ptr->MAX_NUMBER)
            {
                component_ptr->MAX_NUMBER = maximum_number;
            } /* Endif */
            _KLOGX2(KLOG_name_create_component, MQX_OK);
            return (MQX_OK);
        } /* Endif */
        kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT] = handle;
        _int_enable();
    } /* Endif */

    _KLOGX2(KLOG_name_create_component, result);
    return (result);

} /* Endbody */
/*!
 * \private
 * 
 * \brief Initializes a name component.
 * 
 * This function provides the storage and retrieval of a name associated with a 
 * number.
 * \n The name component can be used by any other kernel components. Each may have 
 * a separate set of names.
 * 
 * \param[out] name_handle    An address where the name data structure pointer is 
 * to be stored.
 * \param[in]  initial_number The initial number of names that can be stored.
 * \param[in]  grow_number    The number of names to be added when table is full.
 * \param[in]  maximum_number The maximum number of names that can be stored.
 * \param[in]  total_so_far   The total number of name spaces in all chained pools.
 * 
 * \return MQX_OK
 * \return MQX_OUT_OF_MEMORY (MQX cannot allocate memory for the name component.) 
 */ 
_mqx_uint _name_create_handle_internal
(
    pointer _PTR_ name_handle,
    _mqx_uint     initial_number,
    _mqx_uint     grow_number,
    _mqx_uint     maximum_number,
    _mqx_uint     total_so_far
)
{ /* Body */
    register NAME_COMPONENT_STRUCT_PTR name_manager_ptr;

    name_manager_ptr = (NAME_COMPONENT_STRUCT_PTR) _mem_alloc_system_zero((_mem_size) (sizeof(NAME_COMPONENT_STRUCT)
                    + ((initial_number - 1) * sizeof(NAME_STRUCT))));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (name_manager_ptr == NULL)
    {
        return (MQX_OUT_OF_MEMORY);
    } /* Endif */
#endif /* MQX_CHECK_MEMORY_ALLOCATION_ERRORS */
    _mem_set_type(name_manager_ptr, MEM_TYPE_NAME_COMPONENT);

    /* The name component has been created, so we must create the semaphore */
    if (initial_number == total_so_far)
    {
        /* Initialize the semaphore for this name component */
        _lwsem_create((LWSEM_STRUCT_PTR) (&name_manager_ptr->SEM), 1);
    } /* Endif */

    name_manager_ptr->GROW_NUMBER = grow_number;
    if (maximum_number == 0)
    {
        name_manager_ptr->MAX_NUMBER = MAX_MQX_UINT;
    }
    else if (maximum_number < initial_number)
    {
        name_manager_ptr->MAX_NUMBER = initial_number;
    }
    else
    {
        name_manager_ptr->MAX_NUMBER = maximum_number;
    } /* Endif */
    name_manager_ptr->NUMBER_IN_BLOCK = initial_number;
    name_manager_ptr->TOTAL_NUMBER = total_so_far;
    name_manager_ptr->VALID = NAME_VALID;

    *(NAME_COMPONENT_STRUCT_PTR _PTR_) name_handle = name_manager_ptr;

    return (MQX_OK);
} /* Endbody */

/*!
 * \private
 *
 * \brief This function removes a name from the name component.
 * 
 * \param[in] name_handle The handle returned by _name_internal_create.
 * \param[in] index       The index in the name table.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data is not valid.)    
 */ 
_mqx_uint _name_delete_internal_by_index
(
    pointer   name_handle,
    _mqx_uint index
)
{ /* Body */
    register NAME_COMPONENT_STRUCT_PTR base_name_manager_ptr;
    register NAME_COMPONENT_STRUCT_PTR name_manager_ptr;
    register NAME_STRUCT_PTR           name_ptr;
    _mqx_uint                          result = MQX_OK;

    base_name_manager_ptr = (NAME_COMPONENT_STRUCT_PTR) name_handle;
#if MQX_CHECK_ERRORS
    if (base_name_manager_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    _int_disable();
#if MQX_CHECK_VALIDITY
    if (base_name_manager_ptr->VALID != NAME_VALID)
    {
        _int_enable();
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    _lwsem_wait((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));
    _int_enable();

    /* Scan to end of table, and if the name found, delete it */
    name_manager_ptr = base_name_manager_ptr;
    /* Scan to end of table, if found, return number */
    while (index >= name_manager_ptr->NUMBER_IN_BLOCK)
    {
        index -= name_manager_ptr->NUMBER_IN_BLOCK;
        if (name_manager_ptr->NEXT_TABLE == NULL)
        {
            result = NAME_NOT_FOUND;
            break;
        } /* Endif */
        name_manager_ptr = name_manager_ptr->NEXT_TABLE;
#if MQX_CHECK_VALIDITY
        if (name_manager_ptr->VALID != NAME_VALID)
        {
            result = MQX_INVALID_COMPONENT_BASE;
            break;
        } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    }/* Endwhile */

    if (result == MQX_OK)
    {
        name_ptr = (NAME_STRUCT_PTR) & name_manager_ptr->NAMES[index];
        name_ptr->NAME[0] = '\0';
        name_ptr->NUMBER = 0;
        base_name_manager_ptr->NUMBER--;
    } /* Endif */

    _lwsem_post((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));

    return (MQX_OK);
} /* Endbody */

/*!
 * \brief Deletes the name and its associated number from the names database.
 * 
 * \param[in] name The name to be deleted.
 * 
 * \return MQX_OK
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an 
 * ISR.)
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data is not valid.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * \return NAME_NOT_FOUND (Name is not in the names database.)
 * 
 * \warning Cannot be called from an ISR.
 * 
 * \see _name_add
 * \see _name_create_component
 * \see _name_find     
 */ 
_mqx_uint _name_delete
(
    char_ptr name
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                handle;
    _mqx_uint              result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_name_delete, name);

#if MQX_CHECK_ERRORS
    if (kernel_data->IN_ISR)
    {
        _KLOGX2(KLOG_name_delete, MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
        return (MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    handle = kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT];
#if MQX_CHECK_ERRORS
    if (handle == NULL)
    {
        _KLOGX2(KLOG_name_delete, MQX_COMPONENT_DOES_NOT_EXIST);
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */
    result = _name_delete_internal(handle, name);
    _KLOGX2(KLOG_name_delete, result);
    return (result);
} /* Endbody */

/*!
 * \private
 * 
 * \brief This function removes a name from the name component.
 * 
 * \param[in] name_handle The handle returned by _name_internal_create.
 * \param[in] name        The name to be deleted.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data are not valid.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * \return NAME_NOT_FOUND (Name is not in the names database.) 
 */ 
_mqx_uint _name_delete_internal
(
    pointer  name_handle,
    char_ptr name
)
{ /* Body */
    NAME_COMPONENT_STRUCT_PTR          base_name_manager_ptr;
    register NAME_COMPONENT_STRUCT_PTR name_manager_ptr;
    register NAME_STRUCT_PTR           name_ptr;
    register _mqx_uint                 i;

    name_manager_ptr = (NAME_COMPONENT_STRUCT_PTR) name_handle;
#if MQX_CHECK_ERRORS
    if (name_manager_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */

    base_name_manager_ptr = name_manager_ptr;

    /* We are modifying the table, so lets get exclusive access */
    _int_disable();
#if MQX_CHECK_VALIDITY
    if (name_manager_ptr->VALID != NAME_VALID)
    {
        _int_enable();
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
#if MQX_CHECK_ERRORS
    if (*name == '\0')
    {
        /* Cannot delete 0 length string name */
        _int_enable();
        return (NAME_TOO_SHORT);
    } /* Endif */
    if (_strnlen(name, NAME_MAX_NAME_SIZE) >= NAME_MAX_NAME_SIZE)
    {
        _int_enable();
        return (NAME_TOO_LONG);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */
#if MQX_CHECK_VALIDITY
    if (base_name_manager_ptr->VALID != NAME_VALID)
    {
        _int_enable();
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    _lwsem_wait((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));

    _int_enable();

    /* Scan to end of table, and if the name found, delete it */
    while (TRUE)
    {
        i = name_manager_ptr->NUMBER_IN_BLOCK + 1;
        name_ptr = &name_manager_ptr->NAMES[0];
        while (--i)
        {
            if (name_ptr->NAME[0] != '\0')
            {
                if (strncmp(name_ptr->NAME, name, (_mqx_uint) NAME_MAX_NAME_SIZE - 1) == 0)
                { /* MATCH */
                    name_ptr->NAME[0] = '\0';
                    name_ptr->NUMBER = 0;
                    base_name_manager_ptr->NUMBER--;
                    _lwsem_post((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));
                    return (MQX_OK);
                } /* Endif */
            } /* Endif */
            name_ptr++;
        } /* Endwhile */
        if (name_manager_ptr->NEXT_TABLE == NULL)
        {
            _lwsem_post((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));
            return (NAME_NOT_FOUND);
        }
        else
        {
            name_manager_ptr = name_manager_ptr->NEXT_TABLE;
#if MQX_CHECK_VALIDITY
            if (name_manager_ptr->VALID != NAME_VALID)
            {
                _lwsem_post((LWSEM_STRUCT_PTR) (&base_name_manager_ptr->SEM));
                return (MQX_INVALID_COMPONENT_BASE);
            } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
        } /* Endif */
    } /* Endwhile */
} /* Endbody */

/*!
 * \private
 * 
 * \brief This function looks up a name in the name component, and returns the
 * _mqx_max_type quantity associated with the name.
 * 
 * \param[in]  name_handle The handle returned by _name_internal_create.
 * \param[in]  index       The nth name to locate.
 * \param[out] number_ptr  The location where the number is to be returned.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data are not valid.)
 * \return NAME_NOT_FOUND (Name is not in the names database.)
 */ 
_mqx_uint _name_find_internal_by_index
(
    pointer           name_handle,
    _mqx_uint         index,
    _mqx_max_type_ptr number_ptr
)
{ /* Body */
    register NAME_COMPONENT_STRUCT_PTR name_manager_ptr;
    register NAME_STRUCT_PTR           name_ptr;

    name_manager_ptr = (NAME_COMPONENT_STRUCT_PTR) name_handle;
#if MQX_CHECK_ERRORS
    if (name_manager_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */
#if MQX_CHECK_VALIDITY
    if (name_manager_ptr->VALID != NAME_VALID)
    {
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    /* Scan to end of table, if found, return number */
    while (index >= name_manager_ptr->NUMBER_IN_BLOCK)
    {
        index -= name_manager_ptr->NUMBER_IN_BLOCK;
        if (name_manager_ptr->NEXT_TABLE == NULL)
        {
            return (NAME_NOT_FOUND);
        } /* Endif */
        name_manager_ptr = name_manager_ptr->NEXT_TABLE;
#if MQX_CHECK_VALIDITY
        if (name_manager_ptr->VALID != NAME_VALID)
        {
            return (MQX_INVALID_COMPONENT_BASE);
        } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    }/* Endwhile */
    name_ptr = (NAME_STRUCT_PTR) & name_manager_ptr->NAMES[index];
    if (name_ptr->NAME[0] == '\0')
    {
        return (NAME_NOT_FOUND);
    } /* Endif */
    *number_ptr = name_ptr->NUMBER;

    return (MQX_OK);
} /* Endbody */

/*!
 * \brief Gets the number that is associated with the name in the names database.
 * 
 * \param[in]  name       Pointer to the name for which to get the associated number.
 * \param[out] number_ptr Pointer to the number.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data are not valid.)
 * \return NAME_NOT_FOUND (Name is not in the names database.)   
 * 
 * \see _name_add
 * \see _name_create_component
 * \see _name_delete 
 */ 
_mqx_uint _name_find
(
    char_ptr          name,
    _mqx_max_type_ptr number_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                handle;
    _mqx_uint              result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_name_find, name, number_ptr);

    handle = kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT];
#if MQX_CHECK_ERRORS
    if (handle == NULL)
    {
        _KLOGX2(KLOG_name_find, MQX_COMPONENT_DOES_NOT_EXIST);
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */
    result = _name_find_internal(handle, name, number_ptr);

    _KLOGX3(KLOG_name_find, result, *number_ptr);

    return (result);
} /* Endbody */

/*!
 * \private
 * 
 * \brief Gets the number that is associated with the name in the names database.
 * 
 * \param[in]  name_handle The handle returned by _name_internal_create.
 * \param[in]  name        The name to be looked up.
 * \param[out] number_ptr  The location where the number is to be returned.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return NAME_TOO_SHORT (Name is 0 length string.)
 * \return NAME_TOO_LONG (Name is longer than NAME_MAX_NAME_SIZE.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data are not valid.)
 * \return NAME_NOT_FOUND (Name is not in the names database.) 
 */ 
_mqx_uint _name_find_internal
(
    pointer           name_handle,
    char_ptr          name,
    _mqx_max_type_ptr number_ptr
)
{ /* Body */
    register NAME_COMPONENT_STRUCT_PTR name_manager_ptr;
    register NAME_STRUCT_PTR           name_ptr;
    register _mqx_uint                 i;

    name_manager_ptr = (NAME_COMPONENT_STRUCT_PTR) name_handle;
#if MQX_CHECK_ERRORS
    if (name_manager_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
    if (*name == '\0')
    {
        /* Cannot find 0 length string name */
        return (NAME_TOO_SHORT);
    } /* Endif */
    if (_strnlen(name, NAME_MAX_NAME_SIZE) >= NAME_MAX_NAME_SIZE)
    {
        return (NAME_TOO_LONG);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */
#if MQX_CHECK_VALIDITY
    if (name_manager_ptr->VALID != NAME_VALID)
    {
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    /* Scan to end of table, if found, return number */
    while (TRUE)
    {
        i = name_manager_ptr->NUMBER_IN_BLOCK + 1;
        name_ptr = (NAME_STRUCT_PTR) & name_manager_ptr->NAMES[0];
        while (--i)
        {
            if (name_ptr->NAME[0] != '\0')
            {
                if (strncmp(name_ptr->NAME, name, (_mqx_uint) NAME_MAX_NAME_SIZE - 1) == 0)
                { /* MATCH */
                    *number_ptr = name_ptr->NUMBER;
                    return (MQX_OK);
                } /* Endif */
            } /* Endif */
            name_ptr++;
        } /* Endwhile */
        if (name_manager_ptr->NEXT_TABLE == NULL)
        {
            return (NAME_NOT_FOUND);
        } /* Endif */
        name_manager_ptr = name_manager_ptr->NEXT_TABLE;
#if MQX_CHECK_VALIDITY
        if (name_manager_ptr->VALID != NAME_VALID)
        {
            return (MQX_INVALID_COMPONENT_BASE);
        } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    } /* Endwhile */
} /* Endbody */

/*!
 * \brief Gets the name that is associated with the number in the names database.
 * 
 * The function finds the first entry in the database that matches the number 
 * and returns its name.
 * 
 * \param[in]  number   Number for which to get the associated name.
 * \param[out] name_ptr Pointer to the name.
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data are not valid.)
 * \return NAME_NOT_FOUND (Name is not in the names database.) 
 * 
 * \see _name_add
 * \see _name_create_component
 * \see _name_delete 
 */ 
_mqx_uint _name_find_by_number
(
    _mqx_max_type number,
    char_ptr      name_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    pointer                handle;
    _mqx_uint              result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_name_find_name, number, name_ptr);

    handle = kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT];
    if (handle == NULL)
    {
        _KLOGX2(KLOG_name_find_name, MQX_COMPONENT_DOES_NOT_EXIST);
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */

    result = _name_find_name_internal(handle, number, name_ptr);

    _KLOGX2(KLOG_name_find_name, result);
    return (result);
} /* Endbody */

/*!
 * \private
 * 
 * \brief This function looks up a number in the name component, and returns the
 * name associated with the number.
 * 
 * \param[in]  name_handle     The handle returned by _name_internal_create.
 * \param[in]  number          Number for which to get the associated name.
 * \param[out] name_string_ptr Pointer to the name. 
 * 
 * \return MQX_OK
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Name component is not created.)
 * \return MQX_INVALID_COMPONENT_BASE (Name component data are not valid.)
 * \return NAME_NOT_FOUND (Name is not in the names database.) 
 */ 
_mqx_uint _name_find_name_internal
(
    pointer       name_handle,
    _mqx_max_type number,
    char_ptr      name_string_ptr
)
{ /* Body */
    register NAME_COMPONENT_STRUCT_PTR name_manager_ptr;
    register NAME_STRUCT_PTR           name_ptr;
    register _mqx_uint                 i;

    name_manager_ptr = (NAME_COMPONENT_STRUCT_PTR) name_handle;
#if MQX_CHECK_ERRORS
    if (name_manager_ptr == NULL)
    {
        return (MQX_COMPONENT_DOES_NOT_EXIST);
    } /* Endif */
#endif /* MQX_CHECK_ERRORS */
#if MQX_CHECK_VALIDITY
    if (name_manager_ptr->VALID != NAME_VALID)
    {
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */
#endif /* MQX_CHECK_VALIDITY */

    /* Scan to end of table, if found, return number */
    while (TRUE)
    {
        i = name_manager_ptr->NUMBER_IN_BLOCK + 1;
        name_ptr = (NAME_STRUCT_PTR) & name_manager_ptr->NAMES[0];
        while (--i)
        {
            if (name_ptr->NAME[0] != '\0')
            {
                if (number == name_ptr->NUMBER)
                { /* MATCH */
                    strncpy(name_string_ptr, name_ptr->NAME, (_mqx_uint) NAME_MAX_NAME_SIZE - 1);
                    name_string_ptr[NAME_MAX_NAME_SIZE - 1] = '\0';
                    return (MQX_OK);
                } /* Endif */
            } /* Endif */
            name_ptr++;
        } /* Endwhile */

        if (name_manager_ptr->NEXT_TABLE == NULL)
        {
            return (NAME_NOT_FOUND);
        }
        else
        {
            name_manager_ptr = name_manager_ptr->NEXT_TABLE;
        } /* Endif */
#if MQX_CHECK_VALIDITY
        if (name_manager_ptr->VALID != NAME_VALID)
        {
            return (MQX_INVALID_COMPONENT_BASE);
        } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
    } /* Endwhile */
} /* Endbody */

/*!
 * \brief Tests name component for consistency.
 * 
 * The function tests the data structures that are associated with the name 
 * component.
 * <table>
 *   <tr>
 *     <td><b>Return</b></td>
 *     <td><b>name_error_ptr</b></td>
 *     <td><b>name_extension_error_ptr</b></td>  
 *   </tr>
 *   <tr>
 *     <td><b>MQX_CORRUPT_QUEUE</b>
 *     (Task queue that is associated with the name component is incorrect)</td>
 *     <td>NULL</td>
 *     <td>NULL</td>  
 *   </tr>
 *   <tr>
 *     <td><b>MQX_INVALID_COMPONENT_BASE</b>
 *     (MQX found an error in a name component data structure)</td>
 *     <td>Pointer to the name table that has an error.</td>
 *     <td>Pointer to the name table that has an error.</td>  
 *   </tr>   
 * </table>  
 *  
 * \param[out] name_error_ptr           The address of the base name component 
 * in error.
 * \param[out] name_extension_error_ptr The address of the name component 
 * extension in error.
 * 
 * \return MQX_OK
 * \return MQX_INVALID_COMPONENT_BASE (Name component data are not valid.)
 * \return MQX_CORRUPT_QUEUE (An error was found.) 
 * \return MQX_INVALID_LWSEM (Results of _queue_test().)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)
 * 
 * \warning Disables and enables interrupts, cannot be called from ISR.
 * 
 * \see _name_add
 * \see _name_create_component
 * \see _name_delete   
 */ 
_mqx_uint _name_test
(
    pointer _PTR_ name_error_ptr,
    pointer _PTR_ name_extension_error_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    NAME_COMPONENT_STRUCT_PTR name_component_ptr;
    _mqx_uint                 result;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE3(KLOG_name_test, name_error_ptr, name_extension_error_ptr);

    *name_error_ptr = NULL;
    *name_extension_error_ptr = NULL;

    name_component_ptr = (NAME_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT];
    if (name_component_ptr == NULL)
    {
        _KLOGX2(KLOG_name_test, MQX_OK);
        return (MQX_OK);
    } /* Endif */

    result = _name_test_internal(name_component_ptr, name_error_ptr, name_extension_error_ptr);

    _KLOGX2(KLOG_name_test, result);
    return (result);
} /* Endbody */

/*!
 * \private
 * 
 * \brief This function tests the specified name component for consistency.
 * 
 * \param[in]  base_name_manager_ptr    The name component to test.
 * \param[out] name_error_ptr           The address of the base name component 
 * in error.
 * \param[out] name_extension_error_ptr The address of the name component 
 * extension in error.
 * 
 * \return MQX_OK
 * \return MQX_INVALID_COMPONENT_BASE (Name component data are not valid.)
 * \return MQX_CORRUPT_QUEUE (An error was found.) 
 * \return MQX_INVALID_LWSEM (Results of _queue_test().)
 * \return MQX_CANNOT_CALL_FUNCTION_FROM_ISR (Function cannot be called from an ISR.)   
 */            
_mqx_uint _name_test_internal
(
    NAME_COMPONENT_STRUCT_PTR base_name_manager_ptr,
    pointer _PTR_             name_error_ptr,
    pointer _PTR_             name_extension_error_ptr
)
{ /* Body */
    NAME_COMPONENT_STRUCT_PTR name_manager_ptr;
    pointer                   sem_in_err_ptr;
    pointer                   td_in_err_ptr;
    _mqx_uint                 result;
    _mqx_uint                 total;
    _mqx_uint                 i;

    if (base_name_manager_ptr == NULL)
    {
        return (MQX_OK);
    } /* Endif */

    *name_error_ptr = base_name_manager_ptr;
    if (base_name_manager_ptr->VALID != NAME_VALID)
    {
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */

    result = _lwsem_test(&sem_in_err_ptr, &td_in_err_ptr);
    if (result != MQX_OK)
    {
        return (result);
    } /* Endif */

    name_manager_ptr = base_name_manager_ptr;
    total = 0;
    _int_disable();
    while (name_manager_ptr != NULL)
    {
#if MQX_CHECK_VALIDITY
        if (name_manager_ptr->VALID != NAME_VALID)
        {
            _int_enable();
            *name_extension_error_ptr = name_manager_ptr;
            return (MQX_INVALID_COMPONENT_BASE);
        } /* Endif */
#endif /* MQX_CHECK_VALIDITY */
        for (i = 0; i < name_manager_ptr->NUMBER_IN_BLOCK; ++i)
        {
            if (name_manager_ptr->NAMES[i].NAME[0] != '\0')
            {
                ++total;
            } /* Endif */
        } /* Endfor */
        name_manager_ptr = name_manager_ptr->NEXT_TABLE;
    } /* Endwhile */
    _int_enable();

    if (total != base_name_manager_ptr->NUMBER)
    {
        *name_extension_error_ptr = name_manager_ptr;
        return (MQX_INVALID_COMPONENT_BASE);
    } /* Endif */

    return (MQX_OK);

} /* Endbody */
#endif /* MQX_USE_NAME */

/* EOF */
