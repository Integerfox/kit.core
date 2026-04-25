/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Md5.h"
#include <string.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Checksum {

///////////////////////////////////////
Md5::Md5() noexcept
{
    reset();
}

void Md5::reset( void ) noexcept
{
    md5_init( &m_state );
}


///////////////////////////////////////
void Md5::accumulate( const void* bytes, unsigned numbytes ) noexcept
{
    // Skip if no data or bad pointer
    if ( bytes == nullptr || numbytes == 0 )
    {
        return;
    }

    md5_append( &m_state, (const md5_byte_t*)bytes, numbytes );
}

bool Md5::finalize( void* destBuffer, unsigned destBufferSize  ) noexcept
{
    if ( destBufferSize < eDIGEST_LEN || destBuffer == nullptr )
    {
        return false;
    }

    Digest_T result;
    md5_finish( &m_state, result );
    memcpy( destBuffer, result, eDIGEST_LEN );
    return true;
}

} // end namespace
}
//------------------------------------------------------------------------------