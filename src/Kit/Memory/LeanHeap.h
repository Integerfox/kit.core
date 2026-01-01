#ifndef KIT_MEMORY_LEAN_HEAP_H_
#define KIT_MEMORY_LEAN_HEAP_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Memory/ContiguousAllocator.h"


///
namespace Kit {
///
namespace Memory {


/** This class manages an allocate-only-heap.  Memory allocated by this class
    cannot be freed. All of the memory is aligned to size_t boundaries.

    Note: The heap has a 'reset' method that can be used to free all of the 
          memory allocated by the instance - HOWEVER it is the responsibility
          of the Application to ensure the proper behavior when this
          method is used (i.e. that is okay to 'free everything')
 */

class LeanHeap: public ContiguousAllocator
{

public:
    /** Constructor.  The memory size of 'heapMemory' MUST be a multiple
        of sizeof(size_t).
     */
    LeanHeap( size_t* heapMemory, size_t sizeInBytes );


public:
    /// See Kit::Memory::ContiguousAllocator
    void* allocate( size_t numbytes ) noexcept override;

    /// See Kit::Memory::ContiguousAllocator
    void reset() noexcept override;

    /// See Kit::Memory::ContiguousAllocator. 
    void* getMemoryStart( size_t& dstAllocatedLenInBytes ) noexcept override;

    /// See Kit::Memory::Allocator
    size_t wordSize() const noexcept override { return sizeof( size_t ); }

private:
    /// Prevent access to the copy constructor -->LeanHeap can not be copied!
    LeanHeap( const LeanHeap& m ) = delete;

    /// Prevent access to the assignment operator -->LeanHeap can not be copied!
    const LeanHeap& operator=( const LeanHeap& m ) = delete;

protected:
    /// Pointer to the base of the original heap memory
    size_t*  m_ptrBase;

    /// Size (in words) of the original heap memory
    size_t   m_staringHeapSize;

    /// Pointer to the next available free byte of memory
    size_t*  m_ptr;

    /// The amount of remaining free memory in size_t words
    size_t   m_wordsRemaining;
};


}      // end namespaces
}
#endif  // end header latch
