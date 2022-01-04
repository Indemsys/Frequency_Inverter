/**HEADER********************************************************************
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
* $FileName: mfs_parse.c$
* $Version : 3.8.16.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains functions to parse and manipulate filenames.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_strupr
* Returned Value   :  void
* Comments  :
*      Take a string and convert it to uppercase
*END*---------------------------------------------------------------------*/

void MFS_strupr
    (
    char_ptr string /*[IN]  the string to be converted.*/
    )
{
    while ( *string != '\0' )
    {
        if ( *string >= 'a' &&  *string <= 'z' )
        {
            *string -= 32;
        }
        string++;
    }   
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Parse_pathname
* Returned Value   :  error code
* Comments  :
*      Take a pathname and convert it into its directory name and
*      filename components.
*END*---------------------------------------------------------------------*/
_mfs_error MFS_Parse_pathname
    (
    char_ptr        dirname_ptr,    /*[IN/OUT] pointer to buffer where directory is to be written */
    char_ptr        filename_ptr,   /*[IN/OUT] pointer to buffer where filename is to be written */
    char_ptr        pathname_ptr    /*[IN] pathname include '\', not including drive specification */
    )
{
    _mfs_error error_code;
    char_ptr   src, p, temp;

    if ( pathname_ptr )
    {
        error_code = MFS_alloc_path(&temp);
        if ( error_code!=MFS_NO_ERROR )
        {
            return( error_code );
        }

        src           = pathname_ptr;
        *filename_ptr = '\0';
        temp[0]       = '\0';
        error_code    = MFS_NO_ERROR;

        if ( *src == '\\' || *src == '/' )
        {
            *dirname_ptr++ = '\\';
            src++;
        }

        *dirname_ptr = '\0';
        p = dirname_ptr;

        while ( *src )
        {
            src = MFS_Parse_next_filename (src, temp);
            if ( src == NULL )
            {
                /*
                ** if the next filename was not a valid filename
                */
                error_code = MFS_PATH_NOT_FOUND;
                break;
            }
            else if ( *src )
            {
                if ( *dirname_ptr )
                {
                    /*
                    ** if this is not the first filename in the path,
                    */
                    strcat (dirname_ptr, "\\");
                }
                p = p + strlen(p);
                strcat (dirname_ptr, temp);
                /*
                ** If any of the directory_names is invalid, return.
                */
                if ( MFS_lfn_dirname_valid(p) == FALSE )
                {
                    src = NULL;
                    error_code = MFS_PATH_NOT_FOUND;
                    break;
                }
            }
        }  

        /*
        ** last read word shall be a filename
        */
        if ( src )
        {
            strcpy(filename_ptr, temp);
        }
        MFS_free_path(temp);      
    }
    else
    {
        error_code = MFS_INVALID_PARAMETER;
    }  

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Wildcard_match
* Returned Value   : boolean ... true if the filename matches the wildcard
* Comments  :   Checks if a filename matches a given wildcard. 
*               (8.3 filenames only)
*
*END*---------------------------------------------------------------------*/

boolean MFS_Wildcard_match
    (
    char_ptr              wildcard_ptr, /*[IN] the wildcard (expanded to 11 characters, no dot) */
    char_ptr              filename_ptr  /*[IN] the filename (expanded to 11 characters, no dot) */
    )
{
    uint_16        k;
    register char  c;
    boolean        found;

    found = TRUE;

    for ( k = 0; k < 11; k++ )
    {
        c = *wildcard_ptr;
        if ( (c != *filename_ptr) && (c != '?') )
        {
            found = FALSE;
            break;
        }
        wildcard_ptr++;
        filename_ptr++;
    }  

    return(found);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Expand_wildcard
* Returned Value   : void
* Comments  :   Expands a wildcard (AB*.WK?) to a 11 char string dotless,
*         and NOT '\0' TERMINATED.
*
*END*---------------------------------------------------------------------*/

void MFS_Expand_wildcard
    (
    char_ptr     wildcard_ptr,         /*[IN] the wildcard (dos filename, no spaces 1..12 chars) */
    char_ptr     expanded_filename_ptr /*[IN] the filename (expanded to 11 characters, no dot) */
    )
{
    uint_16     k;
    uchar       cw;

    for ( k = 0; k < 8; k++ )
    {
        cw = *wildcard_ptr;
        if ( cw == '*' )
        {
            *expanded_filename_ptr = '?';
        }
        else if ( (cw == '.') || (cw == '\0') )
        {
            *expanded_filename_ptr = ' ';
        }
        else
        {
            cw = CAPITALIZE(cw);
            *expanded_filename_ptr = cw;
            wildcard_ptr++;
        }  
        expanded_filename_ptr++;
    }  

    /*
    ** Skip everything else, up to the dot or end of wildcard.
    */
    while ( *wildcard_ptr && (*wildcard_ptr != '.') )
    {
        wildcard_ptr++;
    }  

    if ( *wildcard_ptr == '.' )
    {
        wildcard_ptr++;
    }

    for ( k = 0; k < 3; k++ )
    {
        cw = *wildcard_ptr;
        if ( cw == '*' )
        {
            *expanded_filename_ptr = '?';
        }
        else if ( (cw == '.') || (cw == '\0') )
        {
            *expanded_filename_ptr = ' ';
        }
        else
        {
            cw = CAPITALIZE(cw);
            *expanded_filename_ptr = cw;
            wildcard_ptr++;
        }  
        expanded_filename_ptr++;
    }  
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Expand_dotfile
* Returned Value   :  void
* Comments  :
*   Put a dot-format filename in non-dot format. Assumes a valid DOS
*   filename.
*   It maps non_dot_file[0] from 0xE5 to 0x05 if necessary.
*
*END*---------------------------------------------------------------------*/
void MFS_Expand_dotfile
    (
    char_ptr        dot_file,       /*[IN] pointer to the file's dot_name */
    char_ptr        non_dot_file    /*[IN] pointer to the file's non-dot_name  */
    )
{
    uint_16     k;
    uchar       c;
    char_ptr    save_non_dot_file;

    save_non_dot_file = non_dot_file;

    if ( *dot_file == '.' )
    {
        *non_dot_file++ = *dot_file++;
        for ( k = 0; k < 10; k++ )
        {
            c = *dot_file;
            if ( c )
            {
                *non_dot_file++ = CAPITALIZE(c);
                dot_file++;
            }
            else
            {
                *non_dot_file++ = ' ';
            }  
        }  
        return;                                     
    }

    for ( k = 0; k < 8; k++ )
    {
        if ( *dot_file && *dot_file != '.' )
        {
            c = *dot_file;
            *non_dot_file++ = CAPITALIZE(c);
            dot_file++;
        }
        else
        {
            *non_dot_file++ = ' ';
        }  
    }  

    if ( *dot_file == '.' )
    {
        dot_file++;
    }
    for ( k = 0; k < 3; k++ )
    {
        c = *dot_file;
        if ( c && c != '.' )
        {
            *non_dot_file++ = CAPITALIZE(c);
            dot_file++;
        }
        else
        {
            *non_dot_file++ = ' ';
        }  
    }  

    if ( *save_non_dot_file == (char) MFS_DEL_FILE )
    {
        *save_non_dot_file = 0x05;
    }

}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Compress_nondotfile
* Returned Value   :  void
* Comments  :
*   Put a non-dot-format filename in dot format. Assumes a valid DOS
*   filename.
*   It maps non_dot_file[0] from 0x05 to 0xE5 if necessary.
*
*END*---------------------------------------------------------------------*/

void MFS_Compress_nondotfile
    (
    char_ptr        nondot_file,    /*[IN] pointer to the file's non_dot_name */
    char_ptr        dot_file        /*[IN] pointer to the file's dot_name  */
    )
{
    uint_16  k;
    char_ptr s_ptr;
    char_ptr d_ptr;
    char_ptr saved_d_ptr = NULL;
    boolean  save = TRUE;

    s_ptr = nondot_file;
    d_ptr = dot_file;

    for ( k = 0; k < 8; k++ )
    {
        *d_ptr = *s_ptr;
        if ( *s_ptr == ' ' )
        {
            if ( save )
            {
                saved_d_ptr = d_ptr;
                save = FALSE;
            }
        }
        else
        {
            save = TRUE;
        }  
        d_ptr++; s_ptr++;
    }  

    if ( save )
    {
        *d_ptr = '.';
        d_ptr++;
    }
    else
    {
        *saved_d_ptr = '.';
        d_ptr = saved_d_ptr + 1;
        save  = TRUE;
    }  


    for ( k = 0; k < 3; k++ )
    {
        *d_ptr = *s_ptr;
        if ( *s_ptr == ' ' )
        {
            if ( save )
            {
                saved_d_ptr = d_ptr;
                save = FALSE;
            }
        }
        else
        {
            save = TRUE;
        }  
        d_ptr++; s_ptr++;
    }  

    if ( save )
    {
        *d_ptr = '\0';
    }
    else
    {
        *saved_d_ptr = '\0';
    }  
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Filename_valid
* Returned Value   :  boolean (TRUE if valid DOS filename.
* Comments  :
*        Checks for length, invalid characters.
*END*---------------------------------------------------------------------*/

boolean MFS_Filename_valid
    (
    char_ptr        filename   /*[IN] filename to validate */
    )
{
    char c;
    char *s;

    int name_len;
    int ext_len;
    int dot_found;

    if (filename == NULL)
    {
       return FALSE;
    }
    
    s = filename;    

    name_len = 0;
    ext_len = 0;
    dot_found = 0;    
    
    for (c = *s; c != '\0'; c = *(++s)) {

        if (c == '.')
        {
            if (dot_found)
            {
                /* dot may occur only once */
                return FALSE;
            }

            if (name_len == 0)
            {
                /* blank filename not allowed */
                return FALSE;
            }

            dot_found++;

            continue;
        }
    
        if ( c == ' ' || c == '\"' || c == '*' || c == '+' ||
             c == ',' || c == '/'  || c == ':' || c == ';' ||
             c == '<' || c == '='  || c == '>' || c == '?' ||
             c == '[' || c == '\\' || c == ']' || c == '|' )
        {
            return FALSE;
        }
        
        if (!dot_found)
        {
            if (++name_len > 8)
            {
                return FALSE; /* name too long */
            }
        }
        else
        {
            if (++ext_len > 3)
            {
                return FALSE; /* extension too long */
            }
        }
    }
    
    /* only non-blank filenames are valid */    
    return (name_len > 0);
}    


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Dirname_valid
* Returned Value   :  boolean - TRUE if valid DOS directory name.
* Comments  :
*        Allows . and ..
*END*---------------------------------------------------------------------*/

boolean MFS_Dirname_valid
    (
    char_ptr        filename    /*[IN] filename to validate */
    )
{

    if ( *filename != '.' )
    {
        return(MFS_Filename_valid(filename));
    }
    else
    {
        if ( (*(filename + 1) == '\0') ||
            ((*(filename+1) == '.') && !*(filename+2)) )
        {
            return(TRUE);
        }
    }  

    return(FALSE);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Attribute_match
* Returned Value   :  boolean (TRUE if the attributes match)
* Comments  :
*
*END*---------------------------------------------------------------------*/
boolean MFS_Attribute_match
    (
    uchar       dattr,  /*[IN]  the attribute of the existing file on disk */
    uchar       wattr   /*[IN]  the attribute we are trying to find out if it matches */
    )
{
    boolean   match;

    if ( wattr & MFS_ATTR_ANY )
    {
        match = TRUE;
    }
    else if ( wattr == MFS_ATTR_LFN )
    {
        if ( dattr == MFS_ATTR_LFN )
        {
            match = TRUE;
        }
        else
        {
            match = FALSE;
        }  
    }
    else if ( wattr == MFS_SEARCH_VOLUME )
    {
        if ( dattr == MFS_SEARCH_VOLUME )
        {
            match = TRUE;
        }
        else
        {
            match = FALSE;
        }  
    }
    else
    {
        if ( wattr & ATTR_EXCLUSIVE )
        {
            if ( dattr == (wattr & (~ATTR_EXCLUSIVE)) )
            {
                match = TRUE;
            }
            else
            {
                match = FALSE;
            }  
        }
        else
        {
            /* MFS_SEARCH_NORMAL returns all non-hidden, non-system files or directories */
            if (wattr == MFS_SEARCH_NORMAL)
            {
                if (dattr & (MFS_ATTR_HIDDEN_FILE | MFS_ATTR_SYSTEM_FILE))
                {
                    match = FALSE;
                }
                else
                {
                    match = TRUE;
                }
            }
            /* MFS_SEARCH_SUBDIR returns all non-hidden, non-system directories */
            else if (wattr == MFS_SEARCH_SUBDIR)
            {
                if ((dattr & (MFS_ATTR_DIR_NAME | MFS_ATTR_HIDDEN_FILE | MFS_ATTR_SYSTEM_FILE)) == MFS_ATTR_DIR_NAME)
                {
                    match = TRUE;
                }
                else
                {
                    match = FALSE;
                }
            }
            /* otherwise wattr must be subset of dattr */
            else
            {
                if ((dattr | (wattr & (MFS_ATTR_READ_ONLY | MFS_ATTR_HIDDEN_FILE | MFS_ATTR_SYSTEM_FILE | MFS_ATTR_DIR_NAME | MFS_ATTR_ARCHIVE))) == dattr)
                {
                    match = TRUE;
                }
                else
                {
                    match = FALSE;
                }
            }
        }  
    }  

    return(match);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Parse_next_filename
* Returned Value   :  char_ptr to where it left off (non / or \ char)
* Comments  :
*        Reads the first filename in source string, up to '\' ,'/' or '.'
*        If the filename we're reading is invalid, return NULL
*END*---------------------------------------------------------------------*/

char_ptr MFS_Parse_next_filename
    (
    char_ptr   src, /*[IN]  the pathanme */
    char_ptr   out  /*[IN/OUT]  the filename just read in */
    )
{
    uint_32     i;

    if ( out == NULL )
    {
        return(NULL);
    }

    i = 0;

    while ( *src && *src != '/' && *src != '\\' )
    {
        *out++ = *src++;
        i++;
        if ( i > FILENAME_SIZE )
        {
            return NULL;
        }
    }  

    *out = '\0';
    if ( *src == '\\' || *src == '/' )
    {
        src++;
    }

    return(src);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Is_dot_directory
* Returned Value   :  the number of dots in the filename
* Comments  :
*     It checks if the filename is on of '.' or '..' and returns 1 or 2
*     respectively. Otherwise, it returns 0
*
*END*---------------------------------------------------------------------*/

uint_16 MFS_Is_dot_directory
    (
    char_ptr    filename    /*[IN]  pointer to the filename to be checked.*/
    )
{
    if ( *filename++ == '.' )
    {
        if ( *filename )
        {
            if ( *filename++ == '.' )
            {
                if ( !*filename )
                {
                    return(2);
                }
            }
        }
        else
        {
            return(1);
        }  
    }

    return(0);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_strcmp
* Returned Value   :  TRUE if the two string are identical
* Comments  :
*     It stops at '\0' or at the given limit
*END*---------------------------------------------------------------------*/

boolean MFS_strcmp 
    (
    char_ptr    string1,    /* [IN] the first string to compare */
    char_ptr    string2,    /*[IN] the second string to compare */      
    uint_32     limit       /* [IN] the maximun number of chars to count */
    ) 
{
    while ( *string1 && *string2 && *string1 == *string2 && limit-- )
    {
        string1++;
        string2++;     
    }   

    if ( *string1 == *string2 )
    {
        return( TRUE );
    }
    else
    {
        return( FALSE );      
    }  

}
