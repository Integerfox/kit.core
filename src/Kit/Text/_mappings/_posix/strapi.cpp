/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <string.h>
#include <ctype.h>
#include "Kit/Text/strapi.h"


//////////////
char* strupr_MAP( char *string )
{
    if ( string )
    {
        char* ptr = string;
        for ( ; *ptr; ptr++ )
        {
            *ptr = toupper( *ptr );
        }
    }
    return string;
}

char* strlwr_MAP( char *string )
{
    if ( string )
    {
        char* ptr = string;
        for ( ; *ptr; ptr++ )
        {
            *ptr = tolower( *ptr );
        }
    }
    return string;
}


//////////////
char* strnset_MAP( char *string, int c, size_t count )
{
    if ( string )
    {
        size_t i;
        char*  ptr = string;
        for ( i=0; i < count && *ptr; ptr++, i++ )
        {
            *ptr = (char) c;
        }
    }
    return string;
}


///////////////
char* strrev_MAP( char *string )
{
    if ( string )
    {
        size_t len = strlen( string );
        size_t i   = 0;
        char*  ptr = string;
        char*  end = string + len - 1;
        while ( i < len / 2 )
        {
            char temp = *end;
            *end      = *ptr;
            *ptr      = temp;
            ptr++;
            end--;
            i++;
        }
    }
    return string;
}


