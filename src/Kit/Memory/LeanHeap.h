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
#include <cstddef>
#include <cstring>


///
namespace Kit {
///
namespace Memory {


/** This template class manages an allocate-only-heap with statically allocated
    memory. Memory allocated by this class cannot be freed. All of the memory
    is aligned to the alignment requirement of the specified word type.

    Note: The heap has a 'reset' method that can be used to free all of the
          memory allocated by the instance - HOWEVER it is the responsibility
          of the Application to ensure the proper behavior when this
          method is used (i.e. that is okay to 'free everything')

    NOTE: In C++ it is possible that sizeof(T) does NOT equal alignof(T). For
          example on my Ubuntu x86_64 system, sizeof(std::max_align_t) is 32 bytes
          but alignof(std::max_align_t) is 16 bytes.  Why do I care? Because 
          memory is allocated in 'words' of sizeof(T) bytes, not alignof(T) bytes.

    Template args:
        MAXWORDS - The maximum number of words in the heap. Must be > 0.
        WORDTYPE - The type that defines the word size and alignment.
                   Defaults to std::max_align_t.
 */

template <size_t MAXWORDS, typename WORDTYPE = std::max_align_t>
class LeanHeap : public ContiguousAllocator
{
    static_assert( MAXWORDS > 0, "MAXWORDS must be greater than 0" );

    /// Total size of the heap in bytes
    static constexpr size_t HEAP_SIZE_IN_BYTES = MAXWORDS * sizeof( WORDTYPE );

public:
    /** Constructor for statically-allocated heap memory.
     */
    LeanHeap() noexcept
        : m_ptrBase( m_heapStorage )
        , m_ptr( m_heapStorage )
        , m_unitsRemaining( MAXWORDS )
    {
        // Zero out all of memory
        std::memset( m_heapStorage, 0, sizeof(m_heapStorage) );
    }

public:
    /// See Kit::Memory::Allocator
    void* allocate( size_t numbytes ) noexcept override
    {
        // Calculate the actual size needed (rounded up to word boundary)
        size_t alignedBytes = allocatedSizeForNBytes( numbytes );
        size_t numUnits     = alignedBytes / sizeof( WORDTYPE );

        if ( numUnits > m_unitsRemaining )
        {
            return nullptr;
        }

        uint8_t* result   = m_ptr;
        m_unitsRemaining -= numUnits;
        m_ptr            += alignedBytes;
        return result;
    }

    /// See Kit::Memory::Allocator
    size_t wordSize() const noexcept override { return sizeof( WORDTYPE ); }

    /// See Kit::Memory::ContiguousAllocator
    void reset() noexcept override
    {
        m_unitsRemaining = MAXWORDS;
        m_ptr            = m_ptrBase;
        std::memset( m_ptr, 0, MAXWORDS * sizeof( WORDTYPE ) );
    }

    /// See Kit::Memory::ContiguousAllocator.
    MemoryInfo_T& getMemoryStart( MemoryInfo_T& dstInfo ) noexcept override
    {
        dstInfo.m_memoryStartPtr  = m_ptrBase;
        dstInfo.m_totalLenInBytes = HEAP_SIZE_IN_BYTES;
        dstInfo.m_maxWords        = MAXWORDS;
        dstInfo.m_freeWords       = m_unitsRemaining;
        return dstInfo;
    }

private:
    /// Prevent access to the copy constructor -->LeanHeap can not be copied!
    LeanHeap( const LeanHeap& m ) = delete;

    /// Prevent access to the assignment operator -->LeanHeap can not be copied!
    const LeanHeap& operator=( const LeanHeap& m ) = delete;

protected:
    /// Pointer to the base of the original heap memory
    uint8_t* m_ptrBase;

    /// Pointer to the next available free byte of memory
    uint8_t* m_ptr;

    /// The amount of remaining free memory in alignment units
    size_t m_unitsRemaining;

    /// Statically allocated heap storage
    alignas( WORDTYPE ) uint8_t m_heapStorage[HEAP_SIZE_IN_BYTES];
};

}  // end namespaces
}
#endif  // end header latch
