/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */
/*-----------------------------------------------------------------------------
 * FULL DISCLOSURE: This code is based the LTOSTR.C implementation by Jerry
 * Coffin which is in the public domain.  The original code snippet can be
 * found at:
 *
 *   https://github.com/vonj/snippets/blob/master/ltostr.c
 *
 *----------------------------------------------------------------------------*/


#include "ToString.h"
#include <string.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace Text {


const char* ToString::convert_( uint64_t num, char* dstString, size_t maxChars, unsigned base, char padChar, bool isNegative ) noexcept
{
    // Error check the base argument and null destination string
    if ( base < 2 || base > 36 || dstString == nullptr )
    {
        return nullptr;
    }

    // When convert a negative value, I need to leave room for the minus sign
    size_t sign = 0;
    if ( isNegative )
    {
        num  = ~num + 1;  // Convert to positive
        sign = 1;
    }

    // Null terminate the string
    dstString[--maxChars] = '\0';

    // Handle  special case: original number is zero
    if ( num == 0 )
    {
        dstString[--maxChars] = '0';
    }

    // Handle special case: original number is INT64_MIN
    if ( isNegative && num == (uint64_t)INT64_MIN )
    {
        // Note: The negative sign is added later
        static const char* minStr = "9223372036854775808";
        size_t             len    = strlen( minStr );
        if ( len > (maxChars + sign) )
        {
            return nullptr;
        }

        // Copy the string so it is right justified within the destination buffer
        size_t offset = maxChars + sign - len;
        memcpy( dstString + offset, minStr, len );
        maxChars = offset;
    }

    // Convert the number
    else
    {
        // Conversion is done LSB first
        while ( num != 0 && maxChars > sign )
        {
            char remainder = (char)( num % base );
            if ( remainder <= 9 )
            {
                dstString[--maxChars] = remainder + '0';
            }
            else
            {
                dstString[--maxChars] = remainder - 10 + 'A';
            }

            num /= base;
        }

        // Not enough space in the destination string
        if ( num != 0 )
        {
            return nullptr;
        }
    }

    // Add the minus when needed
    if ( isNegative )
    {
        dstString[--maxChars] = '-';
    }

    // Add pad character(s)
    if ( maxChars > 0 )
    {
        memset( dstString, padChar, maxChars );
    }

    // Return the 'left justified' string
    return dstString + maxChars;
}

}  // end namespaces
}
//------------------------------------------------------------------------------