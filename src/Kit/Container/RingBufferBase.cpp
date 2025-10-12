/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "RingBufferBase.h"
#include "Kit/System/Assert.h"
#include <string.h>

//------------------------------------------------------------------------------
namespace Kit { 
namespace Container { 

RingBufferBase::RingBufferBase( unsigned numMaxElements ) noexcept
    : m_readIdx( 0 )
    , m_writeIdx( 0 )
    , m_elements( numMaxElements )
{
}

bool RingBufferBase::remove( void* elemPtr, size_t elemSize, const void* srcRingBufMemory ) noexcept
{
    KIT_SYSTEM_ASSERT( elemPtr != nullptr );
    KIT_SYSTEM_ASSERT( elemSize > 0 );
    KIT_SYSTEM_ASSERT( srcRingBufMemory != nullptr );

    // Snapshot of ring buffer's indexes
    unsigned readIdx  = m_readIdx;
    unsigned writeIdx = m_writeIdx;

    // Trap - the Ring buffer is empty
    if ( readIdx == writeIdx )
    {
        return false;
    }

    // Copy the element data from the ring buffer to the provided pointer
    uint8_t* srcPtr = static_cast<uint8_t*>( const_cast<void*>( srcRingBufMemory ) ) + ( readIdx * elemSize );
    memcpy( elemPtr, srcPtr, elemSize );

    // Update the read index
    m_readIdx = ( readIdx + 1 ) % m_elements;
    return true;
}

bool RingBufferBase::add( const void* elemPtr, size_t elemSize, void* dstRingBufMemory ) noexcept
{
    KIT_SYSTEM_ASSERT( elemPtr != nullptr );
    KIT_SYSTEM_ASSERT( elemSize > 0 );
    KIT_SYSTEM_ASSERT( dstRingBufMemory != nullptr );

    // Snapshot of ring buffer's indexes
    unsigned readIdx  = m_readIdx;
    unsigned writeIdx = m_writeIdx;

    // Trap - the Ring buffer is full
    if ( ( writeIdx + 1 ) % m_elements == readIdx )
    {
        return false;
    }

    // Copy the element data to the provided pointer to the ring buffer
    uint8_t* dstPtr = static_cast<uint8_t*>( dstRingBufMemory ) + ( writeIdx * elemSize );
    memcpy( dstPtr, elemPtr, elemSize );

    // Update the write index
    m_writeIdx = ( writeIdx + 1 ) % m_elements;
    return true;
}

bool RingBufferBase::peekHead( void* elemPtr, size_t elemSize, const void* srcRingBufMemory ) noexcept
{
    KIT_SYSTEM_ASSERT( elemPtr != nullptr );
    KIT_SYSTEM_ASSERT( elemSize > 0 );
    KIT_SYSTEM_ASSERT( srcRingBufMemory != nullptr );

    // Snapshot of ring buffer's indexes
    unsigned readIdx  = m_readIdx;
    unsigned writeIdx = m_writeIdx;

    // Trap - the Ring buffer is empty
    if ( readIdx == writeIdx )
    {
        return false;
    }

    // Copy the element data from the ring buffer to the provided pointer
    uint8_t* srcPtr = static_cast<uint8_t*>( const_cast<void*>( srcRingBufMemory ) ) + ( readIdx * elemSize );
    memcpy( elemPtr, srcPtr, elemSize );

    return true;
}

bool RingBufferBase::peekTail( void* elemPtr, size_t elemSize, const void* srcRingBufMemory ) noexcept
{
    KIT_SYSTEM_ASSERT( elemPtr != nullptr );
    KIT_SYSTEM_ASSERT( elemSize > 0 );
    KIT_SYSTEM_ASSERT( srcRingBufMemory != nullptr );

    // Snapshot of ring buffer's indexes
    unsigned readIdx  = m_readIdx;
    unsigned writeIdx = m_writeIdx;

    // Trap - the Ring buffer is empty
    if ( readIdx == writeIdx )
    {
        return false;
    }

    // Get previous tail index
    unsigned prevTailIdx = writeIdx == 0 ? ( m_elements - 1 ) : ( writeIdx - 1 );

    // Copy the element data from the ring buffer to the provided pointer
    uint8_t* srcPtr = static_cast<uint8_t*>( const_cast<void*>( srcRingBufMemory ) ) + ( prevTailIdx * elemSize );
    memcpy( elemPtr, srcPtr, elemSize );

    return true;
}

void* RingBufferBase::peekNextRemoveItems( unsigned& dstNumFlatElements, size_t elemSize, const void* srcRingBufMemory ) const noexcept
{
    KIT_SYSTEM_ASSERT( srcRingBufMemory != nullptr );

    // Snapshot of ring buffer's indexes
    unsigned readIdx  = m_readIdx;
    unsigned writeIdx = m_writeIdx;

    // Check for empty
    if ( readIdx == writeIdx )
    {
        dstNumFlatElements = 0;
        return nullptr;
    }

    // Number of elements that can be removed without wrapping around the memory buffer
    unsigned currentElems = ( writeIdx - readIdx + m_elements ) % m_elements;
    dstNumFlatElements    = m_elements - readIdx;
    if ( dstNumFlatElements > currentElems )
    {
        dstNumFlatElements = currentElems;
    }

    uint8_t* srcPtr = static_cast<uint8_t*>( const_cast<void*>( srcRingBufMemory ) ) + ( readIdx * elemSize );
    return srcPtr;
}

void* RingBufferBase::peekNextAddItems( unsigned& dstNumFlatElements, size_t elemSize, const void* srcRingBufMemory ) const noexcept
{
    // Snapshot of ring buffer's indexes
    unsigned readIdx  = m_readIdx;
    unsigned writeIdx = m_writeIdx;

    // Check for full
    if ( ( ( writeIdx + 1 ) % m_elements ) == readIdx )
    {
        dstNumFlatElements = 0;
        return nullptr;
    }

    // Number of elements that can be added without wrapping around the memory buffer
    unsigned currentElems    = ( writeIdx - readIdx + m_elements ) % m_elements;
    unsigned totalAvailElems = m_elements - 1 - currentElems;
    dstNumFlatElements       = m_elements - writeIdx;
    if ( dstNumFlatElements > totalAvailElems )
    {
        dstNumFlatElements = totalAvailElems;
    }

    // Get a pointer to next 'available' address to add an element
    uint8_t* srcPtr = static_cast<uint8_t*>( const_cast<void*>( srcRingBufMemory ) ) + ( writeIdx * elemSize );
    return srcPtr;
}


} // end namespace
}
//------------------------------------------------------------------------------