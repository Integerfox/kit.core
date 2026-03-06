#ifndef KIT_CONTAINER_RINGBUFFER_MT_H_
#define KIT_CONTAINER_RINGBUFFER_MT_H_
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
#include "Kit/System/Mutex.h"

///
namespace Kit {
///
namespace Container {


/** This template class extends the RingBuffer class to be thread safe

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
 */
template <class ITEM>
class RingBufferMT : public RingBuffer<ITEM>
{
public:
    /// Constructor
    RingBufferMT( ITEM* memoryBuffer, unsigned N, bool initializeMemory = true )
        : RingBuffer<ITEM>( memoryBuffer, N, initializeMemory )
    {
    }

public:
    /// See Kit::RingBuffer::isEmpty
    bool isEmpty() const noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::isEmpty();
    }

    /// See Kit::RingBuffer::isEmpty
    bool isFull() const noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::isFull();
    }

    /// See Kit::RingBuffer::isEmpty
    unsigned getMaxItems() const noexcept
    {
        return RingBuffer<ITEM>::getMaxItems();
    }

    /// See Kit::RingBuffer::isEmpty
    unsigned getNumItems() const noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::getNumItems();
    }

public:
    /// See Kit::RingBuffer::isEmpty
    void clearTheBuffer() noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        RingBuffer<ITEM>::clearTheBuffer();
    }
public:
    /// See Kit::RingBuffer::isEmpty
    bool add( const ITEM& item ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::add( item );
    }

    /// See Kit::RingBuffer::isEmpty
    bool remove( ITEM& item ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::remove( item );
    }

    /// See Kit::RingBuffer::isEmpty
    bool peekHead( ITEM& item ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::peekHead( item );
    }

    /// See Kit::RingBuffer::isEmpty
    bool peekTail( ITEM& item ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::peekTail( item );
    }

public:
    /// See Kit::RingBuffer::isEmpty
    ITEM* peekNextRemoveItems( unsigned& dstNumFlatElements ) const noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::peekNextRemoveItems( dstNumFlatElements );
    }

    /// See Kit::RingBuffer::isEmpty
    void removeElements( unsigned numElementsToRemove ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        RingBuffer<ITEM>::removeElements( numElementsToRemove );
    }

public:
    /// See Kit::RingBuffer::isEmpty
    ITEM* peekNextAddItems( unsigned& dstNumFlatElements ) const noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::peekNextAddItems( dstNumFlatElements );
    }

    /// See Kit::RingBuffer::isEmpty
    void addElements( unsigned numElementsAdded ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        RingBuffer<ITEM>::addElements( numElementsAdded );
    }

protected:
    /// Mutex to protect the ring buffer's integrity
    mutable Kit::System::Mutex m_mutex;

private:
    /// Prevent access to the copy constructor -->Ring Buffers can not be copied!
    RingBufferMT( const RingBufferMT& m ) = delete;

    /// Prevent access to the assignment operator -->Ring Buffers can not be copied!
    RingBufferMT& operator=( const RingBufferMT& m ) = delete;

    /// Prevent access to the move constructor -->Ring Buffers can not be implicitly moved!
    RingBufferMT( RingBufferMT&& m ) = delete;

    /// Prevent access to the move assignment operator -->Ring Buffers can not be implicitly moved!
    RingBufferMT& operator=( RingBufferMT&& m ) = delete;
};

}  // end namespaces
}
#endif  // end header latch
