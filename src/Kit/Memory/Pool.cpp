/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Pool.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Assert.h"
#include <cstdint>
#include <string.h>

#define SECT_ "Kit::Memory"

//------------------------------------------------------------------------------
namespace Kit {
namespace Memory {


/////////////////////////////
Pool::Pool( BlockInfo infoBlocks[],
            size_t    blockSize,
            size_t    alignedBlockSize,
            size_t    numBlocks,
            void*     arrayOfMemoryBlocks,
            bool      errorsAreFatal )
    : m_blockSize( blockSize )
    , m_alignedBlockSize( alignedBlockSize )
    , m_fatalErrors( errorsAreFatal )
{
    KIT_SYSTEM_ASSERT( infoBlocks != nullptr );
    KIT_SYSTEM_ASSERT( arrayOfMemoryBlocks != nullptr );

    // Generate my free list
    size_t   i;
    uint8_t* blockPtr = static_cast<uint8_t*>( arrayOfMemoryBlocks );
    for ( i = 0; i < numBlocks; i++ )
    {
        infoBlocks[i].m_blockPtr  = blockPtr;
        blockPtr                 += alignedBlockSize;
        m_freeList.put( infoBlocks[i] );
    }
}


/////////////////////////////
size_t Pool::wordSize() const noexcept
{
    return m_alignedBlockSize;
}

void* Pool::allocate( size_t numbytes )
{
    void* ptr = nullptr;

    // Trap requesting more memory than the block size
    if ( numbytes > m_blockSize )
    {
        if ( m_fatalErrors )
        {
            Kit::System::FatalError::logf( Kit::System::Shutdown::eMEMORY,
                                           "Kit::Memory::Pool::allocate().  Failed allocation: Requested size (%zu) > block size (%zu). Allocator=%p",
                                           numbytes,
                                           m_blockSize,
                                           this );
        }
    }

    // Try to allocate memory
    else
    {
        // Get a block from my free list
        BlockInfo* infoPtr = m_freeList.get();
        if ( infoPtr )
        {
            ptr = infoPtr->m_blockPtr;
            m_allocatedList.put( *infoPtr );
        }

        // Handle the error case
        else
        {
            if ( m_fatalErrors )
            {
                Kit::System::FatalError::logf( Kit::System::Shutdown::eMEMORY,
                                               "Kit::Memory::Pool::allocate().  Out of memory/blocks (requested size=%zu). Allocator=%p",
                                               numbytes,
                                               this );
            }
        }
    }

    // Return the allocated block
    return ptr;
}


void Pool::release( void* ptr )
{
    // Handle the case of ptr == 0  \(per semantic of the Allocator interface)
    if ( ptr == nullptr )
    {
        return;
    }

    // Check against my allocated list
    BlockInfo* infoPtr = m_allocatedList.first();
    while ( infoPtr )
    {
        // Found in the allocated list -->move it the free list
        if ( infoPtr->m_blockPtr == ptr )
        {
            m_allocatedList.remove( *infoPtr );
            m_freeList.put( *infoPtr );
            return;
        }

        infoPtr = m_allocatedList.next( *infoPtr );
    }

    // If I get here than a pointer that I did NOT allocated is trying to released (this is bad!)
    Kit::System::FatalError::logf( System::Shutdown::eMEMORY,
                                   "Kit::Memory::Pool::release().  Freeing a pointer (%p) that was not previously allocated. Allocator=%p",
                                   ptr,
                                   this );
}

}  // end namespace
}
//------------------------------------------------------------------------------