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