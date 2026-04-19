/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Fletcher16.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Checksum {

///////////////////////////////////////
Fletcher16::Fletcher16() noexcept
{
    reset();
}


void Fletcher16::reset( void ) noexcept
{
    m_sum1 = 0;
    m_sum2 = 0;
}


///////////////////////////////////////
void Fletcher16::accumulate( const void* bytes, unsigned numbytes ) noexcept
{
    unsigned i;
    uint8_t* ptr = static_cast<uint8_t*>( const_cast<void*>( bytes ) );
    for ( i = 0; i < numbytes; i++, ptr++ )
    {
        m_sum1 = ( (uint16_t)m_sum1 + (uint16_t)( *ptr ) ) % 255;
        m_sum2 = ( (uint16_t)m_sum2 + (uint16_t)m_sum1 ) % 255;
    }
}

bool Fletcher16::finalize( void* destBuffer, unsigned destBufferSize ) noexcept
{
    if ( destBufferSize < sizeof( m_sum1 ) + sizeof( m_sum2 ) )
    {
        return false;
    }

    uint8_t  sumlo = 255 - ( ( (uint16_t)m_sum1 + (uint16_t)m_sum2 ) % 255 );
    uint8_t  sumhi = 255 - ( ( (uint16_t)m_sum1 + (uint16_t)sumlo ) % 255 );
    uint8_t* ptr   = static_cast<uint8_t*>( destBuffer );
    if ( ptr )
    {
        *ptr++ = sumlo;
        *ptr   = sumhi;
    }

    return true;
}

bool Fletcher16::isOkay( void ) noexcept
{
    return m_sum1 == 0 && m_sum2 == 0;
}

}  // end namespace
}
//------------------------------------------------------------------------------
