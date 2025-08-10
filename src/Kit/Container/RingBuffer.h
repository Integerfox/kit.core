#ifndef KIT_CONTAINER_RINGBUFFER_H_
#define KIT_CONTAINER_RINGBUFFER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/RingBufferBase.h"


///
namespace Kit {
///
namespace Container {


/** This template class implements a Ring Buffer.

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
        N:=         Size of the array that is allocated to hold the Ring Buffer
                    data.  The actual maximum number of elements that can be
                    stored in the Ring Buffer is N-1.
 */
template <class ITEM, int N>
class RingBuffer : public RingBufferBase
{
public:
    /// Constructor
    RingBuffer()
        : RingBufferBase( N ) {}

public:
    /** Adds new item to the ring buffer. Returns true on success. If the ring
        buffer is full, then false is returned (and ring buffer is not updated).
     */
    bool add( const ITEM& item ) noexcept
    {
        return RingBufferBase::add( &item, sizeof( ITEM ), m_ringBufferMemory );
    }

    /** Removes an item from the ring buffer.  Returns true on success.  If the
        ring buffer is empty, then false is returned (and the 'item' argument
        is not updated) */
    bool remove( ITEM& item ) noexcept
    {
        return RingBufferBase::remove( &item, sizeof( ITEM ), m_ringBufferMemory );
    }

    /** This method inspects the head of the ring buffer without removing it.
        The contents of the head element will be copied into the 'item' argument.
        The method returns true if the operation was successful; else false is
        returned, i.e. the Ring buffer is empty.
     */
    bool peekHead( ITEM& item ) noexcept
    {
        return RingBufferBase::peekHead( &item, sizeof( ITEM ), m_ringBufferMemory );
    }

    /** This method inspects the tail of the ring buffer without removing it.
        The contents of the tail element will be copied into the 'item' argument.
        The method returns true if the operation was successful; else false is
        returned, i.e. the Ring buffer is empty.
     */
    bool peekTail( ITEM& item ) noexcept
    {
        return RingBufferBase::peekTail( &item, sizeof( ITEM ), m_ringBufferMemory );
    }

protected:
    /// Memory for the Ring buffer
    ITEM m_ringBufferMemory[N];

protected:
    /// Prevent access to the copy constructor -->Ring Buffer can not be copied!
    RingBuffer( const RingBuffer<ITEM, N>& m );

    /// Prevent access to the assignment operator -->Ring Buffer can not be copied!
    const RingBuffer<ITEM, N>& operator=( const RingBuffer<ITEM, N>& m );
};

#if 0 
/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////


template <class ITEM>
RingBuffer<ITEM>::RingBuffer( unsigned maxElements, ITEM memoryForElements[] ) noexcept
    : m_headPtr( memoryForElements )
    , m_tailPtr( memoryForElements )
    , m_memoryNumElements( maxElements )
    , m_endOfMemPtr( memoryForElements + maxElements )
    , m_elements( memoryForElements )
{
}


template <class ITEM>
inline void RingBuffer<ITEM>::clearTheBuffer() noexcept
{
    m_headPtr = m_tailPtr = m_elements;
}


template <class ITEM>
inline bool RingBuffer<ITEM>::add( const ITEM& item ) noexcept
{
    if ( isFull() )
    {
        return false;
    }

    *m_tailPtr = item;
    if ( ++m_tailPtr >= m_endOfMemPtr )
    {
        m_tailPtr = m_elements;
    }

    return true;
}

template <class ITEM>
inline bool RingBuffer<ITEM>::remove( ITEM& dst ) noexcept
{
    if ( isEmpty() )
    {
        return false;
    }

    dst = *m_headPtr;
    if ( ++m_headPtr >= m_endOfMemPtr )
    {
        m_headPtr = m_elements;
    }
    return true;
}


template <class ITEM>
inline ITEM* RingBuffer<ITEM>::peekNextRemoveItems( unsigned& dstNumFlatElements ) noexcept
{
    if ( isEmpty() )
    {
        return nullptr;
    }

    unsigned totalNumElements = getNumItems();
    dstNumFlatElements        = m_endOfMemPtr - m_headPtr;
    if ( dstNumFlatElements > totalNumElements )
    {
        dstNumFlatElements = totalNumElements;
    }

    return m_headPtr;
}

template <class ITEM>
inline void RingBuffer<ITEM>::removeElements( unsigned numElementsToRemove ) noexcept
{
    // By the defined semantics - I simply update the head pointer
    m_headPtr += numElementsToRemove;
    if ( m_headPtr >= m_endOfMemPtr )
    {
        m_headPtr = m_elements;
    }
}

template <class ITEM>
inline ITEM* RingBuffer<ITEM>::peekNextAddItems( unsigned& dstNumFlatElements ) noexcept
{
    if ( isFull() )
    {
        dstNumFlatElements = 0;
        return nullptr;
    }

    unsigned totalAvailElements = getMaxItems() - getNumItems();
    dstNumFlatElements          = m_endOfMemPtr - m_tailPtr;
    if ( dstNumFlatElements > totalAvailElements )
    {
        dstNumFlatElements = totalAvailElements;
    }

    return m_tailPtr;
}

template <class ITEM>
inline void RingBuffer<ITEM>::addElements( unsigned numElementsAdded ) noexcept
{
    // By the defined semantics - I simply update the tail pointer
    m_tailPtr += numElementsAdded;
    if ( m_tailPtr >= m_endOfMemPtr )
    {
        m_tailPtr = m_elements;
    }
}

template <class ITEM>
inline ITEM* RingBuffer<ITEM>::peekHead( void ) const noexcept
{
    if ( isEmpty() )
    {
        return 0;
    }

    return m_headPtr;
}

template <class ITEM>
inline ITEM* RingBuffer<ITEM>::peekTail( void ) const noexcept
{
    if ( isEmpty() )
    {
        return 0;
    }

    ITEM* prevElem = m_tailPtr - 1;
    if ( prevElem < m_elements )
    {
        prevElem = m_endOfMemPtr - 1;
    }

    return prevElem;
}


template <class ITEM>
inline bool RingBuffer<ITEM>::isEmpty( void ) const noexcept
{
    return m_headPtr == m_tailPtr;
}

template <class ITEM>
inline bool RingBuffer<ITEM>::isFull( void ) const noexcept
{
    ITEM* nextElem = m_tailPtr + 1;
    if ( nextElem >= m_endOfMemPtr )
    {
        nextElem = m_elements;
    }
    return nextElem == m_headPtr;
}

template <class ITEM>
inline unsigned RingBuffer<ITEM>::getNumItems( void ) const noexcept
{
    unsigned headIdx = (unsigned)( m_headPtr - m_elements );
    unsigned tailIdx = (unsigned)( m_tailPtr - m_elements );
    if ( tailIdx < headIdx )
    {
        tailIdx += m_memoryNumElements;
    }
    return tailIdx - headIdx;
}

template <class ITEM>
inline unsigned RingBuffer<ITEM>::getMaxItems( void ) const noexcept
{
    return m_memoryNumElements - 1;  // One elem/slot is reserved for the empty-list condition
}
#endif

}       // end namespaces
}
#endif  // end header latch
