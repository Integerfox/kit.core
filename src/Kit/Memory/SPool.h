#ifndef KIT_MEMORY_SPOOL_H_
#define KIT_MEMORY_SPOOL_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Memory/Pool.h"
#include "Kit/Memory/AlignedClass.h"

///
namespace Kit {
///
namespace Memory {


/** This template class defines a concrete Allocator that STATICALLY allocates
    all of its Memory and can allocate up to N instances of the specified Class.
    All of the memory is aligned to class "T" alignment boundaries.

    NOTES:

        1) If you only need memory for ONE instance - use AlignedClass structure
           in Aligned.h instead.

        2) The class is not inherently multi-thread safe.

        3) If the requested number of bytes on the allocate() method is greater
           than the block size (i.e. sizeof(T)), 0 is returned.

        4) The class can be deleted. However, it is the responsibility of the
           Application to properly clean-up/release ALL outstanding block
           allocations before deleting the SPool instance.


    Template args: class "T" is the type of class to allocated
                   int   "N" is the number of instances that can be allocate
 */

template <class T, int N>
class SPool : public Allocator
{
protected:
    /// Allocate blocks
    AlignedClass<T> m_blocks[N];

    /// Allocate memory for BlockInfo_ instances
    Pool::BlockInfo m_infoBlocks[N];

    /// My Pool work object
    Pool m_pool;

public:
    /** Constructor.  When the 'fatalErrors' argument is set to true, memory errors
        (e.g. out-of-memory) will generate a Kit::System::FatalError call. .
     */
    SPool( bool fatalErrors = false )
        : m_infoBlocks()
        , m_pool( m_infoBlocks, sizeof( T ), sizeof( AlignedClass<T> ), N, m_blocks, fatalErrors )
    {
    }

public:
    /// See Kit::Memory::Allocator
    void* allocate( size_t numbytes ) noexcept { return m_pool.allocate( numbytes ); }

    /// See Kit::Memory::Allocator
    void release( void* ptr ) noexcept { m_pool.release( ptr ); }

    /// See Kit::Memory::Allocator
    size_t wordSize() const noexcept { return m_pool.wordSize(); }

private:
    /// Prevent access to the copy constructor -->SPools can not be copied!
    SPool( const SPool& m );

    /// Prevent access to the assignment operator -->SPools can not be copied!
    const SPool& operator=( const SPool& m );
};


}   // end namespaces
}
#endif  // end header latch
