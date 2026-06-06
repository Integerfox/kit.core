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
    RingBufferMT( ITEM* memoryBuffer, unsigned N, bool initializeMemory = false ) noexcept
        : RingBuffer<ITEM>( memoryBuffer, N, initializeMemory )
    {
    }

public:
    /// See Kit::RingBuffer
    bool isEmpty() const noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::isEmpty();
    }

    /// See Kit::RingBuffer
    bool isFull() const noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::isFull();
    }

    /// See Kit::RingBuffer
    unsigned getNumItems() const noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::getNumItems();
    }

    /// See Kit::RingBuffer
    void clearTheBuffer() noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        RingBuffer<ITEM>::clearTheBuffer();
    }

public:
    /// See Kit::RingBuffer
    bool add( const ITEM& item ) noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::add( item );
    }

    /// See Kit::RingBuffer
    bool remove( ITEM& item ) noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::remove( item );
    }

    /// See Kit::RingBuffer
    bool peekHead( ITEM& item ) noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::peekHead( item );
    }

    /// See Kit::RingBuffer
    bool peekTail( ITEM& item ) noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::peekTail( item );
    }

public:
    /// See Kit::RingBuffer
    ITEM* peekNextRemoveItems( unsigned& dstNumFlatElements ) const noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::peekNextRemoveItems( dstNumFlatElements );
    }

    /// See Kit::RingBuffer
    void removeElements( unsigned numElementsToRemove ) noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        RingBuffer<ITEM>::removeElements( numElementsToRemove );
    }

public:
    /// See Kit::RingBuffer
    ITEM* peekNextAddItems( unsigned& dstNumFlatElements ) const noexcept override
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        return RingBuffer<ITEM>::peekNextAddItems( dstNumFlatElements );
    }

    /// See Kit::RingBuffer
    void addElements( unsigned numElementsAdded ) noexcept override
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
