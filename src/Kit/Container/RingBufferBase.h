#ifndef KIT_CONTAINER_RINGBUFFERBASE_H_
#define KIT_CONTAINER_RINGBUFFERBASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Assert.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

///
namespace Kit {
///
namespace Container {

/** This 'implementation' class provide the base functionality for a ring buffer.

    The Ring buffer is ISR/Thread safe as long as following constraints are met:
    - Writing to an 'unsigned' data type is an atomic machine instruction.
    - There is SINGLE producer (add) and a SINGLE consumer (remove)

    NOTE: This class is not intended to be used directly. A child class is
          required to provide the ring buffer memory and type safety.
 */
class RingBufferBase
{
protected:
    /// Constructor
    RingBufferBase( unsigned numMaxElements )
        : m_readIdx( 0 )
        , m_writeIdx( 0 )
        , m_elements( numMaxElements )
    {
    }

public:
    /// This method returns true if the Ring Buffer is empty
    bool isEmpty( void ) const noexcept
    {
        return m_readIdx == m_writeIdx;
    }

    /// This method returns true if the Ring Buffer is full
    bool isFull( void ) const noexcept
    {
        return ( ( m_writeIdx + 1 ) % m_elements ) == m_readIdx;
    }

    /// This method returns the maximum number of items that can be stored in the Ring buffer.
    unsigned getMaxItems( void ) const noexcept
    {
        return m_elements - 1;
    }

public:
    /** This method resets the underlying read/write indexes, i.e. effectively
        clearing the Ring Buffer contents.  This method can ONLY BE USED when
        the Ring Buffer is NOT in use. It is the application's responsibility
        for ensuring this condition is met.
     */
    void clearTheBuffer() noexcept
    {
        m_readIdx = m_writeIdx = 0;
    }

protected:
    /** Removes the first item in the Ring buffer. The contents of the
        removed item will be copied into the 'elemPtr' argument. The method
        returns true if the operation was successful; else false is
        returned, i.e. the Ring buffer is/was empty.
     */
    bool remove( void* elemPtr, size_t elemSize, const void* srcRingBufMemory ) noexcept
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
        uint8_t* srcPtr = ( (uint8_t*)srcRingBufMemory ) + ( readIdx * elemSize );
        memcpy( elemPtr, srcPtr, elemSize );

        // Update the read index
        m_readIdx = ( readIdx + 1 ) % m_elements;
        return true;
    }

    /** Add a new item to the Ring Buffer as the last item in the buffer. The
        contents of the source item (aka 'elemPtr') will be copied into the
        Ring Buffer. Returns true if the operation was successful; else false
        is returned, i.e. the Buffer was full prior to the attempted add().
     */
    bool add( const void* elemPtr, size_t elemSize, void* dstRingBufMemory ) noexcept
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
        uint8_t* dstPtr = ( (uint8_t*)dstRingBufMemory ) + ( writeIdx * elemSize );
        memcpy( dstPtr, elemPtr, elemSize );

        // Update the write index
        m_writeIdx = ( writeIdx + 1 ) % m_elements;
        return true;
    }

protected:
    /** This method inspects the head element of the Ring buffer without removing it.
        The contents of the head element will be copied into the 'elemPtr' argument.
        The method returns true if the operation was successful; else false is
        returned, i.e. the Ring buffer is empty.
     */
    bool peekHead( void* elemPtr, size_t elemSize, const void* srcRingBufMemory ) noexcept
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
        uint8_t* srcPtr = ( (uint8_t*)srcRingBufMemory ) + ( readIdx * elemSize );
        memcpy( elemPtr, srcPtr, elemSize );

        return true;
    }

    /** This method inspects the tail element of the Ring buffer without removing it.
        The contents of the tail element will be copied into the 'elemPtr' argument.
        The method returns true if the operation was successful; else false is
        returned, i.e. the Ring buffer is empty.
     */
    bool peekTail( void* elemPtr, size_t elemSize, const void* srcRingBufMemory ) noexcept
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
        uint8_t* srcPtr = ( (uint8_t*)srcRingBufMemory ) + ( prevTailIdx * elemSize );
        memcpy( elemPtr, srcPtr, elemSize );

        return true;
    }

protected:
    /** Index of the next item to be read from the ring.  This variable is only
        updated when a read operation is performed.  In addition, the variable
        is atomically updated.
     */
    volatile unsigned m_readIdx;

    /** Index of the next item to be added to the ring.  This variable
        is only updated when a add operation is performed.  In addition,
        the variable is atomically updated.
     */
    volatile unsigned m_writeIdx;

    /// Maximum number of element in the 'ring memory
    const unsigned m_elements;
};


}  // end namespaces
}
#endif  // end header latch
