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
#include "Kit/Dm/Mp/Uint32.h"

///
namespace Kit {
///
namespace Container {


/** This template class extends the RingBuffer class to be thread safe AND
    has Uint32 Model Point that tracks the number of elements in the buffer. This
    give the RingBuffer change notification semantics with elements are added
    or removed.

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
 */
template <class ITEM>
class RingBufferMP : public RingBuffer<ITEM>
{
public:
    /// Constructor
    RingBufferMP( Kit::Dm::Mp::Uint32& mpElementCount, ITEM* memoryBuffer, unsigned N, bool initializeMemory = true )
        : RingBuffer<ITEM>( memoryBuffer, N, initializeMemory )
        , m_mpElementCount( mpElementCount )
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
        m_mpElementCount.write( 0 );
        RingBuffer<ITEM>::clearTheBuffer();
    }

public:
    /// See Kit::RingBuffer::isEmpty
    bool add( const ITEM& item ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        //
        bool result = RingBuffer<ITEM>::add( item );
        if ( result )
        {
            m_mpElementCount.increment();
        }

        return result;
    }

    /// Same as add() but also returns the sequence number after the increment operation
    bool add( const ITEM& item, uint16_t& seqNum ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        //
        bool result = RingBuffer<ITEM>::add( item );
        if ( result )
        {
            seqNum = m_mpElementCount.increment();
        }

        return result;
    }

    /// See Kit::RingBuffer::isEmpty
    bool remove( ITEM& item ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        //
        bool result = RingBuffer<ITEM>::remove( item );
        if ( result )
        {
            m_mpElementCount.decrement();
        }
        return result;
    }

    /// Same as remove() but also returns the sequence number after the decrement operation
    bool remove( ITEM& item, uint16_t& seqNum ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        //
        bool result = RingBuffer<ITEM>::remove( item );
        if ( result )
        {
            seqNum = m_mpElementCount.decrement();
        }
        return result;
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
        m_mpElementCount.decrement( numElementsToRemove );
        RingBuffer<ITEM>::removeElements( numElementsToRemove );
    }

    /// Same as removeElements() but also returns the sequence number after the decrement operation
    void removeElements( unsigned numElementsToRemove, uint16_t& seqNum ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        seqNum = m_mpElementCount.decrement( numElementsToRemove );
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
        m_mpElementCount.increment( numElementsAdded );
        RingBuffer<ITEM>::addElements( numElementsAdded );
    }

    /// Same as addElements() but also returns the sequence number after the increment operation
    void addElements( unsigned numElementsAdded, uint16_t& seqNum ) noexcept
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_mutex );
        seqNum = m_mpElementCount.increment( numElementsAdded );
        RingBuffer<ITEM>::addElements( numElementsAdded );
    }
    
public:
    /// Model Point that tracks the number of elements in the buffer. Make public to allow attach/detach of observers.
    Kit::Dm::Mp::Uint32& m_mpElementCount;

protected:
    /// Mutex to protect the ring buffer's integrity
    mutable Kit::System::Mutex m_mutex;

private:
    /// Prevent access to the copy constructor -->Ring Buffers can not be copied!
    RingBufferMP( const RingBufferMP& m ) = delete;

    /// Prevent access to the assignment operator -->Ring Buffers can not be copied!
    RingBufferMP& operator=( const RingBufferMP& m ) = delete;

    /// Prevent access to the move constructor -->Ring Buffers can not be implicitly moved!
    RingBufferMP( RingBufferMP&& m ) = delete;

    /// Prevent access to the move assignment operator -->Ring Buffers can not be implicitly moved!
    RingBufferMP& operator=( RingBufferMP&& m ) = delete;
};

}  // end namespaces
}
#endif  // end header latch
