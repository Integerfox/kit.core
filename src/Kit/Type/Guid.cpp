/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Guid.h"
#include "Kit/Text/Parse.h"
#include <string.h>

using namespace Kit::Type;
bool Guid_T::operator ==( const Guid_T other ) const
{
    return memcmp( block, other.block, sizeof( block ) ) == 0;
}

bool Guid_T::toString( Kit::Text::IString& formattedOutput, bool withBraces )
{
    formattedOutput.format( "%s%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%s",
                            withBraces ? "{" : "",
                            block[0],
                            block[1],
                            block[2],
                            block[3],
                            block[4],
                            block[5],
                            block[6],
                            block[7],
                            block[8],
                            block[9],
                            block[10],
                            block[11],
                            block[12],
                            block[13],
                            block[14],
                            block[15],
                            withBraces ? "}" : "" );

    return formattedOutput.truncated() == false;
}

bool Guid_T::fromString( const char* stringGuid )
{
    // Check for bad input
    if ( stringGuid == nullptr )
    {
        return false;
    }


    // Check for leading/trailing '{}'
    if ( stringGuid[0] == '{' )
    {
        if ( stringGuid[KIT_TYPE_GUID_MAX_FORMATTED_WITH_BRACES_LENGTH - 1] != '}' )
        {
            return false;
        }
        stringGuid++;
    }

    // First block of 8 characters
    uint8_t* binaryOutput = block;
    if ( !Kit::Text::Parse::asciiHexUnsafe( stringGuid, 8, binaryOutput ) )
    {
        return false;
    }
    stringGuid   += 8;
    binaryOutput += 4;

    // The 3 blocks of 4 characters
    for ( int i=0; i < 3; i++ )
    {
        // Check for '-'
        if ( *stringGuid != '-' )
        {
            return false;
        }
        stringGuid++;

        // Check for block of 4
        if ( !Kit::Text::Parse::asciiHexUnsafe( stringGuid, 4, binaryOutput ) )
        {
            return false;
        }

        // Advance to the next block
        stringGuid   += 4;
        binaryOutput += 2;
    }

    // The last Block of 12
    if ( *stringGuid != '-' )
    {
        return false;
    }
    stringGuid++;
    if ( !Kit::Text::Parse::asciiHexUnsafe( stringGuid, 12, binaryOutput ) )
    {
        return false;
    }

    return true;
}
