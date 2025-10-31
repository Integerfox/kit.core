/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *
 *----------------------------------------------------------------------------*/

#include "strapi.h"
#include <ctype.h>



////////////////
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

/*----------------------------------------------------------------------------
 *
 * The following functions are adapted from the lwIP TCP/IP stack and have
 * following copyright and license:
 *
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Simon Goldschmidt
 *
 *----------------------------------------------------------------------------*/

int strcasecmp_MAP( const char* str1, const char* str2 )
{
    char c1, c2;

    do
    {
        c1 = *str1++;
        c2 = *str2++;
        if ( c1 != c2 )
        {
            char c1_upc = c1 | 0x20;
            if ( ( c1_upc >= 'a' ) && ( c1_upc <= 'z' ) )
            {
                // characters are not equal an one is in the alphabet range:
                // downcase both chars and check again
                char c2_upc = c2 | 0x20;
                if ( c1_upc != c2_upc )
                {
                    // still not equal
                    return c1 - c2;
                }
            }
            else
            {
                // characters are not equal but none is in the alphabet range
                return c1 - c2;
            }
        }
    }
    while ( c1 != 0 );
    return 0;
}

int strncasecmp_MAP( const char* str1, const char* str2, size_t len )
{
    char c1, c2;

    do
    {
        c1 = *str1++;
        c2 = *str2++;
        if ( c1 != c2 )
        {
            char c1_upc = c1 | 0x20;
            if ( ( c1_upc >= 'a' ) && ( c1_upc <= 'z' ) )
            {
                // characters are not equal an one is in the alphabet range:
                // downcase both chars and check again
                char c2_upc = c2 | 0x20;
                if ( c1_upc != c2_upc )
                {
                    // still not equal
                    return c1 - c2;
                }
            }
            else
            {
                // characters are not equal but none is in the alphabet range
                return c1 - c2;
            }
        }
        len--;
    }
    while ( ( len != 0 ) && ( c1 != 0 ) );
    return 0;
}
