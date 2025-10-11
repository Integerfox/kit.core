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

#include <stdint.h>
#include <stdlib.h>

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
    RingBufferBase( unsigned numMaxElements ) noexcept;

public:
    /// This method returns true if the Ring Buffer is empty
    bool isEmpty() const noexcept
    {
        return m_readIdx == m_writeIdx;
    }

    /// This method returns true if the Ring Buffer is full
    bool isFull() const noexcept
    {
        return ( ( m_writeIdx + 1 ) % m_elements ) == m_readIdx;
    }

    /// This method returns the maximum number of items that can be stored in the Ring buffer.
    unsigned getMaxItems() const noexcept
    {
        return m_elements - 1;
    }

    /// This method returns the CURRENT number of elements in the Ring Buffer
    unsigned getNumItems() const noexcept
    {
        return ( m_writeIdx - m_readIdx + m_elements ) % m_elements;
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
    bool remove( void* elemPtr, size_t elemSize, const void* srcRingBufMemory ) noexcept;

    /** Add a new item to the Ring Buffer as the last item in the buffer. The
        contents of the source item (aka 'elemPtr') will be copied into the
        Ring Buffer. Returns true if the operation was successful; else false
        is returned, i.e. the Buffer was full prior to the attempted add().
     */
    bool add( const void* elemPtr, size_t elemSize, void* dstRingBufMemory ) noexcept;

protected:
    /** This method inspects the head element of the Ring buffer without removing it.
        The contents of the head element will be copied into the 'elemPtr' argument.
        The method returns true if the operation was successful; else false is
        returned, i.e. the Ring buffer is empty.
     */
    bool peekHead( void* elemPtr, size_t elemSize, const void* srcRingBufMemory ) noexcept;

    /** This method inspects the tail element of the Ring buffer without removing it.
        The contents of the tail element will be copied into the 'elemPtr' argument.
        The method returns true if the operation was successful; else false is
        returned, i.e. the Ring buffer is empty.
     */
    bool peekTail( void* elemPtr, size_t elemSize, const void* srcRingBufMemory ) noexcept;

protected:
    /** This method returns a pointer to the next item to be removed. In addition
        it returns the number of elements that can be removed as linear/flat
        buffer (i.e. without wrapping around raw buffer memory).

        The returned pointer and element count are only valid till the next
        read/remove operation.

        If the Ring buffer is empty, a null pointer is returned
     */
    void* peekNextRemoveItems( unsigned& dstNumFlatElements, size_t elemSize, const void* srcRingBufMemory ) const noexcept;

    /** This method 'removes' N elements - that were removed using the
        pointer returned from peekNextRemoveItems - from the ring buffer.
        Basically it updates the head pointer to reflect items removed using
        direct memory access.

        'numElementsToRemove' be less than or equal to the 'dstNumFlatElements'
        returned from peekNextRemoveItems().

        CAUTION: IF YOU DON'T UNDERSTAND THE USE CASE FOR THIS METHOD - THEN
                 DON'T USE IT.  If this method is used improperly, it WILL
                 CORRUPT the Ring Buffer!
     */
    void removeElements( unsigned numElementsToRemove ) noexcept
    {
        // By definition/design - I simply have to update the read index
        m_readIdx = ( m_readIdx + numElementsToRemove ) % m_elements;
    }

protected:
    /** This method returns a pointer to the next item to be added. In addition
        it returns the number of elements that can be added as linear/flat
        buffer (i.e. without wrapping around raw buffer memory)

        If the Ring buffer is full, a null pointer is returned
     */
    void* peekNextAddItems( unsigned& dstNumFlatElements, size_t elemSize, const void* srcRingBufMemory ) const noexcept;

    /** This method 'adds' N elements - that were populated using the
        pointer returned from peekNextAddItems - to the ring buffer.  Basically
        its updates the tail pointer to reflect items added using direct
        memory access.

        'numElementsAdded' be less than or equal to the 'dstNumFlatElements'
        returned from peekNextAddItems().

        CAUTION: IF YOU DON'T UNDERSTAND THE USE CASE FOR THIS METHOD - THEN
                 DON'T USE IT. If this method is used improperly, it WILL
                 CORRUPT the Ring Buffer!
     */
    void addElements( unsigned numElementsAdded ) noexcept
    {
        // By definition/design - I simply have to update the write index
        m_writeIdx = ( m_writeIdx + numElementsAdded ) % m_elements;
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

private:
    /// Prevent access to the copy constructor -->Ring Buffers can not be copied!
    RingBufferBase( const RingBufferBase& m ) = delete;

    /// Prevent access to the assignment operator -->Ring Buffers can not be copied!
    RingBufferBase& operator=( const RingBufferBase& m ) = delete;

    /// Prevent access to the move constructor -->Ring Buffers can not be implicitly moved!
    RingBufferBase( RingBufferBase&& m ) = delete;

    /// Prevent access to the move assignment operator -->Ring Buffers can not be implicitly moved!
    RingBufferBase& operator=( RingBufferBase&& m ) = delete;
};


}  // end namespaces
}
#endif  // end header latch
