/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Output.h"
#include <stdint.h>
//#include "pico/stdlib.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Serial {
namespace RPi {
namespace RP2xxx {
namespace Stdio {


//////////////////////
bool Output::write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    // Ignore write requests of ZERO bytes
    bytesWritten = 0;
    if ( maxBytes == 0 )
    {
        return true;
    }


    // perform the write
    const uint8_t* srcPtr = static_cast<const uint8_t*>( buffer );
    while ( maxBytes )
    {
        putchar_raw( *srcPtr );
        srcPtr++;
        maxBytes--;
        bytesWritten++;
    }
    
    // Flush output since the putchar() semantics do NOT flush the output (stdout waits for a newline to flush the output)
    stdio_flush();

    // NOTE: The method NEVER returns an error
    return true;
}


void Output::flush() noexcept
{
    stdio_flush();
}

bool Output::isEos( void ) noexcept
{
    return false;   // End-of-Stream has no meaning for the SDK's stdout
}

void Output::close() noexcept
{
    // Ignore - stdout can never be closed
}

} // end namespace
}
}
}
}
}
//------------------------------------------------------------------------------