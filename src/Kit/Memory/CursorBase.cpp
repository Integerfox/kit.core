/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "CursorBase.h"
#include "Kit/System/Assert.h"
#include <string.h>


using namespace Kit::Memory;


//////////////////////////////////////
CursorBase::CursorBase( void* srcBuffer, size_t srcBufferSize, size_t startingOffset )
    : m_baseBufPtr( static_cast<uint8_t*>( srcBuffer ) )
    , m_bufPtr( m_baseBufPtr )
    , m_bufferSize( srcBufferSize )
    , m_bytesRemaining( srcBufferSize )
{
    KIT_SYSTEM_ASSERT( srcBuffer != nullptr );
    KIT_SYSTEM_ASSERT( srcBufferSize != SIZE_MAX );
    setPosition( startingOffset );
}


//////////////////////////////////////
bool CursorBase::errorOccurred() const noexcept
{
    return m_bytesRemaining == SIZE_MAX;
}

bool CursorBase::setPosition( size_t newOffset ) noexcept
{
    if ( newOffset >= m_bufferSize )
    {
        m_bytesRemaining = SIZE_MAX;  // Put the cursor in a error state
        return false;
    }

    m_bufPtr         = m_baseBufPtr + newOffset;
    m_bytesRemaining = m_bufferSize - newOffset;
    return true;
}


bool CursorBase::skip( size_t numBytes )
{
    // Check for errors
    if ( m_bytesRemaining == SIZE_MAX || numBytes > m_bytesRemaining )
    {
        m_bytesRemaining = SIZE_MAX;
        return false;
    }

    m_bufPtr         += numBytes;
    m_bytesRemaining -= numBytes;
    return true;
}

bool CursorBase::readAndAdvance( void* dstPtr, size_t dstSize ) noexcept
{
    // Check for errors
    if ( m_bytesRemaining == SIZE_MAX || dstPtr == nullptr || dstSize > m_bytesRemaining )
    {
        m_bytesRemaining = SIZE_MAX;
        return false;
    }

    // Get the data
    memcpy( dstPtr, m_bufPtr, dstSize );
    m_bufPtr         += dstSize;
    m_bytesRemaining -= dstSize;
    return true;
}

bool CursorBase::writeAndAdvance( const void* srcPtr, size_t srcSize ) noexcept
{
    // Check for errors
    if ( m_bytesRemaining == SIZE_MAX || srcPtr == nullptr || srcSize > m_bytesRemaining )
    {
        m_bytesRemaining = SIZE_MAX;
        return false;
    }

    // Write the data
    memcpy( m_bufPtr, srcPtr, srcSize );
    m_bufPtr         += srcSize;
    m_bytesRemaining -= srcSize;
    return true;
}

bool CursorBase::fill( size_t numBytes, uint8_t fillByte ) noexcept
{
    // Check for errors
    if ( m_bytesRemaining == SIZE_MAX || numBytes > m_bytesRemaining )
    {
        m_bytesRemaining = SIZE_MAX;
        return false;
    }

    // Write the data
    memset( m_bufPtr, fillByte, numBytes );
    m_bufPtr         += numBytes;
    m_bytesRemaining -= numBytes;
    return true;
}
