#ifndef KIT_MEMORY_CONTIGUOUS_ALLOCATOR_H_
#define KIT_MEMORY_CONTIGUOUS_ALLOCATOR_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Memory/Allocator.h"
#include <stdint.h>

///
namespace Kit {
///
namespace Memory {


/** This abstract class is used 'extend' the Memory Allocator to have contiguous
    semantics.  This means all memory that is allocated can be viewed as
    single contiguous block of memory.  In addition, the 'release()' method is
    hidden and replaced with a reset() method that releases/frees ALL memory
    allocated.
*/
class ContiguousAllocator : public Allocator
{
public:
    /// Structure for memory info/status
    struct MemoryInfo_T
    {
        /// Pointer to the start of the contiguous memory block
        void* m_memoryStartPtr;

        /// Total size of the contiguous block memory used by allocator in bytes
        size_t m_totalLenInBytes;

        /// Maximum number of 'words' in the block of memory.  Note: Unit is 'words' NOT bytes (i.e. word size is defined by wordSize() method)
        size_t m_maxWords;

        /// Number of remaining/free 'words' in the block of memory.
        size_t m_freeWords;
    };

public:
    /** Resets the allocator, i.e. effectively frees all allocated memory.  It
        is the caller's RESPONSIBILTY to ensure that it is kosher to free all
        of the memory.
     */
    virtual void reset() noexcept = 0;

public:
    /** This method returns information about Allocator's contiguous memory
        and allocate/free status. The return value is reference to the 'dstInfo'
        argument.

        NOTE: This method has GREAT POWER and it is the responsibility of the
              Application to use it correctly (Uncle Ben 2002).
     */
    virtual MemoryInfo_T& getMemoryStart( MemoryInfo_T& dstInfo ) noexcept = 0;


private:
    /** Hide/disable the individual release method (it does not fit the
        contiguous semantics).  If it gets called via the parent 'Allocator'
        interface - it does NOTHING
     */
    void release( void* ptr ) noexcept {};
};


};  // end namespaces
};
#endif  // end header latch
