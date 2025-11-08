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
#include "Kit/System/Assert.h"

///
namespace Kit {
///
namespace Container {


/** This template class implements a Ring Buffer.

    A Ring Buffer instance IS ISR/Thread safe WHEN using a SINGLE Producer and
    SINGLE consumer of the buffer.  See the RingBufferBase class for additional
    details.

    There is a set of methods that allow the application direct memory access -
    AS A LINEAR BUFFER - to a subset of the ring buffer's data. At any given
    time the size of the linear buffer is restricted to the amount of data that
    can be accessed without 'wrapping' the ring buffer memory space.  The
    intended use case for these methods is for bulk copy operations (e.g populating
    a FIFO or a DMA buffer).
        peekNextRemoveItems()
        removeElements()
        peekNextAddItems()
        addElements()

    Template Args:
        ITEM:=      Type of the data stored in the Ring Buffer
 */
template <class ITEM>
class RingBuffer : public RingBufferBase
{
public:
    /// Constructor
    RingBuffer( ITEM* memoryBuffer, unsigned N )
        : RingBufferBase( N )
        , m_bufferMemory( memoryBuffer )
    {
        KIT_SYSTEM_ASSERT( memoryBuffer != nullptr );
        KIT_SYSTEM_ASSERT( N > 1 );

        // Start with a clean buffer (helps with debugging)
        memset( memoryBuffer, 0, sizeof( ITEM ) * N );
    }

public:
    /** Adds new item to the ring buffer. Returns true on success. If the ring
        buffer is full, then false is returned (and ring buffer is not updated).
     */
    bool add( const ITEM& item ) noexcept
    {
        return RingBufferBase::add( &item, sizeof( ITEM ), m_bufferMemory );
    }

    /** Removes an item from the ring buffer.  Returns true on success.  If the
        ring buffer is empty, then false is returned (and the 'item' argument
        is not updated) */
    bool remove( ITEM& item ) noexcept
    {
        return RingBufferBase::remove( &item, sizeof( ITEM ), m_bufferMemory );
    }

    /** This method inspects the head of the ring buffer without removing it.
        The contents of the head element will be copied into the 'item' argument.
        The method returns true if the operation was successful; else false is
        returned, i.e. the Ring buffer is empty.
     */
    bool peekHead( ITEM& item ) noexcept
    {
        return RingBufferBase::peekHead( &item, sizeof( ITEM ), m_bufferMemory );
    }

    /** This method inspects the tail of the ring buffer without removing it.
        The contents of the tail element will be copied into the 'item' argument.
        The method returns true if the operation was successful; else false is
        returned, i.e. the Ring buffer is empty.
     */
    bool peekTail( ITEM& item ) noexcept
    {
        return RingBufferBase::peekTail( &item, sizeof( ITEM ), m_bufferMemory );
    }

public:
    /** This method returns a pointer to the next item to be removed. In addition
        it returns the number of elements that can be removed as linear/flat
        buffer (i.e. without wrapping around raw buffer memory)

        If the Ring buffer is empty, a null pointer is returned
     */
    ITEM* peekNextRemoveItems( unsigned& dstNumFlatElements ) const noexcept
    {
        return static_cast<ITEM*>( RingBufferBase::peekNextRemoveItems( dstNumFlatElements, sizeof( ITEM ), m_bufferMemory ) );
    }

    /** This method 'removes' N elements - that were removed using the
        pointer returned from peekNextRemoveItems - from the ring buffer.
        Basically it updates the read index to reflect items removed using
        direct memory access.

        'numElementsToRemove' be less than or equal to the 'dstNumFlatElements'
        returned from peekNextRemoveItems().

        CAUTION: IF YOU DON'T UNDERSTAND THE USE CASE FOR THIS METHOD - THEN
                 DON'T USE IT.  If this method is used improperly, it WILL
                 CORRUPT the Ring Buffer!
     */
    void removeElements( unsigned numElementsToRemove ) noexcept
    {
        RingBufferBase::removeElements( numElementsToRemove );
    }

public:
    /** This method returns a pointer to the next item to be added. In addition
        it returns the number of elements that can be added as linear/flat
        buffer (i.e. without wrapping around raw buffer memory)

        If the Ring buffer is full, a null pointer is returned
     */
    ITEM* peekNextAddItems( unsigned& dstNumFlatElements ) const noexcept
    {
        return static_cast<ITEM*>( RingBufferBase::peekNextAddItems( dstNumFlatElements, sizeof( ITEM ), m_bufferMemory ) );
    }

    /** This method 'adds' N elements - that were populated using the
        pointer returned from peekNextAddItems - to the ring buffer.  Basically
        its updates the write index to reflect items added using direct
        memory access.

        'numElementsAdded' be less than or equal to the 'dstNumFlatElements'
        returned from peekNextAddItems().

        CAUTION: IF YOU DON'T UNDERSTAND THE USE CASE FOR THIS METHOD - THEN
                 DON'T USE IT. If this method is used improperly, it WILL
                 CORRUPT the Ring Buffer!
     */
    void addElements( unsigned numElementsAdded ) noexcept
    {
        RingBufferBase::addElements( numElementsAdded );
    }

protected:
    /// Memory for the Ring buffer
    ITEM* m_bufferMemory;

private:
    /// Prevent access to the copy constructor -->Ring Buffers can not be copied!
    RingBuffer( const RingBuffer<ITEM>& m ) = delete;

    /// Prevent access to the assignment operator -->Ring Buffers can not be copied!
    RingBuffer<ITEM>& operator=( const RingBuffer<ITEM>& m ) = delete;

    /// Prevent access to the move constructor -->Ring Buffers can not be implicitly moved!
    RingBuffer( RingBuffer<ITEM>&& m ) = delete;

    /// Prevent access to the move assignment operator -->Ring Buffers can not be implicitly moved!
    RingBuffer<ITEM>& operator=( RingBuffer<ITEM>&& m ) = delete;
};

}  // end namespaces
}
#endif  // end header latch
