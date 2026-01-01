#ifndef KIT_MEMORY_POOL_H_
#define KIT_MEMORY_POOL_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/Memory/Allocator.h"
#include "Kit/Container/DList.h"
#include <stdint.h>

/** Default option for the 'errorsAreFatal' parameter to the Pool constructor */
#ifndef OPTION_KIT_MEMORY_POOL_ERRORS_ARE_FATAL
#define OPTION_KIT_MEMORY_POOL_ERRORS_ARE_FATAL    true // Default is to throw a fatal error
#endif


namespace Kit {
///
namespace Memory {

/** This internal (to the namespace) concrete class implements a Memory Allocator
    using a pool of fixed size blocks.  The implementation relies on a sub-class
    to allocate the actual memory for the blocks.
 */

class Pool : public Allocator
{
public:
    /// Helper class so I can put my blocks into to my standard containers
    class BlockInfo : public Kit::Container::ExtendedListItem
    {
    public:
        /// Constructor
        BlockInfo()
            : m_blockPtr( nullptr ) {}

    public:
        /// Reference to the block being stored in the list
        uint8_t* m_blockPtr;
    };


public:
    /** Constructor.  Note: when 'errorsAreFatal' is true, memory errors cause
        a fatal error (e.g. out of memory); else returns nullptr on errors.
     */
    Pool( BlockInfo infoBlocks[],
          size_t    blockSize,
          size_t    alignedBlockSize,
          size_t    numBlocks,
          void*     arrayOfMemoryBlocks,
          bool      errorsAreFatal = OPTION_KIT_MEMORY_POOL_ERRORS_ARE_FATAL );


    /// Destructor
    ~Pool() = default;

public:
    /// See Kit::Memory::Allocator
    void* allocate( size_t numbytes ) noexcept override;

    /// See Kit::Memory::Allocator
    void release( void* ptr ) noexcept override;

    /// See Kit::Memory::Allocator
    size_t wordSize() const noexcept override;

    protected:
    /// My free list of blocks
    Kit::Container::DList<BlockInfo> m_freeList;

    /// My list of allocated blocks
    Kit::Container::DList<BlockInfo> m_allocatedList;

    /// Block size
    size_t m_blockSize;

    /// Block size
    size_t m_alignedBlockSize;

    /// Flag that controls memory errors behavior
    bool m_fatalErrors;
};


}  // end namespaces
}
#endif  // end header latch
