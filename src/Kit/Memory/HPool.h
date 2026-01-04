#ifndef KIT_MEMORY_HPOOL_H_
#define KIT_MEMORY_HPOOL_H_
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


/** This template class defines a concrete Allocator that allocates memory from
    the HEAP for a memory pool that can create up to N concurrent instances of
    class T.  However, once the initial set of blocks are allocated, no more
    heap operations are performed.  All of the memory is aligned to  class "T"
    alignment boundaries.

    NOTES:

        1) If you only need memory for ONE instance - use AlignedClass structure
           in Aligned.h instead.

        2) The class is not inherently multi-thread safe.

        3) If the requested number of bytes on the allocate() method is greater
           than the block size (i.e. sizeof(T)), 0 is returned.

        4) The class can be deleted. However, it is the responsibility of the
           Application to properly clean-up/release ALL outstanding block
           allocations before deleting the HPool instance.


    Template args: class "T" is the type of class to allocated
 */

template <class T>
class HPool : public Allocator
{
protected:
    /// Allocate memory for BlockInfo_ instances
    Pool::BlockInfo* m_infoBlocks;

    /// Allocate blocks
    AlignedClass<T>* m_blocks;

    /// My Pool work object
    Pool* m_poolPtr;


public:
    /** Constructor.  When the 'fatalErrors' argument is set to true, memory errors
        (e.g. out-of-memory) will generate a Kit::System::FatalError call.
     */
    HPool( size_t maxNumBlocks, bool fatalErrors = false )
        : m_infoBlocks( new Pool::BlockInfo[maxNumBlocks]() )
        , m_blocks( new AlignedClass<T>[maxNumBlocks] )
        , m_poolPtr( new Pool( m_infoBlocks, sizeof( T ), alignof( AlignedClass<T> ), maxNumBlocks, m_blocks, fatalErrors ) )
    {
    }


    /// Destructor.
    ~HPool()
    {
        delete m_poolPtr;
        delete[] m_blocks;
        delete[] m_infoBlocks;
    }


public:
    /// See Kit::Memory::Allocator
    void* allocate( size_t numbytes ) noexcept { return m_poolPtr->allocate( numbytes ); }

    /// See Kit::Memory::Allocator
    void release( void* ptr ) noexcept { m_poolPtr->release( ptr ); }

    /// See Kit::Memory::Allocator
    size_t wordSize() const noexcept { return m_poolPtr->wordSize(); }

private:
    /// Prevent access to the copy constructor -->HPools can not be copied!
    HPool( const HPool& m );

    /// Prevent access to the assignment operator -->HPools can not be copied!
    const HPool& operator=( const HPool& m );
};


}   // end namespaces
}
#endif  // end header latch
