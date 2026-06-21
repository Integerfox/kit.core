/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "IInput.h"
#include "Kit/Io/Types.h"
#include "Kit/System/Assert.h"
#include <cstdint>


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {

///////////////////
bool IInput::read( char& c ) noexcept
{
    Kit::Type::SSize_T bytesRead = 0;
    bool        result    = true;
    do
    {
        result = read( &c, sizeof( c ), bytesRead );
    }
    while ( result && bytesRead == 0 );
    return result;
}


bool IInput::read( Kit::Text::IString& destString ) noexcept
{
    Kit::Type::SSize_T bytesRead = 0;
    int         maxlen    = 0;
    char*       buffer    = destString.getBuffer( maxlen );
    Kit::Type::SSize_T len       = static_cast<Kit::Type::SSize_T>( maxlen );
    bool        result    = read( buffer, len, bytesRead );
    buffer[bytesRead]     = '\0';
    return result;
}

bool IInput::read( Kit::Text::IString& destString, Kit::Type::SSize_T numBytesToRead ) noexcept
{
    KIT_SYSTEM_ASSERT( numBytesToRead >= 0 );

    // Housekeeping
    int         maxlen = 0;
    char*       buffer = destString.getBuffer( maxlen );
    Kit::Type::SSize_T len    = static_cast<Kit::Type::SSize_T>( maxlen );
    if ( numBytesToRead > len )
    {
        numBytesToRead = len;  // Limit numBytesToRead to max allowed length of the string
    }

    // Read the bytes
    bool result            = read( buffer, numBytesToRead );
    buffer[numBytesToRead] = '\0';  // Ensure the string is terminated

    return result;
}

bool IInput::read( void* buffer, Kit::Type::SSize_T numBytesToRead ) noexcept
{
    KIT_SYSTEM_ASSERT( buffer != nullptr );
    KIT_SYSTEM_ASSERT( numBytesToRead >= 0 );

    // Reading zero bytes is a nop
    if ( numBytesToRead > 0 )
    {
        // Housekeeping
        Kit::Type::SSize_T bytesRead = 0;
        Kit::Type::SSize_T remaining = numBytesToRead;
        auto*       ptr       = static_cast<uint8_t*>( buffer );

        // Keep reading until all requested bytes are read
        while ( remaining )
        {
            if ( !read( ptr, remaining, bytesRead ) )
            {
                return false;
            }
            ptr       += bytesRead;
            remaining -= bytesRead;
        }
    }

    // If I get here, all requested bytes were read
    return true;
}

}  // end namespace
}
//------------------------------------------------------------------------------