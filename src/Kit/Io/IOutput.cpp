/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "IOutput.h"
#include "Kit/Io/Types.h"
#include <string.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {

//////////////////////
bool IOutput::write( char c ) noexcept
{
    auto* ptr = static_cast<const void*>( &c );
    ByteCount_T numBytes = sizeof( c );
    return write( ptr, numBytes );
}


bool IOutput::write( const char* string ) noexcept
{
    ByteCount_T numBytes = strlen( string );
    return write( string, numBytes );
}


bool IOutput::write( const Kit::Text::IString& string ) noexcept
{
    ByteCount_T numBytes = string.length();
    return write( string(), numBytes );
}


bool IOutput::write( Kit::Text::IString& formatBuffer, const char* format, ... ) noexcept
{
    va_list ap;
    va_start( ap, format );
    bool result = vwrite( formatBuffer, format, ap );
    va_end( ap );
    return result;
}

bool IOutput::vwrite( Kit::Text::IString& formatBuffer, const char* format, va_list ap ) noexcept
{
    formatBuffer.vformat( format, ap );
    ByteCount_T numBytes = formatBuffer.length();
    return write( formatBuffer(), numBytes );
}

bool IOutput::write( const void* buffer, ByteCount_T numBytes ) noexcept
{
    ByteCount_T written = 0;
    auto*       ptr     = static_cast<const uint8_t*>( buffer );

    // Loop until all data is written
    while ( numBytes > 0 )
    {
        if ( !write( ptr, numBytes, written ) )
        {
            return false;
        }
        numBytes -= written;
        ptr      += written;
    }

    // If I get here, all data has been written
    return true;
}

}  // end namespace
}
//------------------------------------------------------------------------------
