#ifndef KIT_MEMORY_ALLOCATOR_H_
#define KIT_MEMORY_ALLOCATOR_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/ListItem.h"
#include <stdlib.h>


///
namespace Kit {
///
namespace Memory {


/** This abstract class defines the interface for a Memory Allocator.  A Memory
    Allocator manages a pool memory that is assigned/released to/from clients
    at run-time (i.e. provides the memory for "dynamic" memory allocations).

    The following is an example on how to dynamically create/destroy object
    using the memory provided by a Allocator object:

    @code

    Example of placement new:
    -------------------------
    #include <new>
    #include "Kit/Memory/Allocator.h"

    class Foo { .... };

    Foo* newMe( Allocator& src, ...)
    {
        // Get a chunk memory large enough to contain an instance of Foo
        void* mem = src.allocate(sizeof(Foo));

        // Create an instance Foo using placement new
        if ( mem )
        {
            return new(mem) Foo(...);
        }
        else
        {
            // error condition: not enough memory. Do...
            return 0;
        }
    }


    Example of delete for an object created with by placement new:
    --------------------------------------------------------------
    void deleteMe( Foo* ptr, Allocator& src )
    {
        // Since delete is not called ->I have to Explicitly call the destructor!
        ptr->~Foo();

        // Release the memory back to the MemorySource
        src.release(ptr);
    }


    @endcode
*/
class Allocator : public Kit::Container::ListItem
{
public:
    /** Allocate and returns a pointer to at least numBytes of storage.
        Returns 0 if out of memory.
     */
    virtual void* allocate( size_t numbytes ) noexcept = 0;

    /** Frees memory that was allocated via the allocate() method.  It is VERY
        IMPORTANT that the memory is allocate() and release() from/to the
        SAME MemorySource Instance!

        NOTES:

            1) Freeing a nullptr pointer causes release() to return immediately
               without any errors.

            2) The pointer released() MUST be the same value as the pointer
               that was returned by the allocate().  This is particularly
               important if the memory allocated is used to create a
               concrete object that inherits multiple abstract/virtual
               interfaces.  When deleting the object, the pointer value
               passed to the release() method must be a pointer type of
               concrete object - NOT a pointer to any of its parent classes.
               This is required to prevent possible memory leaks because of
               the C++ magic where the actual pointer value to the concrete
               object is not necessarily the same pointer value when the pointer
               is up-cast to one of its abstract base classes!
     */
    virtual void release( void *ptr ) noexcept = 0;

public:
    /** Returns the 'word' size of the allocator, i.e. chunks of memory are
        allocate in multiple of this size. For example: Given a word size of 4
        and memory request for 5 bytes - the allocated would actually allocate
        8 bytes of memory, i.e. 2 * 4 >= 5
     */
    virtual size_t wordSize() const noexcept = 0;

    /** Convenience method that determines the actual amount of memory that
        actually allocated for a successful allocate request of N bytes.
     */
    inline size_t allocatedSizeForNBytes( size_t nbytes ) const noexcept
    {
        size_t ws = wordSize();
        return ((nbytes + ws - 1) / ws) * ws;
    }

public:
    /// Provide a virtual destructor
    virtual ~Allocator() = default;
};


}      // end namespaces
}
#endif  // end header latch
