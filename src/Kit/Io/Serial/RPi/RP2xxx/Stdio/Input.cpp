/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Input.h"
#include <stdint.h>
// #include "pico/stdlib.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Serial {
namespace RPi {
namespace RP2xxx {
namespace Stdio {


///////////////////
Input::Input() noexcept
    : m_nextByte( PICO_ERROR_TIMEOUT )
{
}


//////////////////////
bool Input::read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
{
    // Ignore read requests of ZERO bytes
    bytesRead = 0;
    if ( numBytes == 0 )
    {
        return true;
    }

    // Get the cached next byte (when there is one)
    uint8_t* dstPtr = static_cast<uint8_t*>( buffer );
    if ( m_nextByte != PICO_ERROR_TIMEOUT )
    {
        *dstPtr = static_cast<uint8_t>( m_nextByte );
        dstPtr++;
        bytesRead++;
        numBytes--;
        m_nextByte = PICO_ERROR_TIMEOUT;
    }

    // Attempt to read the remaining request bytes
    while ( numBytes )
    {
        int byte = getchar_timeout_us( 0 );

        // No available bytes
        if ( byte == PICO_ERROR_TIMEOUT )
        {
            break;
        }

        *dstPtr = static_cast<uint8_t>( byte );
        dstPtr++;
        bytesRead++;
        numBytes--;
    }

    // NOTE: The method NEVER returns an error
    return true;
}


bool Input::available() noexcept
{
    // If there is cached next byte -->return immediately
    if ( m_nextByte == PICO_ERROR_TIMEOUT )
    {
        return true;
    }

    // Attempt a read...
    m_nextByte = getchar_timeout_us( 0 );
    return m_nextByte != PICO_ERROR_TIMEOUT;
}

bool Input::isEos( void ) noexcept
{
    return false;  // End-of-Stream has no meaning for the SDK's stdin
}

void Input::close() noexcept
{
    // stdin can never be closed - so pretty much ignore the close() call
    m_nextByte = PICO_ERROR_TIMEOUT;
}

}  // end namespace
}
}
}
}
}
//------------------------------------------------------------------------------