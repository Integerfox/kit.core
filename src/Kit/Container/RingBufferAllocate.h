#ifndef KIT_CONTAINER_RINGBUFFER_ALLOCATE_H_
#define KIT_CONTAINER_RINGBUFFER_ALLOCATE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/RingBuffer.h"
#include <string.h>


///
namespace Kit {
///
namespace Container {


/** This template class extends the RingBuffer class by statically allocating 
    memory for the ring buffer.

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
        N:=         Size of the array that is allocated to hold the Ring Buffer
                    data.  The actual maximum number of elements that can be
                    stored in the Ring Buffer is N-1.
 */
template <class ITEM, int N>
class RingBufferAllocate : public RingBuffer<ITEM>
{
public:
    /// Constructor
    RingBufferAllocate()
        : RingBuffer<ITEM>( m_rawMemory, N, false )
    {
        // Initialize the ring buffer memory - it helps with debugging
        memset( static_cast<void*>(m_rawMemory), 0, sizeof( ITEM ) * N );
    }

protected:
    /// Memory for the Ring buffer
    ITEM m_rawMemory[N];

private:
    /// Prevent access to the copy constructor -->Ring Buffers can not be copied!
    RingBufferAllocate( const RingBufferAllocate& m ) = delete;

    /// Prevent access to the assignment operator -->Ring Buffers can not be copied!
    RingBufferAllocate& operator=( const RingBufferAllocate& m ) = delete;

    /// Prevent access to the move constructor -->Ring Buffers can not be implicitly moved!
    RingBufferAllocate( RingBufferAllocate&& m ) = delete;

    /// Prevent access to the move assignment operator -->Ring Buffers can not be implicitly moved!
    RingBufferAllocate& operator=( RingBufferAllocate&& m ) = delete;
};

}  // end namespaces
}
#endif  // end header latch
