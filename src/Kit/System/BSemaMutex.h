#ifndef KIT_SYSTEM_BSEMAMUTEX_H_
#define KIT_SYSTEM_BSEMAMUTEX_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/BSemaphore.h"
#include <stdlib.h>


///
namespace Kit {
///
namespace System {

/** This concrete class provides mutual exclusion that is built on top of a
    Binary Semaphore.  Because a Binary Semaphore has NO concept of an 'owner',
    this class intentionally provides NEITHER of the following:

        o Recursive semantics.  A thread that calls lock() while it already owns
          the mutex will DEADLOCK itself.  Use Kit::System::Mutex when recursive
          semantics are required.

        o Priority Inheritance.  A low priority thread that owns the mutex will
          NOT be boosted while a higher priority thread is blocked waiting for
          it, i.e. unbounded priority inversion is possible.  Use
          Kit::System::Mutex when priority inheritance is required.

    In exchange, the behavior is identical on every platform since no native
    mutex is involved.

    NOTE: Because there is no 'owner', unlock() can be called from a different
          thread than the one that called lock().
 */
class BSemaMutex
{
public:
    /// Constructor.  The mutex is created in the unlocked state
    BSemaMutex() noexcept;

public:
    /** This method is invoked prior to entering a critical section.  If another
        thread currently "owns" the mutex, the current thread will "wait" until
        it can obtain ownership before proceeding.
     */
    void lock() noexcept;

    /** This method is invoked at the end of a critical section.  This call will
        release the ownership of the mutex.
     */
    void unlock() noexcept;


protected:
    /// The Binary Semaphore used to provide the mutual exclusion
    BSemaphore m_sema;


private:
    /// Prevent access to the copy constructor -->mutexes can not be copied!
    BSemaMutex( const BSemaMutex& m ) = delete;

    /// Prevent access to the assignment operator -->mutexes can not be copied!
    const BSemaMutex& operator=( const BSemaMutex& m ) = delete;

    /// Prevent access to the move constructor -->mutexes can not be implicitly moved!
    BSemaMutex( BSemaMutex&& m ) = delete;

    /// Prevent access to the move assignment operator -->mutexes can not be implicitly moved!
    BSemaMutex& operator=( BSemaMutex&& m ) = delete;

public:
    /** This concrete class provides a simple mechanism for providing mutex
        protection for a "scope block".  The class is instantiated as
        a local variable for the scope block it is meant to protected.  When
        this object is created - it calls lock() on its mutex.  When this
        object is destroyed - it calls unlock() on its mutex.
     */
    class ScopeLock
    {
    protected:
        /// Reference to the mutex to be used for synchronization
        BSemaMutex& m_mutex;

    public:
        /// Constructor.  This will block until the mutex lock is acquired.
        ScopeLock( BSemaMutex& mutex ) noexcept
            : m_mutex( mutex ) { m_mutex.lock(); }

        /// Destructor.  This method releases ownership of the mutex
        ~ScopeLock() { m_mutex.unlock(); }


    private:
        /** This method PREVENTS this object from being created on the Heap.
            This class can only/should only be instantiated as a local variable to
            a critical section scope block!
         */
        void* operator new( size_t );
    };
};


}  // end namespaces
}
#endif  // end header latch
